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
#include <fcntl.h>


#define TOKEN_BUFFER_SIZE 64
#define TOKENS " \t\n&"


char* read_line();
char** split(char* line);
void execute(char** pString,int flag);
void shell_loop();
void launch(char **args,int flag);
void launch_io_put(char** args,int flag);
int find_filename_index(char** args,int* in,int* out);
void my_cd(char **args);
int find_first_io(char** args);

#endif //SHELL_MY_SHELL_H
