#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

#define Max_size 128
#define true 1
#define false 0

char* command_argV[Max_size];
char current_path[Max_size];

// 内部命令列表
char* internal_commands[] = {"cd", "echo", "exit", "type", "pwd", "date", NULL};

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
    for(int i = 0; i < 6; i++) {
        if(strcmp(command, internal_commands[i]) == 0) {
            return true;
        }
    }
    return false;
}

void find_external_command(char* command) {
    char* path_env = getenv("PATH");  // 获取 PATH 环境变量
    if (path_env == NULL) {
        printf("PATH not found\n");
        return;
    }

    // 创建一个副本，因为 strtok 会修改原字符串
    char path_copy[Max_size];
    strncpy(path_copy, path_env, Max_size);

    // 分割 PATH 环境变量
    char* path = strtok(path_copy, ":");
    while (path != NULL) {
        char full_path[Max_size];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, command);

        // 检查命令是否存在于该路径中，且可执行
        if (access(full_path, F_OK) == 0) {
            printf("%s is %s\n", command, full_path);  // 找到命令路径，打印出来
            return;
        }

        path = strtok(NULL, ":");
    }

    // 如果没有找到命令
    printf("type: %s: not found\n", command);
}

void auto_find_external_command(char* command) {
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

int main() {

    while(1) {
        char* command_V;
        char prompt[Max_size];

        getcwd(current_path, Max_size); // 获取当前路径
        snprintf(prompt, sizeof(prompt), "\033[1;32mwanshaowen@Linux\033[0m:\033[1;34m%s\033[0m$ ", current_path);

        // 使用 readline 获取用户输入，并添加历史记录
        command_V = readline(prompt);
        if (strlen(command_V) > 0) {
            add_history(command_V);  // 将命令加入历史
        }

        // 分割命令行
        command_argV[0] = strtok(command_V, " ");
        int index = 1;
        if (strcmp(command_argV[0], "ls") == 0) {
            command_argV[index++] = "--color=auto"; // 更加美观
        }

        if (strcmp(command_argV[0], "ll") == 0) {
            command_argV[0] = "ls";
            command_argV[index++] = "-l";
            command_argV[index++] = "--color=auto";
        }

        while (command_argV[index++] = strtok(NULL, " "));

        if (strcmp(command_argV[0], "cd") == 0 && command_argV[1] != NULL) {
            // 路径一定是一个不含空格的字符串
            if (command_argV[2] != NULL) {
                printf("cd: No such file or directory\n");
            } else {
                changedir(command_argV[1]); // command_argV[1] 是需要进入的路径
            }
            continue;
        }

        if (strcmp(command_argV[0], "echo") == 0) {
            echo_command(command_argV);
            continue;
        }

        if (strcmp(command_V, "pwd") == 0) {
            showdir();
            continue;
        }

        if (strcmp(command_argV[0], "type") == 0) {
            for (int i = 1; command_argV[i] != NULL; i++) {
                if (is_internal_commands(command_argV[i])) {
                    printf("%s is a shell builtin\n", command_argV[i]);
                } else {
                    printf("%s is an external command\n", command_argV[i]);
                    auto_find_external_command(command_argV[i]);
                }
            }
        }

        if (strcmp(command_V, "exit") == 0) {
            free(command_V);  // 释放 readline 分配的内存
            exit(0);
        }

        pid_t id = fork();
        if (id == 0) {
            // 进程替换
            execvp(command_argV[0], command_argV);
            exit(-1); // 替换失败则退出
        }

        int status = 0;
        pid_t res = waitpid(id, &status, 0);
        if (res < 0) {
            exit(-1);
        }

        free(command_V);  // 释放 readline 分配的内存
    }

    return 0;
}
