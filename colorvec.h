#ifndef COLORVEC_H
#define COLORVEC_H

#define RED 2
#define GREEN 1
#define BLUE 0
#define MAX_GRADIENT_COLORS 3

struct ColorVec {
  unsigned char r;
  unsigned char g;
  unsigned char b;
};

void initColor(struct ColorVec *clr, unsigned char r, unsigned char g, unsigned char b);
void printColorVec(struct ColorVec *clr);

struct ColorVecGradient {
  struct ColorVec gradient[MAX_GRADIENT_COLORS];
  int n_colors;
};

void addColorToColorVecGradient(struct ColorVecGradient *cvg, struct ColorVec *clr);
struct ColorVec getColorFromGradient(struct ColorVecGradient *cvg, float degree);

#endif
