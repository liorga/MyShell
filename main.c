#include "my_shell.h" //includes all defines and functions statements for the program

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

    //main loop of the program
    while (true) {
        if(getcwd(cwd,size) == NULL ){ //prints to the shell the current working dir
            perror("getcwd\n");
            exit(EXIT_FAILURE);
        }
        printf("LG_SHELL:%s(^_^): ",cwd); // prompt printing
        line = read_line(); //read line from user
        if(strlen(line) == 0){ //if line is empty then continue
            continue;
        }
        if (!strcmp(line, "exit")) { //handle the exit command from user
            exit(EXIT_SUCCESS);
        }
        //check the & sign and turn on flag if its in the line
        if(strchr(line,wait_sign) == NULL){
            flag_for_wait = 0;
        } else{
            flag_for_wait = 1;
        }

        // checks the redirection signs in the line and turn on the flag
        if((strchr(line,redi_input) != NULL) || (strchr(line,redi_output) != NULL)){
            io_flag = 1;
        } else{
            io_flag = 0;
        }


        args = split(line); // gets the command to char** to send later to execvp
        if(strcmp(args[0],"cd") == 0){//handle cd command from user
            my_cd(args);
        }
        //printf("io flag:%d\n",io_flag);
        if(io_flag){
            launch_io_put(args,flag_for_wait); // launch the redirection exec and handle > or < or < >
        } else {
            execute(args, flag_for_wait); // launch the regular commands
        }
        free(line); // free line
        free(args); // free args
    }
    free(cwd);
}

/**
 * read line from user using get line and put \0 insted of the \n read by getline
 * @return the input from user after \0 inserted
 */
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


/**
 * gets the input from the user and splits the sting into tiny strings using tokens that each word
 * is inserted into char** args one by one then put NULL in the end of args for later execvp
 *
 * @param line input from the user
 * @return char** of the command and the rest of the command args
 */
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

//launch the commands if the args is empty then print a usge error to the user
void execute(char** pString,int flag) {

    if (pString[0] == NULL) {
        // An empty command was entered.
        printf("No Command has entered\n");
        return;
    } else {
        launch(pString, flag);
    }
}

/**
 * open a new fork then launching the wanted command in the child process
 * if the & is on then wait
 */
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
 * launching the commands but for the redirection operators like < or > or < > combined
 * gets the indexs of the < > signs and open the right files thet comes after them to read or write
 * on or from them, then do the redirection by using dup and changing the stdin to the file read from and the stdout to be the file
 * @param args
 * @param flag
 */
void launch_io_put(char** args,int flag){
    pid_t pid;
    pid_t wpid;
    int status;
    int fd;
    int input = 0;
    int output = 0;
    int pos = find_first_io(args);
    int num = find_filename_index(args,&input,&output);
    num--;

    pid = fork();
    if (pid == 0) {
        // Child process
        if(input){
            //do input
            if ((fd = open(args[input+1], O_RDONLY)) == -1){
                perror(args[input+1]);
            }
            dup2(fd,0);
            close(fd);
        }
        if (output){
            //do output
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
//find the first time < or > appears and return its index
int find_first_io(char** args){
    int i = 0;
    for(i = 0 ; args[i] != NULL ; i++){
        if((strcmp(args[i],">") == 0) || (strcmp(args[i],"<") == 0)) {
            return i;
        }
    }
    return 0;
}
/**
 *
 * @param args list of word to iterate on
 * @param in sent from outside of the fun to be changed inside the function
 * @param out sent from outside of the fun to be changed inside the function
 * @return
 */
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
//my cd implemetation
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