#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXLINE 256
#define MAX_PASSWORD 10

#define SUCCESS "Password verified\n"
#define INVALID "Invalid password\n"
#define NO_USER "No such user\n"

int main(void) {
  char user_id[MAXLINE];
  char password[MAXLINE];

  if(fgets(user_id, MAXLINE, stdin) == NULL) {
      perror("fgets");
      exit(1);
  }
  if(fgets(password, MAXLINE, stdin) == NULL) {
      perror("fgets");
      exit(1);
  }

	int file_descriptor[2];
	if (pipe(file_descriptor) == -1){
		perror("pipe");
    exit(1);
	}

	int r = fork();
	if (r == -1){
		perror("fork");
    exit(1);
	}

	if (r == 0){
		// child process
		if (close(file_descriptor[1]) == -1){
			perror("close");
      exit(1);
		}
		if (dup2(file_descriptor[0], fileno(stdin)) == -1){
			perror("dup2");
      exit(1);
		}
	  if (execl("./validate", "validate", NULL) == -1){
			perror("execl");
      exit(1);
		}
		return 1;
	}
	else if(r > 0){
		// parent process
		if (close(file_descriptor[0]) == -1){
			perror("close");
      exit(1);
		}
		write(file_descriptor[1], user_id, MAX_PASSWORD);
		write(file_descriptor[1], password, MAX_PASSWORD);

		int ret_value;

		wait(&ret_value);

		if (WIFEXITED(ret_value)){
			int result = WEXITSTATUS(ret_value);
			if (result == 0){
				printf(SUCCESS);
			}
			else if (result == 2){
				printf(INVALID);
			}
      else if (result == 1){
        printf("Error\n");
      }
			else if (result == 3){
				printf(NO_USER);
			}
		}
	}
  return 0;
}
