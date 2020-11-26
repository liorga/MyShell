
#include "my_shell.h"




int main(int argc, char* argv[]) {
	
	shell_loop();
	return 0;
}

void shell_loop() {
	char* line;
	char** args;
	int status = 1;
	
	
	while (status) {
		printf("> ");
		line = shell_read_line();
		if (!strcmp(line, "exit")) {
			exit(EXIT_SUCCESS);
		}
		args = shell_split_line(line);
		//status = shell_execute(args);
		
		free(line);
		free(args);
	}
	
}

char* shell_read_line() {
	char* line = NULL;
	
	ssize_t bufsize = 0; // have getline allocate a buffer for us
	if (getline(&line, &bufsize, stdin) == -1){
/*		if (feof(stdin)) {
			exit(EXIT_SUCCESS);  // We recieved an EOF
		} else{*/
			perror("readline");
			exit(EXIT_FAILURE);
		//}
	}
	//getline reads the entire line including \n so this action will put \0 in the end of the string
	size_t size = 0;
	size = strlen(line);
	line[size-1] = '\0';
	
	
	return line;
}

char** shell_split_line(char* line) {
	int bufsize = TOKEN_BUFFER_SIZE;
	int pos = 0;
	char** words_arr = malloc(bufsize * sizeof(char*));
	char* single_word;
	
	if (!words_arr) {
		fprintf(stderr, "lsh: allocation error\n");
		exit(EXIT_FAILURE);
	}
	
	single_word = strtok(line, TOKENS);
	while (single_word != NULL) {
		words_arr[pos] = single_word;
		pos++;
		if (pos >= bufsize) {
			bufsize += TOKEN_BUFFER_SIZE;
			words_arr = realloc(words_arr, bufsize * sizeof(char*));
			if (!words_arr) {
				fprintf(stderr, "lsh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
		
		single_word = strtok(NULL, TOKENS);
	}
	words_arr[pos] = NULL; // put null in the end of string array for execv to know when the args are over
	
	return words_arr;
}

int shell_execute(char** pString) {
	//int i;
	
	if (pString[0] == NULL) {
		// An empty command was entered.
		return 1;
	}
	
	if(!strcmp(pString[0],"exit")){
		return 0;
	}
	
/*	for (i = 0; i < lsh_num_builtins(); i++) {
		if (strcmp(args[0], builtin_str[i]) == 0) {
			return (*builtin_func[i])(args);
		}
	}*/
	
	return lsh_launch(pString);
	//return 0;
}



int lsh_launch(char **args)
{
	pid_t pid;
	pid_t wpid;
	int status;
	
	pid = fork();
	if (pid == 0) {
		// Child process
		if (execvp(args[0], args) == -1) {
			perror("lsh");
		}
		exit(EXIT_FAILURE);
	} else if (pid < 0) {
		// Error forking
		perror("lsh");
	} else {
		// Parent process
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	
	return 1;
}

/*void print(char *a)
{
	int i=0;
	printf("the list of names include : \n");
	while(*(a) != '\0') {
		printf("%s\n", *(a+i));
		i++;
	}
}*/
