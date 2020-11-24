
#include "my_shell.h"




int main(int argc, char* argv[]) {
	
	shell_loop();
	return 0;
}

void shell_loop() {
	char* line;
	char** args;
	int status = 1, size = 0;
	
	do {
		printf("> ");
		line = shell_read_line();
		args = shell_split_line(line);
		//status = shell_execute(args);
		size = sizeof(args)/sizeof(*args);
		//print()
		printf("%s %d\n",line,size);
		free(line);
		//free(args);
	} while (status);
}

char* shell_read_line() {
	char* line = NULL;
	ssize_t bufsize = 0; // have getline allocate a buffer for us
	if (getline(&line, &bufsize, stdin) == -1){
		if (feof(stdin)) {
			exit(EXIT_SUCCESS);  // We recieved an EOF
		} else  {
			perror("readline");
			exit(EXIT_FAILURE);
		}
	}
	return line;
}

char** shell_split_line(char* line) {
	int bufsize = TOKEN_BUFFER_SIZE, position = 0;
	char **tokens = malloc(bufsize * sizeof(char*));
	char *token;
	
	if (!tokens) {
		fprintf(stderr, "lsh: allocation error\n");
		exit(EXIT_FAILURE);
	}
	
	token = strtok(line, TOKEN_DELIM);
	while (token != NULL) {
		tokens[position] = token;
		position++;
		
		if (position >= bufsize) {
			bufsize += TOKEN_BUFFER_SIZE;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if (!tokens) {
				fprintf(stderr, "lsh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
		
		token = strtok(NULL, TOKEN_DELIM);
	}
	tokens[position] = NULL;
	printf("%d\n",position);
	return tokens;
}

int shell_execute(char** pString) {
	return 0;
}

void print(char *a)
{
	int i=0;
	printf("the list of names include : \n");
	while(*(a) != '\0') {
		printf("%s\n", *(a+i));
		i++;
	}
}
