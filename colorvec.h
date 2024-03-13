#ifndef COLORVEC_H
#define COLORVEC_H

#define RED 0
#define GREEN 1
#define BLUE 2

struct ColorVec {
  unsigned char r;
  unsigned char g;
  unsigned char b;
};

void initColor(struct ColorVec *clr, unsigned char r, unsigned char g, unsigned char b);
void printColorVec(struct ColorVec *clr);

#endif
