#include<stdio.h>
#include<stdlib.h>
#include "filter.h"
#include<math.h>
#include<sys/types.h>
#include<unistd.h>
#include<signal.h>

int filter(int m, int read_fd, int write_fd){
  int num;
  int rer;
  while((rer = read(read_fd, &num, sizeof(int))) == sizeof(int)){
    if (num % m != 0){
      if (write(write_fd, &num, sizeof(int)) == -1){
        // perror("write");
        // return 1;
        if(signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
          perror("signal");
          exit(1);
        }
      }
    }
  }
  if (rer == -1){
    perror("read");
    return 1;
  }
  return 0;
}
