#ifndef SHAPES_H
#define SHAPES_H

#include "canvas.h"
#include "colorvec.h"

int plot(struct Canvas *cvs, int x, int y, struct ColorVec *clr);

void drawRect(Canvas *cvs, int x1, int y1, int x2, int y2, ColorVec* clr);
void fillRectWH(Canvas *cvs, int x, int y, int w, int h, ColorVec* clr);
void etchRect(Canvas *cvs, int x1, int y1, int x2, int y2, ColorVec* clr);
void etchRectWH(Canvas *cvs, int x, int y, int width, int height, ColorVec *clr);
void drawLine(Canvas *cvs, int x1,int y1, int x2, int y2, ColorVec* clr);
void drawLineAngle(Canvas *cvs, int x,int y, float radians, int length,  ColorVec* clr);
void drawLineAngleSec( Canvas *cvs, int x,int y, float radians, int slength, int elength,  ColorVec* clr);
void etchCircle(Canvas *cvs, int x, int y, int radius, ColorVec *clr);
void fillCircle(Canvas *cvs, int x, int y, int radius, ColorVec *clr);
void drawTriangle(unsigned char *image, int rowlength, int t1, int t2, int t3);

#endif
