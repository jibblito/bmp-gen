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
#include "shapes.h"

/**
 * Plot a point on the canvas! The basic functionality-function of the
 * whole class of functions, yo!
 */
int plot(Canvas *cvs, int x, int y, ColorVec *clr) {
  if (x >cvs->width-1 || x < 0) {
    return 1;
  }
  if (y > cvs->height-1 || y < 0) {
    return 1;
  }
  unsigned int *pixpointer = cvs->image + y*cvs->width + x;
  // Format for canvas: 
  *pixpointer = 0x00000000;
  *pixpointer |= (unsigned int) clr->r << RSHIFT_RED;
  *pixpointer |= (unsigned int) clr->g << RSHIFT_GREEN;
  *pixpointer |= (unsigned int) clr->b << RSHIFT_BLUE;
  *pixpointer |= (unsigned int) clr->a << RSHIFT_ALPHA;
  return 0;
}


/**
 * Draw a rectangle of a set color
 */
void drawRect(Canvas *cvs, int x1, int y1, int x2, int y2, ColorVec* clr) {
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
 * Etch a rectangle of a set color
 */
void etchRect(Canvas *cvs, int x1, int y1, int x2, int y2, ColorVec *clr) {
  int x_start = x1, y_start = y1, x_end = x2, y_end = y2;

	// Assign Ordering of X and Y
  if (x1 > x2) {
    x_start = x2;
    x_end = x1;
  } 
	if (y1 > y2) {
		y_start = y2;
		y_end = y1;
	}

	int xr, yr;
	for (xr = x_start; xr <= x_end; xr++) {
		plot(cvs,xr,y_start,clr);
		plot(cvs,xr,y_end,clr);
	}
	for (yr = y_start; yr <= y_end; yr++) {
		plot(cvs,x_start,yr,clr);
		plot(cvs,x_end,yr,clr);
	}
}

/**
 * Etch a rectangle w/ width and height, aka the best way!
 */
void etchRectWH(Canvas *cvs, int x, int y, int width, int height, ColorVec *clr) {
	int xr = x, yr = y+1;
	for (; xr <= xr + width; xr++) {
		plot(cvs,xr,y,clr);
		plot(cvs,xr,y+width,clr);
	}
	for (; yr <= yr + height; yr++) {
		plot(cvs,x,yr,clr);
		plot(cvs,x+width,yr,clr);
	}
}

/**
 * Draws a line very naively, with a resolution... lollllll...
 * Must be improved with a line-drawing algorithm. I am thinking Bresenham's
 * for simplicity and .. honestly I don't like antialiased lines that much...
 * Perhaps we can have another function drawAntiAliasedLine.. perhaps.
 */
void drawLine(Canvas *cvs, int x1,int y1, int x2, int y2, ColorVec* clr) {

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

void drawLineAngle(Canvas *cvs, int x,int y, float radians, int length, ColorVec* clr) {
  int x_offset, y_offset;
  x_offset = sinf(radians) * length;
  y_offset = cosf(radians) * length;
}

void drawLineAngleSec(Canvas *cvs, int x,int y, float radians, int slength, int elength, ColorVec* clr) {
  if (slength > elength) return;
  int sx_offset, sy_offset, ex_offset, ey_offset;
  sx_offset = sinf(radians) * slength;
  sy_offset = cosf(radians) * slength;
  ex_offset = sinf(radians) * elength;
  ey_offset = cosf(radians) * elength;
  drawLine(cvs,x+sx_offset,y+sy_offset,x+ex_offset,y+ey_offset,clr);
}

/**
 * Etch (outline) a circle, dude!
 */
void etchCircle(Canvas *cvs, int x, int y, int radius, ColorVec *clr) {
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
void drawTriangle(unsigned char *image, int rowlength, int t1, int t2, int t3) {
    //stub
}
