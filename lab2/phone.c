#include <stdio.h>
#include <stdlib.h>

int main(){
  char phone[11];
  int i;
  scanf("%10s%d", phone, &i);
  if (i == 0){
  printf("%s\n", phone);
  return 0;
  }
  else if (i > 0 && i < 10){
    printf("%c\n", phone[i]);
    return 0;
  }
  else if (i < 0 || i > 9){
    printf("ERROR\n");
    return 1;
  }
}
