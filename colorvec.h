#ifndef COLORVEC_H
#define COLORVEC_H

// Represents color offsets: Matches ARGB format
#define RSHIFT_RED 16
#define RSHIFT_GREEN 8
#define RSHIFT_BLUE 0
#define RSHIFT_ALPHA 24
#define MAX_GRADIENT_COLORS 10

#define INIT_GRADIENT(X) struct ColorVecGradient X = { .n_colors = 0, .loop = 0 }
#define INIT_COLOR(X,Y) struct ColorVec X; initColorHex(&X,Y);

typedef struct ColorVec {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
} ColorVec;

/* Define basic colors */

static const ColorVec C_white =	{ 0xff,0xff,0xff,0xff };
static const ColorVec C_black =	{ 0x00,0x00,0x00,0xff };
static const ColorVec C_gray50=	{ 0x88,0x88,0x88,0xff };
static const ColorVec C_red =		{ 0xff,0x00,0x00,0xff };
static const ColorVec C_green=		{ 0x00,0xff,0x00,0xff };
static const ColorVec C_blue=		{ 0x00,0x00,0xff,0xff };
static const ColorVec C_cyan=		{ 0x00,0xff,0xff,0xff };
static const ColorVec C_yellow=	{ 0xff,0x00,0xff,0xff };
static const ColorVec C_magenta=	{ 0xff,0xff,0x00,0xff };
static const ColorVec C_clear=		{ 0xff,0xff,0xff,0x00 };

void initColor(struct ColorVec *clr, unsigned char r, unsigned char g, unsigned char b);
void initColorHex(struct ColorVec *clr, unsigned int hex);
void printColorVec(struct ColorVec *clr);

typedef struct ColorVecGradient {
  struct ColorVec gradient[MAX_GRADIENT_COLORS];
  int loop;
  int n_colors;
} ColorVecGradient;

void addColorToColorVecGradient(struct ColorVecGradient *cvg, const struct ColorVec *clr);
struct ColorVec getColorFromGradient(struct ColorVecGradient *cvg, float degree);
void printGradient(struct ColorVecGradient *gradient);

struct ColorVec combineColors(struct ColorVec *a, struct ColorVec *b, float degree);
struct ColorVec invertColor(struct ColorVec *clr);

#endif
