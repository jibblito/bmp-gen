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
#include <string.h>
#include <math.h>

void initColor(struct ColorVec *clr, unsigned char r, unsigned char g, unsigned char b) {
  clr->r = r;
  clr->g = g;
  clr->b = b;
}

void initColorHex(struct ColorVec *clr, unsigned long hex) {
  clr->r = hex>>(RED*8);
  clr->g = hex>>(GREEN*8);
  clr->b = hex>>(BLUE*8);
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
  float roughIndex;
  int indexFloor, indexCeil;
  if (degree < 0.0f) degree = 0.0f;
  if (degree > 1.0f) degree = 1.0f;
  if (cvg->n_colors == 1) return cvg->gradient[0];

  int n_colors = cvg->n_colors;
  if (cvg->loop == 1)
    n_colors++;
  roughIndex = degree * (float)(n_colors-1);
  indexFloor = floorf(roughIndex);
  if (cvg->loop == 1) {
    if (indexFloor == cvg->n_colors-1)
      indexCeil = 0;
  } else
    indexCeil = indexFloor + 1;

  float normalizedDegree = roughIndex - (float)indexFloor;
  ret.r = normalizedDegree * (float)cvg->gradient[indexCeil].r;
  ret.g = normalizedDegree * (float)cvg->gradient[indexCeil].g;
  ret.b = normalizedDegree * (float)cvg->gradient[indexCeil].b;
  ret.r += (1 - normalizedDegree) * (float)cvg->gradient[indexFloor].r;
  ret.g += (1 - normalizedDegree) * (float)cvg->gradient[indexFloor].g;
  ret.b += (1 - normalizedDegree) * (float)cvg->gradient[indexFloor].b;
  return ret;
}

struct ColorVec combineColors(struct ColorVec *a, struct ColorVec *b, float degree) {
  struct ColorVec ret;
  if (degree < 0) degree = 0;
  if (degree > 1) degree = 1;
  ret.r = degree * a->r + (1-degree) * b->r;
  ret.g = degree * a->g + (1-degree) * b->g;
  ret.b = degree * a->b + (1-degree) * b->b;
  return ret;
}
