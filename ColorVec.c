#include <stdio.h>
#include "ColorVec.h"
#include <stdlib.h>
#define BLUE 0
#define GREEN 1
#define RED 2

struct ColorVec *initColor(unsigned char r, unsigned char g, unsigned char b) {
  struct ColorVec *clr = (struct ColorVec*)malloc(sizeof(struct ColorVec));
  *(clr->clr + RED) = r;
  *(clr->clr + GREEN) = g;
  *(clr->clr + BLUE) = b;
  return clr;
}

void printColorVec(struct ColorVec *clr) {
  printf("Red:%c\nGreen:%c\nBlue:%c\n",*clr->clr+BLUE,*clr->clr+GREEN,*clr->clr+BLUE);
}
