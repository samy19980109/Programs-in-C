#include <stdio.h>

int main(){
  char phone[11];
  int i;
  int flag = 0;
  scanf("%s\n", phone);
  while(scanf("%d", &i) != EOF){
    if (i == 0){
      printf("%s\n", phone);
    }
    else if (i > 0 && i < 10){
      printf("%c\n", phone[i]);
    }
    else if (i < 0 || i > 9){
      printf("ERROR\n");
      flag = 1;
    }
  }
  return flag;
}
