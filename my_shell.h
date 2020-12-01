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
#define TOKENS " \\t\r\n\a&"


char* shell_read_line();
char** split(char* line);
void execute(char** pString,int flag);
void shell_loop();
void launch(char **args,int flag);

#endif //SHELL_MY_SHELL_H
