#include <stdio.h>
#include "sprites.h"
#include <magic.h>

/*

struct Sprite {
  int height,width;
  unsigned char pixels[];
}

*/

void loadSpriteFromFile(struct Sprite *sprite, char *filename) {
  
  FILE *imageFile = fopen(filename, "wb");
  magic_t cookie = magic_open(0);
  magic_file(cookie, filename);

}

