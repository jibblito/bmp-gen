#ifndef COLORVEC_H
#define COLORVEC_H

#define RED 2
#define GREEN 1
#define BLUE 0
#define MAX_GRADIENT_COLORS 10


#define INIT_GRADIENT(X) struct ColorVecGradient X = { .n_colors = 0, .loop = 0 }
#define INIT_COLOR(X,Y) struct ColorVec X; initColorHex(&X,Y);

struct ColorVec {
  unsigned char r;
  unsigned char g;
  unsigned char b;
};

void initColor(struct ColorVec *clr, unsigned char r, unsigned char g, unsigned char b);
void initColorHex(struct ColorVec *clr, unsigned long hex);
void printColorVec(struct ColorVec *clr);

struct ColorVecGradient {
  struct ColorVec gradient[MAX_GRADIENT_COLORS];
  int loop;
  int n_colors;
};

void addColorToColorVecGradient(struct ColorVecGradient *cvg, struct ColorVec *clr);
struct ColorVec getColorFromGradient(struct ColorVecGradient *cvg, float degree);

struct ColorVec combineColors(struct ColorVec *a, struct ColorVec *b, float degree);

#endif
