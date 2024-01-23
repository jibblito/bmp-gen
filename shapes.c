// shapes.c: draw various shapes, from square to other one's
#include "ColorVec.h"
#include <stdio.h>

extern const int BYTES_PER_PIXEL;

int drawSquare(unsigned char *image, int rowlength, int x, int y, int width) {
    int i,j;
    for (i = x; i < x + width; i++) {
      for (j = y; j < y + width; j++) {
        unsigned char* pixelpointer = image + i*rowlength + j*BYTES_PER_PIXEL;
        *(pixelpointer + 0) = (unsigned char) (200); //  Blue
        *(pixelpointer + 1) = (unsigned char) (20); // greene
        *(pixelpointer + 2) = (unsigned char) (55); // Red
      }

    }
}

int drawGradiSquare(unsigned char *image, int rowlength, int x, int y, int width) {
    int i,j;
    for (i = x; i < x + width; i++) {
      for (j = y; j < y + width; j++) {
        unsigned char* pixelpointer = image + i*rowlength + j*BYTES_PER_PIXEL;
        *(pixelpointer + 0) = (unsigned char) (((float)i/(float)width)*255.0f); //  Blue
        *(pixelpointer + 1) = (unsigned char) (20); // greene
        *(pixelpointer + 2) = (unsigned char) (255); // Red
      }
    }
}

int drawLine(unsigned char *image, int rowlength, int x1,int y1, int x2, int y2) {
  float x = x1;
  float y = y1;

  int rangex = x2-x1;
  int rangey = y2-y1;
  float resolution = 1000;

  while ((int)x < x2 || (int)y < y2) {
    // x = x + ((float)x1/(float)x2);
    // y = y + ((float)y1/(float)y2);

    x = x + (float)rangex/resolution;
    y = y + (float)rangey/resolution;

    unsigned char* pixelpointer = image + ((int)x)*rowlength + ((int)y)*BYTES_PER_PIXEL;
    *(pixelpointer + 0) = (unsigned char) (255);
    *(pixelpointer + 1) = (unsigned char) (255);
    *(pixelpointer + 2) = (unsigned char) (255);
  }

  unsigned char* pixelpointer = image + ((int)x)*rowlength + ((int)y)*BYTES_PER_PIXEL;
  *(pixelpointer + 0) = (unsigned char) (255);
  *(pixelpointer + 1) = (unsigned char) (255);
  *(pixelpointer + 2) = (unsigned char) (255);
}

int drawLineConColor(unsigned char *image, int rowlength, int x1,int y1, int x2, int y2, struct ColorVec *clr) {
  float x = x1;
  float y = y1;

  int rangex = x2-x1;
  int rangey = y2-y1;
  float resolution = 1000;

  printColorVec(clr);

  while ((int)x < x2 || (int)y < y2) {
    // x = x + ((float)x1/(float)x2);
    // y = y + ((float)y1/(float)y2);

    x = x + (float)rangex/resolution;
    y = y + (float)rangey/resolution;

    unsigned char* pixelpointer = image + ((int)x)*rowlength + ((int)y)*BYTES_PER_PIXEL;
    *(pixelpointer + 0) = (unsigned char) (255);
    *(pixelpointer + 1) = (unsigned char) (255);
    *(pixelpointer + 2) = (unsigned char) (255);
  }

  unsigned char* pixelpointer = image + ((int)x)*rowlength + ((int)y)*BYTES_PER_PIXEL;
  *(pixelpointer + 0) = (unsigned char) (255);
  *(pixelpointer + 1) = (unsigned char) (255);
  *(pixelpointer + 2) = (unsigned char) (255);
}

int plot(unsigned char *image, int rowlength, int x, int y, struct ColorVec *clr) {
  unsigned char* pixelpointer = image + ((int)y)*rowlength + ((int)x)*BYTES_PER_PIXEL;
  *(pixelpointer + RED) = (unsigned char) *(clr->clr + RED);
  *(pixelpointer + GREEN) = (unsigned char) *(clr->clr + GREEN);
  *(pixelpointer + BLUE) = (unsigned char) *(clr->clr + BLUE);
}

int drawTriangle(unsigned char *image, int rowlength, int t1, int t2, int t3) {
    //stub
}
