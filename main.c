#include "my_shell.h"

int main(int argc, char* argv[]) {

    shell_loop();
    return 0;
}

void shell_loop() {
    char* line;
    size_t size = 128;
    char* cwd = (char*)malloc(size*sizeof(char));
    int res = 0;
    char** args;
    int true = 1;
    int flag_for_wait = 0;
    int io_flag = 0;
    char wait_sign = '&';
    char redi_output = '>';
    char redi_input = '<';

    while (true) {
        if(getcwd(cwd,size) == NULL ){
            perror("getcwd\n");
            exit(EXIT_FAILURE);
        }
        printf("%s (^_^)>  ",cwd);
        line = read_line();
        if (!strcmp(line, "exit")) {
            exit(EXIT_SUCCESS);
        }

        if(strchr(line,wait_sign) == NULL){
            flag_for_wait = 0;
        } else{
            flag_for_wait = 1;
        }

        if((strchr(line,redi_input) != NULL) || (strchr(line,redi_output) != NULL)){
            io_flag = 1;
        } else{
            io_flag = 0;
        }

        args = split(line);

        if(strcmp(args[0],"cd") == 0){
            my_cd(args);
        }
        //printf("io flag:%d\n",io_flag);
        if(io_flag){
            launch_io_put(args,flag_for_wait);
        } else {
            execute(args, flag_for_wait);
        }

        free(line);
        free(args);
    }
    free(cwd);

}

char* read_line() {
    char* line = NULL;

    ssize_t bufsize = 0; // have getline allocate a buffer for us
    if (getline(&line, &bufsize, stdin) == -1){
        perror("readline");
        exit(EXIT_FAILURE);
    }
    //getline reads the entire line including \n so this action will put \0 in the end of the string
    size_t size = 0;
    size = strlen(line);
    line[size-1] = '\0';

    return line;
}

char** pipe_parsing()

char** split(char* line) {
    int bufsize = TOKEN_BUFFER_SIZE;
    int pos = 0;
    char** words_arr = malloc(bufsize * sizeof(char*));
    char* single_word;

    if (words_arr == NULL) {
        fprintf(stderr, "malloc has failed error\n");
        exit(EXIT_FAILURE);
    }

    single_word = strtok(line, TOKENS);
    while (single_word != NULL) {
        words_arr[pos] = single_word;
        //printf("%s\n",single_word);
        pos++;
        if (pos >= bufsize) {
            bufsize += TOKEN_BUFFER_SIZE;

            words_arr = realloc(words_arr, bufsize * sizeof(char*));
            if (words_arr == NULL) {
                fprintf(stderr, "realloc has failed error\n");
                exit(EXIT_FAILURE);
            }
        }

        single_word = strtok(NULL, TOKENS);
    }
    words_arr[pos] = NULL; // put null in the end of string array for execv to know when the args are over

    return words_arr;
}

void execute(char** pString,int flag) {

    if (pString[0] == NULL) {
        // An empty command was entered.
        printf("No Command has entered\n");
        return;
    } else {
        launch(pString, flag);
    }
}



void launch(char **args,int flag)
{
    pid_t pid;
    pid_t wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        execvp(args[0], args);
        //if exec has returned then it has failed then i went the program to exit;
        exit(EXIT_FAILURE);
    }

    // Parent process
    if(!flag) {
        //check if returned -1
        wpid = waitpid(pid, &status, WUNTRACED);
    }
}


/**
 *
 * @param args
 * @param io_flag
 * @param flag
 */

void launch_io_put(char** args,int flag){
    pid_t pid;
    pid_t wpid;
    int status;
    int fd;
    //int fd1;
    int input = 0;
    int output = 0;
    int pos = find_first_io(args);
    int num = find_filename_index(args,&input,&output);
    num--;
    //printf("input:%d  output:%d\n",input,output);
    pid = fork();
    if (pid == 0) {
        // Child process
        if(input){
            //do input
            //printf("< has entered\n");
            if ((fd = open(args[input+1], O_RDONLY)) == -1){
                perror(args[input+1]);
            }
            dup2(fd,0);
            close(fd);
        }
        if (output){
            //do output
            //printf("> has entered\n");
            if ((fd = open(args[output+1], O_WRONLY | O_CREAT, 0644)) == -1){
                perror(args[output+1]);
            }
            dup2(fd,1);
            close(fd);
        }

        args[pos] = NULL;

        execvp(args[0], args);
        //if exec has returned then it has failed then i went the program to exit;
        exit(EXIT_FAILURE);

    }

    // Parent process
    if(!flag) {
        wpid = waitpid(pid, &status, WUNTRACED);
    }
}

int find_first_io(char** args){
    int i = 0;
    for(i = 0 ; args[i] != NULL ; i++){
        if((strcmp(args[i],">") == 0) || (strcmp(args[i],"<") == 0)) {
            return i;
        }
    }
}

int find_filename_index(char** args,int* in,int* out){
    int i = 0;
    for(i = 0 ; args[i] != NULL ; i++){
        if((strcmp(args[i],">") == 0)){
            *out = i;
        }
        if((strcmp(args[i],"<") == 0)){
            *in = i;
        }
    }
    return i;
}

void my_cd(char **args)
{
    if (args[1] == NULL) {
        fprintf(stderr, "expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("cd error");
        }
    }

}


