#ifndef COLORVEC_H
#define COLORVEC_H

#define RED 2
#define GREEN 1
#define BLUE 0

struct ColorVec {
  unsigned char r;
  unsigned char g;
  unsigned char b;
};

void initColor(struct ColorVec *clr, unsigned char r, unsigned char g, unsigned char b);
void printColorVec(struct ColorVec *clr);

#endif
