/*22281285万绍文
内部命令：cd, echo, pwd, exit, date, type, export
*/

#include "function.h"

int main() {

    while(1) {
        char command_V[Max_size];
        memset(command_V, '\0', sizeof(command_V));
        
        getcwd(current_path, Max_size); //获取当前路径
        printf("\033[1;32mwanshaowen@Linux\033[0m:\033[1;34m%s\033[0m$ ", current_path); //输出提示信息
        fgets(command_V, Max_size, stdin);
        command_V[strlen(command_V) - 1] = '\0';

         // 分割命令行
        command_argV[0] = strtok(command_V, " ");
        int index = 1;
        if(strcmp(command_argV[0], "ls") == 0) {
            command_argV[index++] = "--color=auto"; // 更加美观
        }

        if(strcmp(command_argV[0], "ll") == 0) {
            command_argV[0] = "ls";
            command_argV[index++] = "-l";
            command_argV[index++] = "--color=auto";
        }

        while(command_argV[index++] = strtok(NULL, " "));

        if(strcmp(command_argV[0], "cd") == 0 && command_argV[1] != NULL) {
            //路径一定是一个不含空格的字符串
            if(command_argV[2] != NULL) {
                printf("cd: No such file or directory\n"); 
            }
            else changedir(command_argV[1]); //command_argV[1]是需要进入的路径
            continue;
        }

        if(strcmp(command_argV[0], "echo") == 0) {
            echo_command(command_argV);
            continue;
        }

        if(strcmp(command_V, "pwd") == 0) {
            showdir();
            continue;
        }

        if(strcmp(command_argV[0], "type") == 0) {
            for(int i = 1; command_argV[i] != NULL; i++) {
                if(is_internal_commands(command_argV[i])) {
                    printf("%s is a shell builtin\n", command_argV[i]);
                }
                else {
                    printf("%s is an external command\n", command_argV[i]);
                    find_external_command(command_argV[i]);
                }
            }
        }

        if (strcmp(command_argV[0], "export") == 0) {
            export_command(command_argV);  // 处理 export 命令
            continue;
        }


        if(strcmp(command_V, "exit") == 0) {
            exit(0);
        }

        pid_t id = fork();
        if(id == 0) {
            //进程替换
            execvp(command_argV[0], command_argV);
            exit(-1); // 替换失败则退出
        }

        int status = 0;
        pid_t res = waitpid(id, &status, 0);
        if(res < 0) {
            exit(-1);
        }

    }
    return 0;
}