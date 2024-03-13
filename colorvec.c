/**
 * ColorVec.c: Implementation of ColorVec.h
 *
 * Idea: Simplify graphical colors (3-channel) for an easy-to-use
 * data structure. In future may support alpha...
 *
 * Red-Green-Blue
 */

#include <stdio.h>
#include "colorvec.h"
#include <stdlib.h>

void initColor(struct ColorVec *clr, unsigned char r, unsigned char g, unsigned char b) {
  clr->r = r;
  clr->g = g;
  clr->b = b;
}

void printColorVec(struct ColorVec *clr) {
  printf("Red:%c\nGreen:%c\nBlue:%c\n",clr->r,clr->g,clr->b);
}
