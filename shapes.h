#ifndef SHAPES_H
#define SHAPES_H

#include "canvas.h"
#include "colorvec.h"

int plot(struct Canvas *cvs, int x, int y, struct ColorVec *clr);


int drawSquare(char *image, int rowlength, int x, int y, int width);
int drawRect(struct Canvas *cvs, int x1, int y1, int x2, int y2, struct ColorVec* clr);
int drawGradiSquare(unsigned char *image, int rowlength, int x, int y, int width);
int drawLine(struct Canvas *cvs, int x1,int y1, int x2, int y2, struct ColorVec* clr);
int etchCircle(struct Canvas *cvs, int x, int y, int radius, struct ColorVec *clr);
int drawTriangle(unsigned char *image, int rowlength, int t1, int t2, int t3);

#endif
