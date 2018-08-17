#include <stdio.h>
// Add your other system includes here.
#include <sys/stat.h>
#include "ptree.h"
#include<string.h>
#include<stdlib.h>

// Defining the constants described in ptree.h
const unsigned int MAX_PATH_LENGTH = 1024;

// If TEST is defined (see the Makefile), will look in the tests
// directory for PIDs, instead of /proc.
#ifdef TEST
    const char *PROC_ROOT = "tests";
#else
    const char *PROC_ROOT = "/proc";
#endif


/*
 * Creates a PTree rooted at the process pid.
 * The function returns 0 if the tree was created successfully
 * and 1 if the tree could not be created or if at least
 * one PID was encountered that could not be found or was not an
 * executing process.
 */
 int generate_ptree(struct TreeNode **root, pid_t pid) {
    // Here's a way to generate a string representing the name of
    // a file to open. Note that it uses the PROC_ROOT variable.
    struct TreeNode *cur = NULL;
    char nameoffile[MAX_PATH_LENGTH];
    int pidnumber = 0;
    FILE *file;
    char procfile[MAX_PATH_LENGTH];
    if (sprintf(procfile, "%s/%d/exe", PROC_ROOT, pid) < 0) {
        fprintf(stderr, "sprintf could not produce a name for extern\n");
        return 1;
    }
    struct stat buff;
	  if(lstat(procfile, &buff) != 0){
		    return 1;
    }
	  (*root) = malloc(sizeof(struct TreeNode));

 	if (sprintf(procfile, "%s/%d/cmdline", PROC_ROOT, pid) < 0) {
        fprintf(stderr, "sprintf could not produce a name for cmdline\n");
        return 1;
    }
	file = fopen(procfile, "r");
	if (file == NULL){
		fprintf(stderr, "problem will fopen with cmdline'");
        return 1;
	}
	fscanf(file, "%s", nameoffile);
	fclose(file);
	char *name = malloc(sizeof(char) * (strlen(nameoffile)+1));
	strncpy(name, nameoffile, strlen(nameoffile)+1);
  	(*root)->pid = pid;
	(*root)->name = name;
	(*root)->child = NULL;
  	(*root)->sibling = NULL;
	
	if (sprintf(procfile, "%s/%d/task/%d/children", PROC_ROOT, pid, pid) < 0) {
        fprintf(stderr, "sprintf could not produce a name for children file\n");
        return 1;
    }
	file = fopen(procfile, "r");
	if (file == NULL){
		fprintf(stderr, "problem with fopen with children file");
        return 1;
	}
	while (fscanf(file, "%d", &pidnumber) != EOF){

		if ((*root)->child == NULL){
			    generate_ptree(&(*root)->child, pidnumber);
				cur = (*root)->child;
		}else{
			    generate_ptree(&cur->sibling, pidnumber);
			    cur = cur->sibling;
		}
	}
    	fclose(file);
    	return 0;
}


/*
 * Prints the TreeNodes encountered on a preorder traversal of an PTree
 * to a specified maximum depth. If the maximum depth is 0, then the
 * entire tree is printed.
 */
void print_ptree(struct TreeNode *root, int max_depth) {
    // declating a new temporary variable called height which stores the value of depth
    if (root!=NULL){
        static int depth = 0;
        if (max_depth == 0){
            int height = depth;
            printf("%*s%d: %s\n", depth * 2, "", root->pid, root->name);
            if (root->child != NULL){
                depth++;
                print_ptree(root->child, max_depth);
            }
            if (root->sibling != NULL){
                depth = height;
                print_ptree(root->sibling, max_depth);
            }
        }else{
            int height = depth;
            printf("%*s%d: %s\n", depth * 2, "", root->pid, root->name);
            if (root->child != NULL && depth+1 <= max_depth){
                depth++;
                print_ptree(root->child, max_depth);
            }
            if (root->sibling != NULL && height <= max_depth){
                depth = height;
                print_ptree(root->sibling, max_depth);
            }
        }
    }
}
