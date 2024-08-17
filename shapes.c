/** 
 * shapes.c: draw various shapes, from square to other ones
 *
 * Uses ColorVec.h to coordinate colors.
 */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "colorvec.h"
#include "canvas.h"

/**
 * Plot a point on the canvas! The basic functionality-function of the
 * whole class of functions, yo!
 */
int plot(struct Canvas *cvs, int x, int y, struct ColorVec *clr) {
  if (x >cvs->width-1 || x < 0) {
    return 1;
  }
  if (y > cvs->height-1 || y < 0) {
    return 1;
  }
  unsigned char* pixelpointer = cvs->image + ((int)y*cvs->rowlength + 
                                ((int)x) * BYTES_PER_PIXEL);
  *(pixelpointer + RED) = (unsigned char) clr->r;
  *(pixelpointer + GREEN) = (unsigned char) clr->g;
  *(pixelpointer + BLUE) = (unsigned char) clr->b;
}


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
 * Draw a rectangle of a set color
 */
int drawRect(struct Canvas *cvs, int x1, int y1, int x2, int y2, struct ColorVec* clr) {
  int x_start, y_start, x_end, y_end;

  if (x1 <= x2) {
    x_start = x1;
    x_end = x2;
    y_start = y1;
    y_end = y2;
  } else {
    x_start = x2;
    x_end = x1;
    y_start = y2;
    y_end = y1;
  }
  int i, j;
  for (i = x_start; i <= x_end; i++) {
    for (j = y_start; j <= y_end; j++) {
      plot(cvs,i,j,clr);
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
int drawLine(struct Canvas *cvs, int x1,int y1, int x2, int y2, struct ColorVec* clr) {

  int x_start, y_start, x_end, y_end;

  if (x1 <= x2) {
    x_start = x1;
    x_end = x2;
    y_start = y1;
    y_end = y2;
  } else {
    x_start = x2;
    x_end = x1;
    y_start = y2;
    y_end = y1;
  }

  int x_range = x_end-x_start;
  int y_range = y_end-y_start;

  int i;
  float slope = (float)y_range/(float)x_range;
  if (abs(x_range) > abs(y_range)) {
    for (i = 0; i <= x_range; i++) {
      int rel_y = (int)roundf(slope * (float)i);
      plot(cvs,x_start+i,y_start+rel_y,clr);
    }
  } else {
    if (y_range >= 0) {
      for (i = 0; i < y_range; i++) {
        int rel_x = (int)roundf((1.0f/slope) * (float)i);
        plot(cvs,x_start+rel_x,y_start+i,clr);
      }
    } else {
      for (i = 0; i >= y_range; i--) {
        int rel_x = (int)roundf((1.0f/slope) * (float)i);
        plot(cvs,x_start+rel_x,y_start+i, clr);
      }
    }
  }
}

int drawLineAngle(struct Canvas *cvs, int x,int y, float radians, int length, struct ColorVec* clr) {
  int x_offset, y_offset;
  x_offset = sinf(radians) * length;
  y_offset = cosf(radians) * length;
  return drawLine(cvs,x,y,x+x_offset,y+y_offset,clr);
}

int drawLineAngleSec(struct Canvas *cvs, int x,int y, float radians, int slength, int elength, struct ColorVec* clr) {
  if (slength > elength) return 0;
  int sx_offset, sy_offset, ex_offset, ey_offset;
  sx_offset = sinf(radians) * slength;
  sy_offset = cosf(radians) * slength;
  ex_offset = sinf(radians) * elength;
  ey_offset = cosf(radians) * elength;
  return drawLine(cvs,x+sx_offset,y+sy_offset,x+ex_offset,y+ey_offset,clr);
}

/**
 * Etch (outline) a circle, dude!
 */
int etchCircle(struct Canvas *cvs, int x, int y, int radius, struct ColorVec *clr) {
  int rel_x = radius;
  int rel_y = 0;
  plot(cvs,x+rel_x,y+rel_y,clr);
  while(rel_y < radius) { // Quadrant i
    if (rel_y < rel_x) {
      rel_y = rel_y + 1;
      rel_x = sqrtf(radius*radius-rel_y*rel_y);
    } else {
      rel_x = rel_x - 1;
      rel_y = sqrtf(radius*radius-rel_x*rel_x);
    }
    plot(cvs,x+rel_x,y+rel_y,clr);
  }

  while(rel_y > 0) { // Quadrant ii
     if (0-rel_x < rel_y) {
       rel_x = rel_x - 1;
       rel_y = sqrtf(radius*radius-rel_x*rel_x);
     } else {
      rel_y = rel_y - 1;
      rel_x = 0 - sqrtf(radius*radius-rel_y*rel_y);
    }
    plot(cvs,x+rel_x,y+rel_y,clr);
  }

  while(rel_x < 0) { // Quadrant iii
     if (rel_x < rel_y) {
       rel_y = rel_y - 1;
       rel_x = 0 - sqrtf(radius*radius-rel_y*rel_y);
     } else {
      rel_x = rel_x + 1;
      rel_y = 0 - sqrtf(radius*radius-rel_x*rel_x);
    }
    plot(cvs,x+rel_x,y+rel_y,clr);
  }

  while(rel_y < 0) { // Quadrant iv
     if (0 - rel_y > rel_x) {
       rel_x = rel_x + 1;
       rel_y = 0 - sqrtf(radius*radius-rel_x*rel_x);
     } else {
       rel_y = rel_y + 1;
       rel_x = sqrtf(radius*radius-rel_y*rel_y);
    }
    plot(cvs,x+rel_x,y+rel_y,clr);
  }
}

/**
 * Draw a triangle. Utilizes other helper functions such as draw line.
 */
int drawTriangle(unsigned char *image, int rowlength, int t1, int t2, int t3) {
    //stub
}
