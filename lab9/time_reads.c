/* The purpose of this program is to practice writing signal handling
 * functions and observing the behaviour of signals.
 */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

/* Message to print in the signal handling function. */
#define MESSAGE "%ld reads were done in %ld seconds.\n"

/* Global variables to store number of read operations and seconds elapsed. 
 */
long num_reads, seconds;

// a function for the handler
void handler(int code){
	fprintf(stdout, MESSAGE, num_reads, seconds);
	exit(0);
}


/* The first command line argument is the number of seconds to set a timer to run.
 * The second argument is the name of a binary file containing 100 ints.
 * Assume both of these arguments are correct.
 */
int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: time_reads s filename\n");
        exit(1);
    }
    seconds = strtol(argv[1], NULL, 10);

    FILE *fp;
    if ((fp = fopen(argv[2], "r")) == NULL) {
      perror("fopen");
      exit(1);
    }

	struct itimerval timer;
	struct sigaction newact;


	 newact.sa_handler = handler;
	 sigaction(SIGPROF, &newact, NULL);


	 timer.it_value.tv_sec = seconds;
	 timer.it_value.tv_usec = 0;

	 timer.it_interval.tv_sec = seconds;
	 timer.it_interval.tv_usec = 0;

	 setitimer(ITIMER_PROF, &timer, NULL);

	// creating an integer to store the value read by fread
	int store;


    /* In an infinite loop, read an int from a random location in the file,
     * and print it to stderr.
     */
    for (;;) {
		// creating a random integer
		int j = (random() % 100) * 4;

		// fseeking to that random integer location
		if (fseek(fp, j, SEEK_SET) != 0){
		  perror("fseek");
		  exit(1);
		}

		// fread the integer at that location
		if (fread(&store, sizeof(int), 1, fp) != 1){
			fprintf(stderr, "fread failed\n");
			exit(1);
		}

		// printing the read in integer
		// printf("The Number read in is: %d\n", store);
 
		num_reads++;
	
    }
    return 1; // something is wrong if we ever get here!
}
