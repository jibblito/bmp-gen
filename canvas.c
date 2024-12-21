/**
 * Implementation of canvas.h
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "canvas.h"
#include "colorvec.h"



/**
 *  HEIGHT (4) | WIDTH (4) | ROWLENGTH (4) | NAME (32) | IMAGE (variable)
 */
struct Canvas *initCanvas(int width, int height, char *name) {
  size_t n_bytes = sizeof(struct Canvas) + height*width*sizeof(unsigned int);
  struct Canvas *cvs = calloc(n_bytes, 1); 
  cvs->height = height;
  cvs->width = width;
  snprintf(cvs->name,sizeof(cvs->name), name);
  return cvs;
}

/**
 * Open and write the bitmap image file, line by line..... Do it.
 */
void generateBitmapImage(struct Canvas *cvs) {
  int widthInBytes = cvs->width * BYTES_PER_PIXEL; // ex 90 * 3 = 270
  unsigned char padding[3] = {0,0,0};
  int paddingSize = (4 - widthInBytes % 4) % 4;
  int stride = (widthInBytes) + paddingSize;

  FILE* imageFile = fopen(cvs->name, "wb");

  unsigned char* fileHeader = createBitmapFileHeader(cvs->height, stride);
  fwrite(fileHeader, 1, FILE_HEADER_SIZE, imageFile);
  unsigned char* infoHeader = createBitmapInfoHeader(cvs->height, cvs->width);
  fwrite(infoHeader, 1, INFO_HEADER_SIZE, imageFile);

  int i;
  for (i = cvs->height-1; i >= 0; i--) {
    fwrite(cvs->image + (i * widthInBytes), BYTES_PER_PIXEL, cvs->width, imageFile);
    fwrite(padding, 1, paddingSize, imageFile);
  }
  fclose(imageFile);
}

/**
 * Generate an XImage struct for da image
 */
int flashCanvasToXImage(struct Canvas *cvs, XImage *xim, int offset_x, int offset_y) {
  int redLShift, blueLShift, greenLShift;
  int i;
  for (i = 0; i < 32; i++) {
    if (0x01 << i & xim->red_mask) break;
  }
  redLShift = i;
  for (i = 0; i < 32; i++) {
    if (0x01 << i & xim->green_mask) break;
  }
  greenLShift = i;
  for (i = 0; i < 32; i++) {
    if (0x01 << i & xim->blue_mask) break;
  }
  blueLShift = i;

  int j;
  unsigned int pixel;
  for (i = offset_y; i < cvs->height+offset_y && i < xim->height; i++) {
    for (j = offset_x; j < cvs->width+offset_x && j < xim->width; j++) {
      pixel = *(cvs->image+i*cvs->width+j);
      // translate RGBA to XImage's pixel format
      unsigned int xPixel = pixel >> RSHIFT_RED << redLShift & xim->red_mask;
      xPixel |= pixel >> RSHIFT_GREEN << greenLShift & xim->green_mask;
      xPixel |= pixel >> RSHIFT_BLUE << blueLShift & xim->blue_mask;
      unsigned char alpha;
      if ((alpha = (unsigned char)(pixel >> RSHIFT_ALPHA)) != 0xff) {
        /*
         * Deal with compositing
         */
        unsigned int beforePixel = xim->f.get_pixel(xim,j,i);
        // xim->f.put_pixel(xim,j,i,0xffff0000);
      } else {
        xim->f.put_pixel(xim,j,i,xPixel);
      }
    }
  }
  return 0;
}

struct BitmapFile {
  struct FileHeader {
    unsigned char signature  [2]; // 0x424D (BM) for a BMP file - other types are possible but not supported here
    unsigned char file_size  [4]; // size of the BMP file in bytes
    unsigned char reserved_1 [2]; // reserved
    unsigned char reserved_2 [2]; // reserved
    unsigned char offset     [4]; // Starting address of the byte where the pixel array can be found
  } f_header;
  struct InfoHeader {
    unsigned char header_size [4];  // size of this header (typically 40)
    unsigned char bm_width    [4];  // Width in pixels of da image
    unsigned char bm_height   [4];  // Height in pixels of da image
    unsigned char n_color_planes [2]; // Number of color planes (must be 1)
    unsigned char bits_per_pixel [2]; // Number of bits per pixel (color depth o' th'image): 1, 4, 8, 16, 24, 32
    unsigned char compression_method [4]; // There are many multiple compression methods on Wikipedia, yo.
    unsigned char raw_image_size     [4]; // Size of the raw bitmap data
    unsigned char horizontal_res [4];  // Horizontal resolution, yo (pixel per metre)
    unsigned char vertical_res   [4];  // Vertical resolution, yo
    unsigned char n_colors       [4];  // Number of colors in color palette
    unsigned char n_imp_colors   [4];  // Number of important colors in color palette
  } i_header;
  unsigned char first_pixel;
};



/**
 * Bitmap file header.. Have not tooled with the settings of bitmaps,
 * this was the simplest way I could find to create bitmap images...
 * I'm sure there is a reference somewhere online that can help you
 * figure out how to make bitmaps with ... The comments sort of explain
 * the format of the File Header... Signature I'm not sure on.
 */
unsigned char* createBitmapFileHeader(int height, int stride) {
  size_t fileSize = FILE_HEADER_SIZE + INFO_HEADER_SIZE + (stride * height);

  static unsigned char fileHeader[] = {
      0,0,     /// signature
      0,0,0,0, /// image file size in bytes
      0,0,0,0, /// reserved
      0,0,0,0, /// start of pixel array
  };

  fileHeader[ 0] = (unsigned char)('B');
  fileHeader[ 1] = (unsigned char)('M');
  fileHeader[ 2] = (unsigned char)(fileSize      );
  fileHeader[ 3] = (unsigned char)(fileSize >>  8);
  fileHeader[ 4] = (unsigned char)(fileSize >> 16);
  fileHeader[ 5] = (unsigned char)(fileSize >> 24);
  fileHeader[10] = (unsigned char)(FILE_HEADER_SIZE + INFO_HEADER_SIZE);

  return fileHeader;
}

/**
 * Bitmap info header.. The fields somewhat explained in the comments. Obviously there
 * is a header size in the first. Includes width, height, number of color planes,
 * bits per pixel.
 * I do not screw with the other options such as compression, image size,
 * horizontal resolution, vertical resolution, colors in color table, or
 * important color count... I assume this will help with compression. Later.
 */
unsigned char* createBitmapInfoHeader (int height, int width)
{
    static unsigned char infoHeader[] = {
        0,0,0,0, /// header size               [0 - 3]
        0,0,0,0, /// image width               [4 - 7]
        0,0,0,0, /// image height              [8 -11]
        0,0,     /// number of color planes    [12-13]
        0,0,     /// bits per pixel            [14-15]
        0,0,0,0, /// compression               [16-19]
        0,0,0,0, /// image size                [20-23]
        0,0,0,0, /// horizontal resolution     [24-27]
        0,0,0,0, /// vertical resolution       [28-31]
        0,0,0,0, /// colors in color table     [31-35]
        0,0,0,0, /// important color count     [36-39]
    };

    infoHeader[ 0] = (unsigned char)(INFO_HEADER_SIZE);
    infoHeader[ 4] = (unsigned char)(width      );
    infoHeader[ 5] = (unsigned char)(width >>  8);
    infoHeader[ 6] = (unsigned char)(width >> 16);
    infoHeader[ 7] = (unsigned char)(width >> 24);
    infoHeader[ 8] = (unsigned char)(height      );
    infoHeader[ 9] = (unsigned char)(height >>  8);
    infoHeader[10] = (unsigned char)(height >> 16);
    infoHeader[11] = (unsigned char)(height >> 24);
    infoHeader[12] = (unsigned char)(1);
    infoHeader[14] = (unsigned char)(BYTES_PER_PIXEL*8);

    return infoHeader;
}
