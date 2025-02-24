#ifndef SHAPES_H
#define SHAPES_H

#include "canvas.h"
#include "colorvec.h"

int plot(struct Canvas *cvs, int x, int y, const ColorVec *clr);
int plot_hex(struct Canvas *cvs, int x, int y, unsigned int color);

int darken_percent_pixel(Canvas *cvs, int x, int y, float pct,  const ColorVec *clr);

void drawRect(Canvas *cvs, int x1, int y1, int x2, int y2,  const ColorVec *clr);
void fillRectWH(Canvas *cvs, int x, int y, int w, int h,  const ColorVec *clr);
void etchRect(Canvas *cvs, int x1, int y1, int x2, int y2,  const ColorVec *clr);
void etchRectWH(Canvas *cvs, int x, int y, int width, int height,  const ColorVec *clr);
void drawLine(Canvas *cvs, int x1,int y1, int x2, int y2, const ColorVec *clr);
void drawLineAngle(Canvas *cvs, int x,int y, float radians, int length,  const ColorVec *clr);
void drawLineAngleSec( Canvas *cvs, int x,int y, float radians, int slength, int elength,  const ColorVec *clr);
void etchCircle(Canvas *cvs, int x, int y, int radius,  const ColorVec *clr);
void fillCircle(Canvas *cvs, int x, int y, int radius,  const ColorVec *clr);
void drawTriangle(unsigned char *image, int rowlength, int t1, int t2, int t3);

#endif
