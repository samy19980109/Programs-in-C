#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"


/*
 * Read in the location of the pixel array, the image width, and the image
 * height in the given bitmap file.
 */
void read_bitmap_metadata(FILE *image, int *pixel_array_offset, int *width, int *height) {
  int check1 = fseek(image, 10, SEEK_SET);
  if (check1 != 0){
    fprintf(stderr, "Error: Cant seek to pixel array offset\n");
  }
  int check = fread(pixel_array_offset, sizeof(int), 1, image);
  if (check != 1){
    fprintf(stderr, "Error: Cant read pixel array\n");
  }

  check1 = fseek(image, 18, SEEK_SET);
  if (check1 != 0){
    fprintf(stderr, "Error: Cant seek to width\n");
  }
  check = fread(width, sizeof(int), 1, image);
  if (check != 1){
    fprintf(stderr, "Error: Cant read width\n");
  }

  check1 = fseek(image, 22, SEEK_SET);
  if (check1 != 0){
    fprintf(stderr, "Error: Cant seek to height\n");
  }
  check = fread(height, sizeof(int), 1, image);
  if (check != 1){
    fprintf(stderr, "Error: Cant read height\n");
  }
}

/*
 * Read in pixel array by following these instructions:
 *
 * 1. First, allocate space for m `struct pixel *` values, where m is the
 *    height of the image.  Each pointer will eventually point to one row of
 *    pixel data.
 * 2. For each pointer you just allocated, initialize it to point to
 *    heap-allocated space for an entire row of pixel data.
 * 3. Use the given file and pixel_array_offset to initialize the actual
 *    struct pixel values. Assume that `sizeof(struct pixel) == 3`, which is
 *    consistent with the bitmap file format.
 *    NOTE: We've tested this assumption on the Teaching Lab machines, but
 *    if you're trying to work on your own computer, we strongly recommend
 *    checking this assumption!
 * 4. Return the address of the first `struct pixel *` you initialized.
 */
struct pixel **read_pixel_array(FILE *image, int pixel_array_offset, int width, int height) {
  struct pixel **height1 = malloc(height * sizeof(struct pixel *));
  fseek(image, pixel_array_offset, SEEK_SET);
  int check = 0;
  for (int i  = 0; i < height; i++){
    height1[i] = malloc(sizeof(struct pixel) * width);
    for (int j = 0; j < width; j++){
      check = fread(&(height1[i][j].blue), sizeof(unsigned char), 1,image);
      if (check != 1){
        fprintf(stderr, "Error: Cant read blue\n");
      }
      check = fread(&(height1[i][j].green), sizeof(unsigned char), 1,image);
      if (check != 1){
        fprintf(stderr, "Error: Cant read green\n");
      }
      check = fread(&(height1[i][j].red), sizeof(unsigned char), 1,image);
      if (check != 1){
        fprintf(stderr, "Error: Cant read red\n");
      }
    }
  }
  return height1;
}


/*
 * Print the blue, green, and red colour values of a pixel.
 * You don't need to change this function.
 */
void print_pixel(struct pixel p) {
    printf("(%u, %u, %u)\n", p.blue, p.green, p.red);
}
