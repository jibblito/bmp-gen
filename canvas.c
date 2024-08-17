/**
 * Implementation of canvas.h
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "canvas.h"



/**
 *  HEIGHT (4) | WIDTH (4) | ROWLENGTH (4) | NAME (32) | IMAGE (variable)
 */
struct Canvas *initCanvas(int width, int height, char *name) {
  size_t n_bytes = sizeof(struct Canvas) + height*width*BYTES_PER_PIXEL*sizeof(unsigned char);
  struct Canvas *cvs = malloc(n_bytes); 
  cvs->height = height;
  cvs->width = width;
  cvs->rowlength = width * BYTES_PER_PIXEL;
  snprintf(cvs->name,sizeof(cvs->name), name);
  return cvs;
}

/**
 * Open and write the bitmap image file, line by line..... Do it.
 */
void generateBitmapImage(struct Canvas *cvs) {
  int widthInBytes = cvs->width * BYTES_PER_PIXEL; // ex 90 * 3 = 270
  unsigned char padding[3] = {0,0,0};
  int paddingSize = (4 - widthInBytes % 4) % 4;
  int stride = (widthInBytes) + paddingSize;

  FILE* imageFile = fopen(cvs->name, "wb");

  unsigned char* fileHeader = createBitmapFileHeader(cvs->height, stride);
  fwrite(fileHeader, 1, FILE_HEADER_SIZE, imageFile);
  unsigned char* infoHeader = createBitmapInfoHeader(cvs->height, cvs->width);
  fwrite(infoHeader, 1, INFO_HEADER_SIZE, imageFile);

  int i;
  for (i = cvs->height-1; i >= 0; i--) {
    fwrite(cvs->image + (i * widthInBytes), BYTES_PER_PIXEL, cvs->width, imageFile);
    fwrite(padding, 1, paddingSize, imageFile);
  }
  fclose(imageFile);
}

/**
 * Generate an XImage struct for da image
 */
int flashCanvasToXImage(struct Canvas *cvs, XImage *xim) {
  if (cvs->height != xim->height) {
    fprintf(stderr,"Canvas height does not match XImage height\n");
    return 1;
  }
  if (cvs->width != xim->width) {
    fprintf(stderr,"Canvas width does not match XImage width\n");
    return 1;
  }
  int i,j;
  unsigned long pixel;
  for (i = 0; i < cvs->height; i++) {
    for (j = 0; j < cvs->width; j++) {
      memcpy(&pixel,cvs->image+i*cvs->rowlength+j*BYTES_PER_PIXEL,3);
      xim->f.put_pixel(xim,j,i,pixel);
    }
  }
  return 0;
}


/**
 * Bitmap file header.. Have not tooled with the settings of bitmaps,
 * this was the simplest way I could find to create bitmap images...
 * I'm sure there is a reference somewhere online that can help you
 * figure out how to make bitmaps with ... The comments sort of explain
 * the format of the File Header... Signature I'm not sure on.
 */
unsigned char* createBitmapFileHeader(int height, int stride) {
  size_t fileSize = FILE_HEADER_SIZE + INFO_HEADER_SIZE + (stride * height);

  static unsigned char fileHeader[] = {
      0,0,     /// signature
      0,0,0,0, /// image file size in bytes
      0,0,0,0, /// reserved
      0,0,0,0, /// start of pixel array
  };

  fileHeader[ 0] = (unsigned char)('B');
  fileHeader[ 1] = (unsigned char)('M');
  fileHeader[ 2] = (unsigned char)(fileSize      );
  fileHeader[ 3] = (unsigned char)(fileSize >>  8);
  fileHeader[ 4] = (unsigned char)(fileSize >> 16);
  fileHeader[ 5] = (unsigned char)(fileSize >> 24);
  fileHeader[10] = (unsigned char)(FILE_HEADER_SIZE + INFO_HEADER_SIZE);

  return fileHeader;
}

/**
 * Bitmap info header.. The fields somewhat explained in the comments. Obviously there
 * is a header size in the first. Includes width, height, number of color planes,
 * bits per pixel.
 * I do not screw with the other options such as compression, image size,
 * horizontal resolution, vertical resolution, colors in color table, or
 * important color count... I assume this will help with compression. Later.
 */
unsigned char* createBitmapInfoHeader (int height, int width)
{
    static unsigned char infoHeader[] = {
        0,0,0,0, /// header size               [0 - 3]
        0,0,0,0, /// image width               [4 - 7]
        0,0,0,0, /// image height              [8 -11]
        0,0,     /// number of color planes    [12-13]
        0,0,     /// bits per pixel            [14-15]
        0,0,0,0, /// compression               [16-19]
        0,0,0,0, /// image size                [20-23]
        0,0,0,0, /// horizontal resolution     [24-27]
        0,0,0,0, /// vertical resolution       [28-31]
        0,0,0,0, /// colors in color table     [31-35]
        0,0,0,0, /// important color count     [36-39]
    };

    infoHeader[ 0] = (unsigned char)(INFO_HEADER_SIZE);
    infoHeader[ 4] = (unsigned char)(width      );
    infoHeader[ 5] = (unsigned char)(width >>  8);
    infoHeader[ 6] = (unsigned char)(width >> 16);
    infoHeader[ 7] = (unsigned char)(width >> 24);
    infoHeader[ 8] = (unsigned char)(height      );
    infoHeader[ 9] = (unsigned char)(height >>  8);
    infoHeader[10] = (unsigned char)(height >> 16);
    infoHeader[11] = (unsigned char)(height >> 24);
    infoHeader[12] = (unsigned char)(1);
    infoHeader[14] = (unsigned char)(BYTES_PER_PIXEL*8);

    return infoHeader;
}
