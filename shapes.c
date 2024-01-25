/** 
 * shapes.c: draw various shapes, from square to other ones
 *
 * Uses ColorVec.h to coordinate colors.
 */
#include "ColorVec.h"
#include <stdio.h>
#include "canvas.h"


/**
 * Draw a square of a set color!
 *
 * Needs to be improved to support other colors (easy fix, just lazy)
 * Also, there are too many arguments. How can we simplify to just x, y, width?
 *
 * @TODO: SIMPLIFY TO drawSquare(x,y,width,color);
 *  - unsigned char *image: could contain a data structure that includes rowlength....
 *  think about that - would simplify all functions in this file... with just an image to worry
 *  about instead of all these extra shitty parameters that just clog... they just clog.
 */
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

/**
 * Draw a square of a set gradient!
 *
 * Needs to be improved to support other gradients.
 */
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

/**
 * Draws a line very naively, with a resolution... lollllll...
 * Must be improved with a line-drawing algorithm. I am thinking Bresenham's
 * for simplicity and .. honestly I don't like antialiased lines that much...
 * Perhaps we can have another function drawAntiAliasedLine.. perhaps.
 */
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

/**
 * Draw a line with a decided color. Can merge with above function at some point.
 *
 * ADDITIONAL IDEA: drawLineConGradient... a gradient line.. This would be good
 * for renders and such.
 */
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

/**
 * Plot a pixel. This should really have three arguments: x,y, and clr
 *
 * @TODO: START HERE FOR ARGUMENT SIMPLIFICATION... FISH OUT *image and rowlength
 * por favor
 */
int plot(unsigned char *image, int rowlength, int x, int y, struct ColorVec *clr) {
  unsigned char* pixelpointer = image + ((int)y)*rowlength + ((int)x)*BYTES_PER_PIXEL;
  *(pixelpointer + RED) = (unsigned char) *(clr->clr + RED);
  *(pixelpointer + GREEN) = (unsigned char) *(clr->clr + GREEN);
  *(pixelpointer + BLUE) = (unsigned char) *(clr->clr + BLUE);
}

/**
 * Draw a triangle. Utilizes other helper functions such as draw line.
 */
int drawTriangle(unsigned char *image, int rowlength, int t1, int t2, int t3) {
    //stub
}
