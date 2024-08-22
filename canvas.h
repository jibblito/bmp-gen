#ifndef CANVAS_H
#define CANVAS_H

#include <X11/Xlib.h>

/**
 * Defined as canvas.h when in reality it also deals with
 * bitmap writing -- I realize this violates one-file, one-function
 * later on the backends will be interchangeable (.bmp, .jpg, etc)
 */

#define MAX_FILENAME_LENGTH 32
#define BYTES_PER_PIXEL 3
#define FILE_HEADER_SIZE 14 // length in bytes of header
#define INFO_HEADER_SIZE 40 // length in bytes of info header

struct Canvas {
  int height, width;
  int rowlength;
  char name[MAX_FILENAME_LENGTH];
  unsigned char image[];
};

void generateBitmapImage(struct Canvas *cvs);
int flashCanvasToXImage(struct Canvas *cvs, XImage *xim, int offset_x, int offset_y);
unsigned char* createBitmapFileHeader(int height, int stride);
unsigned char* createBitmapInfoHeader(int height, int width);
struct Canvas *initCanvas(int width, int height, char *name);

#endif
