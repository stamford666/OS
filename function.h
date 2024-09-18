#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define Max_size 128
#define true 1
#define false 0
#define internal_command_num 7

char* command_argV[Max_size];
char current_path[Max_size];

void changedir(char* new_path);

void showdir();

void echo_command(char** args);

int is_internal_commands(char* command);

void export_command(char** args);

void find_external_command(char* command);
    