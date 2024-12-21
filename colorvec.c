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
  clr->a = 0xff;
}

/*
 * Initialize a hex color from input unsigned int:
 * 0xRRGGBB or 0xAARRGGBB
 */
void initColorHex(struct ColorVec *clr, unsigned int hex) {
  clr->r = hex>>16;
  clr->g = hex>>8;
  clr->b = hex>>0;
  if ((clr->a = hex>>24) == 0x00) // Assume missing Alpha component means full alpha
    clr->a = 0xff;
}

void printColorVec(struct ColorVec *clr) {
  printf("Red:%d\nGreen:%d\nBlue:%d\nAlpha:%d\n",clr->r,clr->g,clr->b,clr->a);
}


void addColorToColorVecGradient(struct ColorVecGradient *cvg, struct ColorVec *clr) {
  if (cvg->n_colors == MAX_GRADIENT_COLORS) return;
  cvg->gradient[cvg->n_colors].r = clr->r;
  cvg->gradient[cvg->n_colors].g = clr->g;
  cvg->gradient[cvg->n_colors].b = clr->b;
  cvg->gradient[cvg->n_colors].a = clr->a;
  cvg->n_colors = cvg->n_colors + 1;
}

struct ColorVec getColorFromGradient(struct ColorVecGradient *cvg, float degree) {
  struct ColorVec ret = { .a = 0xff };
  float roughIndex;
  int indexFloor, indexCeil;
  int n_colors = cvg->n_colors;

  if (n_colors == 1)
    return cvg->gradient[0];

  if (cvg->loop == 1)
    n_colors++;

  roughIndex = degree * (float)(n_colors-1);
  indexFloor = floorf(roughIndex);

  if (cvg->loop == 1 && indexFloor == cvg->n_colors-1)
    indexCeil = 0;
  else
    indexCeil = indexFloor + 1;

  if (degree <= 0.0f)
    return cvg->gradient[0];
  if (degree >= 1.0f) {
    if (cvg->loop == 1) 
      return cvg->gradient[0];
    else
      return cvg->gradient[n_colors-1];
  }

  float normalizedDegree = roughIndex - (float)indexFloor;
  ret.r = normalizedDegree * (float)cvg->gradient[indexCeil].r;
  ret.g = normalizedDegree * (float)cvg->gradient[indexCeil].g;
  ret.b = normalizedDegree * (float)cvg->gradient[indexCeil].b;
  ret.r += (1 - normalizedDegree) * (float)cvg->gradient[indexFloor].r;
  ret.g += (1 - normalizedDegree) * (float)cvg->gradient[indexFloor].g;
  ret.b += (1 - normalizedDegree) * (float)cvg->gradient[indexFloor].b;
  return ret;
}

void printGradient(struct ColorVecGradient *gradient) {
	int i;
	printf("Gradient: %d colors [R,G,B,A] ");
	printf((gradient->loop) ? "(Looping)\n" : "(Non-Looping)\n");
	for (i = 0; i < gradient-> n_colors; i++) {
		struct ColorVec clr = gradient->gradient[i];
		printf("%d:[%d,%d,%d,%d]\n",i,clr.r,clr.g,clr.b,clr.a);
	}
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

struct ColorVec invertColor(struct ColorVec *clr) {
	struct ColorVec ret;
	ret.r = 0xff - clr->r;
	ret.g = 0xff - clr->g;
	ret.b = 0xff - clr->b;
	ret.a = 0xff;
	return ret;
}
 
