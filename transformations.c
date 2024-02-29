/** 
 * transformations.c: perform transformations on canvas pixels
 * or something like that, dude!
 */
#include <stdio.h>
#include <math.h>
#include "colorvec.h"
#include "canvas.h"

/**
 * Plot a point on the canvas! The basic functionality-function of the
 * whole class of functions, yo!
 */
int basic_shift(struct Canvas *cvs, int factor) {
  if (factor >cvs->width-1 || factor < 0) {
    fprintf(stderr,"X coordinate out of range for canvas!\n");
    return 1;
  }
}
