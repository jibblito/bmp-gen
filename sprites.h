#ifndef SPRITES_H 
#define SPRITES_H 

#include "canvas.h"
#include "colorvec.h"

struct Sprite {
  int height,width;
  unsigned char pixels[];
}

void loadSpriteFromFile(struct Sprite *sprite, char *filename);

#endif
