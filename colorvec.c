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


void addColorToColorVecGradient(struct ColorVecGradient *cvg, struct ColorVec *clr) {
  if (cvg->n_colors == MAX_GRADIENT_COLORS) return;
  cvg->gradient[cvg->n_colors].r = clr->r;
  cvg->gradient[cvg->n_colors].g = clr->g;
  cvg->gradient[cvg->n_colors].b = clr->b;
  cvg->n_colors = cvg->n_colors + 1;
}

struct ColorVec getColorFromGradient(struct ColorVecGradient *cvg, float degree) {
  struct ColorVec ret;
  if (degree < 0.0f) degree = 0.0f;
  if (degree > 1.0f) degree = 1.0f;
  if (cvg->n_colors == 1) return cvg->gradient[0];
  if (cvg->n_colors == 2) {
    ret.r = degree * (float)cvg->gradient[0].r + (1-degree) * (float)cvg->gradient[1].r;
    ret.g = degree * (float)cvg->gradient[0].g + (1-degree) * (float)cvg->gradient[1].g;
    ret.b = degree * (float)cvg->gradient[0].b + (1-degree) * (float)cvg->gradient[1].b;
  } else if (cvg->n_colors == 3) {
    ret.r = degree * (float)cvg->gradient[0].r + (1-degree) * (float)cvg->gradient[1].r;
    ret.g = degree * (float)cvg->gradient[0].g + (1-degree) * (float)cvg->gradient[1].g;
    ret.b = degree * (float)cvg->gradient[0].b + (1-degree) * (float)cvg->gradient[1].b;
  }
  return ret;
}
