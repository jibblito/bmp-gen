#ifndef CANVAS_H
#define CANVAS_H

/**
 * Defined as canvas.h when in reality it also deals with
 * bitmap writing -- I realize this violates one-file, one-function
 * later on the backends will be interchangeable (.bmp, .jpg, etc)
 */

const int BYTES_PER_PIXEL = 3;
const int FILE_HEADER_SIZE = 14; // length in bytes of header
const int INFO_HEADER_SIZE = 40; // length in bytes of info header
const int MAX_FILENAME_LENGTH = 32;

struct Canvas {
  int height, width;
  unsigned char *image;
  char *name[MAX_FILENAME_LENGTH];
};

void generateBitmapImage(unsigned char* image, int height, int width, char* imageFileName);
unsigned char* createBitmapFileHeader(int height, int stride);
unsigned char* createBitmapInfoHeader(int height, int width);
struct Canvas *initCanvas(int height, int width, char *name);

#endif
