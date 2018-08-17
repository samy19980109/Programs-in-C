#include <stdio.h>
#include <stdlib.h>

/* Return a pointer to an array of two dynamically allocated arrays of ints.
   The first array contains the elements of the input array s that are
   at even indices.  The second array contains the elements of the input
   array s that are at odd indices.

   Do not allocate any more memory than necessary.
*/
int **split_array(const int *s, int length) {
    int ** ret_array = malloc(sizeof(int*)*2);
    int odd = 0,even = 0;
    if (length%2 == 0){
	even = length /2 ;
	odd = length /2;
    }else{
	even = length /2 +1;
	odd = length /2;
    }
    int *odd_array = malloc(sizeof(int)*odd);
    int *even_array = malloc(sizeof(int)*even);
    int j = 0;
    for (int i =0; i < length; i+=2){
	even_array[j] = s[i];
	j++;	
    }
    int k =0;
    for (int i =1; i < length; i+=2){
	odd_array[k] = s[i];
	k++;	
    }
    ret_array[0] = even_array;
    ret_array[1] = odd_array;
    return ret_array;
}

/* Return a pointer to an array of ints with size elements.
   - strs is an array of strings where each element is the string
     representation of an integer.
   - size is the size of the array
 */

int *build_array(char **strs, int size) {
    int * ret = malloc(sizeof(int)*size);
    char * end;
    int j = 1;
    for (int i = 0; i < size; i++){
	ret[i] = strtol(strs[j], &end, 10);
	j++;
    }
    return ret;
}


int main(int argc, char **argv) {
    /* Replace the comments in the next two lines with the appropriate
       arguments.  Do not add any additional lines of code to the main
       function or make other changes.
     */
    int *full_array = build_array(argv ,argc-1);
    int **result = split_array(full_array, argc-1);

    printf("Original array:\n");
    for (int i = 0; i < argc - 1; i++) {
        printf("%d ", full_array[i]);
    }
    printf("\n");

    printf("result[0]:\n");
    for (int i = 0; i < argc/2; i++) {
        printf("%d ", result[0][i]);
    }
    printf("\n");

    printf("result[1]:\n");
    for (int i = 0; i < (argc - 1)/2; i++) {
        printf("%d ", result[1][i]);
    }
    printf("\n");
    free(full_array);
    free(result[0]);
    free(result[1]);
    free(result);
    return 0;
}
