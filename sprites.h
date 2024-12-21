#ifndef SPRITES_H 
#define SPRITES_H 

#include "canvas.h"
#include "colorvec.h"
#include <stdint.h>

struct Sprite {
  int width,height;
  uint32_t *pixels;
};

void loadSpriteFromFile(struct Sprite *sprite, char *filename);
void drawSpriteToCanvas(struct Canvas *cvs, struct Sprite *sprite, int x, int y);
void destroySprite(struct Sprite *sprite);

#endif
