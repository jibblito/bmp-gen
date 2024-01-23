#ifndef COLORVEC_H
#define COLORVEC_H

#define BLUE 0
#define GREEN 1
#define RED 2

struct ColorVec {
  unsigned char clr[3];
};

struct ColorVec *initColor(unsigned char r, unsigned char g, unsigned char b);
void printColorVec(struct ColorVec *clr);

#endif
