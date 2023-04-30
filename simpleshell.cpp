#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
using namespace std;

#define BUFFSIZE 255
#define MAX_CMD 32
#define MAX_CMD_LEN 100

char *argv[MAX_CMD];
char curPath[BUFFSIZE];
char cmdHis[MAX_CMD][MAX_CMD_LEN];
int argc = 0;
int should_run = 1;
int round = 0;

void history(int a) {
    if(a > round) {
        printf("no command");
        return;
    }
    int status = 0;
    int i = 0, j = 0;
    int len = strlen(cmdHis[round - a]);

    for(i, j; i < len; i++) 
        cmdHis[round][j++] = cmdHis[round - a][i];    
    cmdHis[round][j] = '\0';

    for(int i = 0; argv[i]; i++)
        argv[i] = NULL;
    argc = 0;

    for(i = 0; cmdHis[round - a][i]; i++) {
        if(cmdHis[round - a][i] != ' ' && status == 0) {
            argv[argc++] = cmdHis[round - a] + i;//adress of char before ' '
            status = 1;
        } else if(cmdHis[round - a][i] == ' ') {
            status = 0;
        }
    }
    argv[argc] = NULL;

}

void printHistory() {
    int i ,j;
    if(round < 10)
        for(i = 0; i <= round; i++)
            printf("%s\n",cmdHis[i]);
    else
        for(i = round - 9; i <= round; i++)
            printf("%s\n",cmdHis[i]);
    
}

int callCd(int arg) {   
    int result = 1;
    if(arg != 2) {
        printf("wrong instruction input\n");
    } else {
        int ret = chdir(argv[1]);
        if(ret) return 0;
    }

    if(result) {
        char *res = getcwd(curPath,BUFFSIZE);
        if(res == NULL) {
            printf("file path not exist\n");
        }
        return result;
    }
    return 0;
}

void do_pares() {//argvs dealing
    char buf[1024] = {};
    scanf("%[^\n]%*c",buf);

    int i = 0, j = 0;
    int status = 0;
    int len = strlen(buf);

    for(i, j; i < len; i++) 
        cmdHis[round][j++] = buf[i];    
    cmdHis[round][j] = '\0';

    // for(i = 0; i < len; i++) {
    //     if(buf[i] != ' ') cmd[i] = buf[i];
    //     else break;
    // }

    for(int i = 0; argv[i]; i++)
        argv[i] = NULL;
    argc = 0;

    for(i = 0; buf[i]; i++) {
        if(buf[i] != ' ' && status == 0) {
            argv[argc++] = buf + i;//adress of char before ' '
            status = 1;
        } else if(buf[i] == ' ') {
            status = 0;
            //buf[i] = 0;
        }
    }
    argv[argc] = NULL;
}

void do_execute() {//fork and prog run    
    string cmd = argv[0];
    if(cmd == "!!") history(1);
    else if(cmd == "!1") history(1);
    else if(cmd == "!2") history(2);
    else if(cmd == "!3") history(3);

    cmd = argv[0];      

    if(cmd == "history") {
        printHistory();

    } else if(cmd == "cd") { 
        callCd(argc);
    
    } else if(cmd == "pwd") { 
        getcwd(curPath,BUFFSIZE);
        printf("%s\n",curPath);
               
    } else if(cmd == "exit") {//chuxianle duoyude jincheng
        should_run = 0;
     
    } else {
        pid_t pid = fork();
        switch(pid) {
            case -1://fail to fork
                perror("fork");
                exit(EXIT_FAILURE);
                break;
            
            case 0://succeed to fork and go to prog
                execvp(argv[0], argv);//**********
                perror("execvp");
                exit(EXIT_FAILURE);

            default://the father process
                {
                    int st;
                    while(wait(&st) != pid);
                }
        }
    }   
}

int main(void) {
    while (should_run) {
        fflush(stdout);
        printf("xsh>");
        
        do_pares();
        do_execute();
        round++;
    }
    return 0;
}