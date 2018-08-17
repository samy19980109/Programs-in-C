#include <stdio.h>
#include <stdlib.h>
// rwx------
// TODO: Implement a helper named check_permissions that matches the prototype below.
int check_permissions(char *arr1, char *arr_reqd){
  for (int i = 0; i < 9; i+=3){
    if(arr_reqd[i] == 'r' && arr1[i] != 'r'){
      return 1;
    }else if (arr_reqd[i+1] == 'w' && arr1[i+1] != 'w'){
      return 1;
    }else if (arr_reqd[i+2] == 'x' && arr1[i+2] != 'x'){
      return 1;
    }
  }
  return 0;
}

int main(int argc, char** argv) {
    if (!(argc == 2 || argc == 3)) {
        fprintf(stderr, "USAGE: count_large size [permissions]\n");
        return 1;
    }
    // TODO: Process command line arguments.
    int fileSize;
    char * end;
    fileSize = strtol(argv[1], &end, 10);
    char *arr = argv[2];
    // ignoring first line
    char rubbish[1000];
    scanf("%[^\n]%*c", rubbish);
    // initializing a counter to keep track of how many files are there
    int counter = 0;
    // takign input line by line and assorting the various attributes into different variables
    char access[11], number[32], owner[32], group[32], size[32], month[32];
    char date[32], time[32], name[32];
    while(scanf("%s %s %s %s %s %s %s %s %s", access, number, owner, group,
    size, month, date, time, name) != EOF){
      int size_int = strtol(size, &end, 10);
      if (arr == NULL){
        if (size_int > fileSize){
          counter++;
        }
      }else{
        if (size_int >= fileSize && check_permissions(&access[1], arr) == 0){
          counter++;
        }
      }
    }
    // TODO Call check permissions as part of your solution to counting the files to
    // compute and print the correct value.
    printf("%d\n", counter);
    return 0;
}
