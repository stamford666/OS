#include "function.h"

// 内部命令列表
char* internal_commands[] = {"cd", "echo", "exit", "type", "pwd", "date", "export", NULL};

void changedir(char* new_path) {
    if(chdir(new_path) == -1) {
        printf("cd: %s: No such file or directory\n", new_path);
    }
}

void showdir() {
    printf("%s\n", current_path);
}

void echo_command(char** args) {
    int index = 1;
    while (args[index] != NULL) {
        for(int i = 0; i < strlen(args[index]); i++) {
            if(args[index][i] != '"' && args[index][i] != '\'') { //不输出引号
                printf("%c", args[index][i]);
            }
        }
        printf(" ");
        index++;
    }
    printf("\n");  // 输出完成后换行
}

int is_internal_commands(char* command) {
    for(int i = 0; i < internal_command_num; i++) {
        if(strcmp(command, internal_commands[i]) == 0) {
            return true;
        }
    }
    return false;
}

// 实现 export 命令，设置环境变量
void export_command(char** args) {
    if (args[1] == NULL) {
        // 不带参数时，显示所有环境变量
        extern char **environ;
        for (char **env = environ; *env != 0; env++) {
            printf("%s\n", *env);
        }
    } else {
        // 解析 "VAR=value" 格式
        char* var = strtok(args[1], "=");
        char* value = strtok(NULL, "=");
        
        if (var != NULL && value != NULL) {
            // 设置环境变量
            if (setenv(var, value, 1) == 0) {
                printf("Environment variable %s set to %s\n", var, value);
            } else {
                perror("setenv");
            }
        } else {
            printf("export: %s: not a valid identifier\n", args[1]);
        }
    }
}

void find_external_command(char* command) {
    pid_t id = fork();
    if (id == 0) {
        char *args[] = {"which", command, NULL};
        execvp("which", args);  
    }

    int status = 0;
    pid_t res = waitpid(id, &status, 0);
    if(res < 0) {
        exit(-1);
    }
}