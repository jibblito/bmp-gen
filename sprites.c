#include <stdio.h>
#include <string.h>

#include "sprites.h"
#include "colorvec.h"
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>


struct BitmapFile {
  struct FileHeader {
    unsigned char signature [2]; // 0x424D (BM) for a BMP file - other types are possible but not supported here
    uint32_t file_size ;         // size of the BMP file in bytes
    uint16_t reserved_1;         // reserved
    uint16_t reserved_2;         // reserved
    uint32_t offset    ;         // Starting address of the byte where the pixel array can be found
  } f_header;
  struct InfoHeader {
    uint32_t header_size;         // size of this header (typically 40)
    uint32_t bm_width;            // Width in pixels of da image
    uint32_t bm_height;           // Height in pixels of da image
    uint16_t n_color_planes;      // Number of color planes (must be 1)
    uint16_t bits_per_pixel;      // Number of bits per pixel (color depth o' th'image): 1, 4, 8, 16, 24, 32
    uint32_t compression_method;  // There are many multiple compression methods on Wikipedia, yo.
    uint32_t raw_image_size;      // Size of the raw bitmap data
    uint32_t horizontal_res;      // Horizontal resolution, yo (pixel per metre)
    uint32_t vertical_res;        // Vertical resolution, yo
    uint32_t n_colors;            // Number of colors in color palette
    uint32_t n_imp_colors;        // Number of important colors in color palette
  } i_header;
};

/*
 * A sprite is loaded from a bitmap file
 */
void loadSpriteFromFile(struct Sprite *sprite, char *filename) {
  

  int fd = open(filename, O_RDONLY);
  if (fd == -1) {
    fprintf(stderr,"Unable 2 open da file (%s): %s\n",filename,strerror(errno));
  } else {
    fprintf(stdout,"We opened dat filee my guy (%s)..... hahahahahaha\n",filename);
  }

  unsigned char read_buf[512] = { 0 };
  struct BitmapFile bmp_file;
  ssize_t bytes_in;
  bytes_in = read(fd,&bmp_file,2);
  if(strncmp(bmp_file.f_header.signature,"BM",2) != 0) {
    printf("Image %d is not in the BITMAP format, yo!\n");
    exit(1);
  }
  bytes_in = read(fd,(unsigned char *)(&bmp_file) + 4,12);
  bytes_in = read(fd,(unsigned char *)(&bmp_file.i_header),40);

  sprite->width = bmp_file.i_header.bm_width;
  sprite->height = bmp_file.i_header.bm_height;
  sprite->pixels = malloc(bmp_file.i_header.raw_image_size);

  lseek(fd,bmp_file.f_header.offset,SEEK_SET);
  int i;
  bytes_in = 0;
  int rowsize_b = bmp_file.i_header.raw_image_size / sprite->height;
  for(i = sprite->height-1; i >= 0; i--)
    bytes_in += read(fd,sprite->pixels + i*sprite->width,rowsize_b);

  close(fd);
  printf("Sprite loaded into data structure, yo. Bytes read total: %d\n",bytes_in);
}

/*
 * Draw a sprite to a can-viss
 */
void drawSpriteToCanvas(struct Canvas *cvs, struct Sprite *sprite, int x, int y) {
  int i, j;
  for (i = 0; i < sprite->width; i++) {
    for (j = 0; j < sprite->height; j++) {
      uint32_t pixel = (uint32_t) *(sprite->pixels+i+j*sprite->width);
      // Bmp file: R G B A in memory
      // Each word (uint32_t): A B G R, thus alpha value is at 24 bit offset
      unsigned char alpha = pixel >> RSHIFT_ALPHA;
      if (alpha != 0) {
        *((uint32_t *)cvs->image+(x+i)+(y+j)*cvs->width) = pixel;
      }
    }
  }
}

/*
 * Destroy the memory taken up by a sprite, tho
 */
void destroySprite(struct Sprite *sprite) {
  free(sprite->pixels);
}
