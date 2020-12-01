

#include "my_shell.h"




int main(int argc, char* argv[]) {
	
	shell_loop();
	return 0;
}

void shell_loop() {
	char* line;
	char** args;
	int true = 1;
	int flag_for_wait = 0;
	int input_flag = 0;
	int output_flag = 0;
	char wait_sign = '&';
	char redi_output = '>';
	char redi_input = '<';
	
	while (true) {
		printf(":))) ");
		line = read_line();
		if (!strcmp(line, "exit")) {
			exit(EXIT_SUCCESS);
		}
		
		if(strchr(line,wait_sign) == NULL){
			flag_for_wait = 0;
		} else{
			flag_for_wait = 1;
		}
		if (strchr(line,redi_output) == NULL){
			output_flag = 0;
		} else{
			output_flag = 2;
		}
		if (strchr(line,redi_input) == NULL){
			input_flag = 0;
		} else{
			input_flag = 1;
		}
		
		
		//printf("%s %d\n",line,flag_for_wait);
		
		args = split(line);
		
		execute(args,flag_for_wait,input_flag,output_flag);
		
		free(line);
		free(args);
	}
	
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

void execute(char** pString,int flag,int input,int output) {
	//int i;
/*	if(flag){
		printf("i am da i need to wait\n");
	} else{
		printf("i dont need to wait\n");
	}*/
	

	if (pString[0] == NULL) {
		// An empty command was entered.
		printf("No Command has entered\n");
		return;
	} else if(input || output){
		if (input){
			input = 1;
			launch_io_put(pString,input,flag);
		}
		if (output){
			output = 2 ;
			launch_io_put(pString,output,flag);
		}
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
		wpid = waitpid(pid, &status, WUNTRACED);
	}
	//printf("%d\n",wpid);
	
}

void launch_io_put(char** args,int io_flag,int flag){
	pid_t pid;
	pid_t wpid;
	int status;
	int fd;
	pid = fork();
	if (pid == 0) {
		// Child process
		//printf("flag is %d\n",io_flag);
		if(io_flag == 1){
			//do input
			if ((fd = open(args[1], O_RDONLY | O_CREAT, 0644)) == -1){
				perror(args[1]);
			}
			dup2(fd,0);
			close(fd);
		}
		if (io_flag == 2){
			//do output
			if ((fd = open(args[1], O_WRONLY | O_CREAT, 0644)) == -1){
				perror(args[1]);
			}
			dup2(fd,1);
			close(fd);
		}
		
		execv(args[0], args);
		//if exec has returned then it has failed then i went the program to exit;
		exit(EXIT_FAILURE);
		
	}

	// Parent process
	if(!flag) {
		wpid = waitpid(pid, &status, WUNTRACED);
	}
}
