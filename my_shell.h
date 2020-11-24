//
// Created by lior on 24/11/2020.
//

#ifndef SHELL_MY_SHELL_H
#define SHELL_MY_SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>


#define TOKEN_BUFFER_SIZE 64
#define TOKEN_DELIM " \\t\r\n\a"


char* shell_read_line();
char** shell_split_line(char* line);
int shell_execute(char** pString);
void shell_loop();
int lsh_launch(char **args);
void print(char *a);

#endif //SHELL_MY_SHELL_H
