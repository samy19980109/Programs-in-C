#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "filter.h"
#include <signal.h>

/*
* Main method for the assignment. This method runs a for loop inside which it forks.
*/
int main(int argc, char ** argv){

    // Check the correct input of arguments are provided to the program
    if (argc != 2){
        fprintf(stderr, "Usage:\n\tpfact n\n");
        exit(1);
    }

    // see if there is a siganl error
    if(signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
      perror("signal");
      exit(1);
    }

    // Check if the provided arguments are correct, the inpot should be more than 1
    char *if_letters;
    int inpot;
    inpot = strtol(argv[1], &if_letters, 10);

    // see if the provided arguments are correct or not
    if (inpot < 2 || strlen(if_letters) != 0){
        fprintf(stderr, "Usage:\n\tpfact n\n");
        exit(1);
    }

    // to get the squareroot of the input
    int sqr = sqrt(inpot);

    // making the file descriptor which will eventually become pipes
    int fd[(sqr) + 1][2];

    // a int to store the read in value
    int store = 2;

    // an array to store the factors
    int factors[2] = {0, 0};

    // a counter for the factors
    int b = 0;

    // the loop iterator
    int i;

    // we are going to run the for loo strictly lesser than the squareroot of the input number
    for (i = 0; store < sqr + 1; i++){

        // making the pipe for file descriptor fd[i]
        if (pipe(fd[i]) == -1) {
            perror("pipe");
            exit(1);
        }

        // the case when the input is a perfect square
        if (inpot == store * store){
            printf("%d %d %d\n", inpot, store, store);
            printf("Number of filters = %d\n", i);
            exit(0);
        }else {

        // Fork the children
        int r = fork();

        if (r > 0){
            // Parent processes

            // Since we dont need the read file descriptor of the Parent, we can shut it
            if ((close(fd[i][0])) == -1) {
                perror("close");
                exit(1);
            }

            if (i > 0){
              // in the second and more iteration, we must filter and then close both the pipes
              if (filter(store, fd[i - 1][0], fd[i][1]) != 0){
                  printf("filter error\n");
              }
              // Since we have finished reading, we can shut the file descriptor
              if (close(fd[i - 1][0]) == -1){
                  perror("close");
                  exit(1);
              }

              // Since we have finished writing, we can shut the file descriptor
              if (close(fd[i][1]) == -1){
                  perror("close");
                  exit(1);
              }
            } else {
                // In the first run of the loop, write the values from 2 to n to the file descriptor
                for (int j = 2; j <= inpot; j++){
                    if(write(fd[i][1], &j, sizeof(int)) == -1 ) {
                        exit(1);
                    }
                }

                // Since we have finished writing, we can shut the file descriptor
                if (close(fd[i][1]) == -1){
                    perror("close");
                    exit(1);
                }
            }

            // waiting for the child to return and then exiting with the appropriate exit value
            int wait_return;
            wait(&wait_return);
      	 		if(WIFEXITED(wait_return)){
      				if (i == 0){
      					printf("Number of filters = %d\n", WEXITSTATUS(wait_return));
                return 0;
      				}
      				exit(WEXITSTATUS(wait_return) + 1);
      			}
      			else{
              printf("some child aborted\n");
      				exit(1);
            }
        } else if (r == 0){
            // child process

            // Since the child never writes, we can shut the write file descriptor
            if (close(fd[i][1]) == -1){
                perror("close");
                exit(1);
            }
            // we must shut the read of the child to after reading in the value
            if(i > 0){
				       // closing the read of the child
				        if (close(fd[i - 1][0]) == -1) {
					             perror("close");
					             exit(1);
				        }
			      }

            // now we must read the first value
            if (read(fd[i][0], &store, sizeof(int)) == -1){
                perror("read");
                exit(1);
            }



            // if the input number has a factor and we find another factor which is less than the squareroot
            if (b == 1){
                if (inpot % store == 0 && store < sqr) {
                    printf("%d is not the product of two primes\n", inpot);
                    exit(0);
                }
            }
            // if the input number is a perfect square
            if (inpot == store * store){
                printf("%d %d %d\n", inpot, store, store);
                exit(0);
            }
            // if the input number has a factor and we find another factor which is more than the squareroot
            if (b == 1 && factors[1] == store && factors[1] >= sqr){
                printf("%d %d %d\n", inpot, factors[0], factors[1]);
                exit(0);
            }

            // if the current read in value is a factor is the input, then update factors
            if (b == 0 && inpot % store == 0){
                factors[0] = store;
                b++;
                factors[1] = inpot / store;
            }

        } else {
            perror("fork\n");
            exit(1);
        }
      }
    }
    if (b == 1 &&  factors[1] >= sqr){
        if ( factors[1] % factors[0] == 0){
            printf("%d is not the product of two primes\n", inpot);
            exit(0);
        }
        printf("%d %d %d\n", inpot, factors[0], factors[1]);
        exit(0);
    } else {
        if (i == 0){
            printf("%d is prime\n", inpot);
            printf("Number of filters = %d\n", 0);
            exit(0);
        }
        printf("%d is prime\n", inpot);
    }
    return 0;
}
