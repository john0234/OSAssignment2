#include <iostream>
#include <string>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wait.h>

using namespace std;

void runLSR(const char* dir, int tabs);
bool findCommand(string userInput, string* path);

int main() {
    char usr[128];
    char* ptr = usr;
    for(int i = 0; i < 128; i++)
    {
        usr[i] = '\0';
    }

    string path = getenv("PATH");
    char path1[1024];
    strncpy(path1,path.c_str(), sizeof(path1));
    string strArr[128];
    int i = 0;
    char * pch;
    pch = strtok(path1, ":");

    while(pch != NULL)
    {
        strArr[i] = pch;
        pch = strtok(NULL, ":");
        i++;
    }

    cout << "Welcome to OSShell! Please enter your commands ('exit' to quit)." << endl;

    while((string)usr != "exit"){
        cout << "osshell>";
        string temp;
        getline(cin,temp);

        for (int j = 0; j < temp.length(); ++j) {
            usr[j] = temp[j];
        }
        char * pch1;
        string parts[128];
        pch1 = strtok(usr," ");
        int k = 0;

        while(pch1 != nullptr){
            parts[k] = pch1;
            pch1 = strtok(nullptr," ");
            k++;
        }

        if(parts[0] == "exit"){
            break;
        }

        if(usr[0] == 'l' && usr[1] == 's' && usr[2] == 'r'){
            if(usr[3]=='\0' || usr[3]==' ' || usr[3] == '\0') {
                if (parts[2] != "\0"){

                    printf("lsr:Error running command\n");

                } else if (parts[1] == "\0") {

                    char currPath[1024];
                    if((getcwd(currPath, sizeof(currPath))) != NULL){
                        runLSR(currPath,0);
                    }

                } else if (parts[1] != "\0" && parts[2] == "\0") {
                    runLSR(parts[1].c_str(), 0);
                }
            }
        }
        else if(usr[0] == '\0'){
            continue;
        }
        else{
            string pathOfExe;
            string* pathOfExeP;
            pathOfExeP = &pathOfExe;

            if(findCommand(parts[0],pathOfExeP) == true && parts[1] != "\0"){
                pid_t PID = 0;
                PID = fork();
                string temp2 = parts[1];
                char * const command[] = {"nc", (char*)temp2.c_str(), NULL};
                int status;
                if(PID == 0){
                    execv(pathOfExe.c_str(), command);
                }
                else{
                    waitpid(PID, &status, 0);
                    WEXITSTATUS(status);
                }

            }else if(findCommand(parts[0],pathOfExeP) == true){
                pid_t PID = 0;
                PID = fork();
                char * const command[] = {"nc", NULL};
                int status;
                if(PID == 0){
                    execv(pathOfExe.c_str(), command);
                }
                else{
                    waitpid(PID, &status, 0);
                    WEXITSTATUS(status);
                }

            }else{

                printf("%s: Error running command\n", parts[0].c_str());
            }
        }
        for(int i = 0; i < 128; i++){
            usr[i] = '\0';
        }
    }
    printf("Thank you for using our shell!\n");
    return 0;
}

void runLSR(const char* dir, int tabs){

    DIR* d;
    struct dirent *dirp;
    d = opendir(dir);
    string dir_s = string(dir);
    if(d != NULL){

        while((dirp = readdir(d)) != nullptr){
            if(dirp->d_name[0] == '.'){
                continue;
            }

            struct stat s;
            string tabString = "";
            for(int i = 0; i < tabs*4; i++){
                tabString = tabString + " ";
            }

            string dirp_s = string(dirp->d_name);
            string temp = dir_s + '/' + dirp_s;

            if(stat(temp.c_str(), &s) == 0){

                if(S_ISDIR(s.st_mode)){
                    cout << "\033[0;34m" << tabString << dirp->d_name << " (directory)" << "\033[0m\n";
                    runLSR(temp.c_str(),tabs+1);
                }else{
                    if(S_IEXEC & s.st_mode){
                        cout << "\033[0;32m" << tabString << dirp->d_name << " (" << s.st_size << " bytes) \033[0m\n";

                    }else{
                        printf("%s%s", tabString.c_str(),dirp->d_name);
                        printf(" (%llu bytes)\n",(unsigned long long)s.st_size);
                    }
                }
                temp.clear();
            }
        }
        closedir(d);

    }else{
        //todo put something here
        printf("File not found.\n");
    }
}

bool findCommand(string userInput, string* path1){
    int i = 0;
    int temp2;
    char tab2[1024];
    char * pch;
    string path = getenv("PATH");
    string listOfPaths[128];
    string pathExec;
    string input;
    string test;
    string temp;
    string isExec;
    struct dirent *ent;
    bool isEx = true;
    struct stat sb;


    strncpy(tab2, path.c_str(), sizeof(tab2));
    pch = strtok (tab2,":");

    while (pch != NULL){
        listOfPaths[i] = pch;
        pch = strtok (NULL, ":");
        i++;
    }

    temp2 = i;
    i = 0;
    while(i < temp2){
        input = listOfPaths[i];
        DIR *dir = opendir(input.c_str());
        test = userInput; //user input
        if(dir)
        {
            while((ent = readdir(dir)) != NULL)
            {
                temp = ent->d_name;
                if( temp == test){
                    pathExec = listOfPaths[i]; // path to command
                    isExec = pathExec + "/" + ent->d_name;
                    if (stat(isExec.c_str(), &sb) == 0 && sb.st_mode & S_IXUSR) {
                        *path1 = isExec;
                        return true;//true if file is executable

                    }else{
                        isEx = false;//fasle if file is not executable
                        return false;
                    }
                }
            }
        }
        i++;
    }

    if(pathExec.empty()){
        return false;//false if command isn't found
    }
    if(isEx == false){
        return false;//false if command is not executable
    }
}



