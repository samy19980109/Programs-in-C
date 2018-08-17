#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include "ptree.h"


int main(int argc, char **argv) {
		struct TreeNode *root = NULL;
		if (argc == 2){
			int a = generate_ptree(&root, strtol(argv[1], NULL, 10));
			if (a == 1){
				fprintf( stderr, "generate_ptree failed\n");
			  return 2;
			}else{
				// printf("reached print_ptree statment\n");
				print_ptree(root, 0);
				// printf("excecuted successfully\n");
				return 0;
			}
		}else if (argc == 4){
			if (strcmp(argv[1], "-d") != 0){
				fprintf( stderr, "Usage:\n\tptree [-d N] PID\n");
				return 1;
			}
			int b = generate_ptree(&root, strtol(argv[3], NULL, 10));
	    if (b == 1){
				fprintf( stderr, "generate_ptree failed\n");
				return 2;
			}else{
				print_ptree(root, strtol(argv[2], NULL, 10));
				return 0;
			}
		}else{
			fprintf( stderr, "Usage:\n\tptree [-d N] PID\n");
			return 1;
		}
	//how to check is PID is a entryy in proc, if not then return 2
}
