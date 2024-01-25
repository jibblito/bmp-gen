/**
 * Implementation of canvas.h
 */

#include <string.h>
#include <stdio.h>


struct Canvas *initCanvas(int height, int width, char *name) {
  struct Canvas *cvs = (struct Canvas*)malloc(sizeof(struct Canvas)); 
  *(cvs->height) = height;
  *(cvs->width) = width;
  strcpy(cvs->name, name);
  cvs->image = malloc(height * width * BYTES_PER_PIXEL);
}

/**
 * Open and write the bitmap image file, line by line..... Do it.
 */
void generateBitmapImage(struct Canvas *cvs) {
  int widthInBytes = cvs->width * BYTES_PER_PIXEL;
  unsigned char padding[3] = {0,0,0};
  int paddingSize = (4 - (widthInBytes) % 4) % 4;
  int stride = (widtnInBytes) + paddingSize;

  FILE* imageFile = fopen(imageFileName, "wb");

  unsigned char* fileHeader = createBitmapFileHeader(cvs->height, stride);
  fwrite(fileHeader, 1, FILE_HEADER_SIZE, imageFile);
  unsigned char* infoHeader = createBitmapInfoHeader(cvs->height, cvs->width);
  fwrite(infoHeader, 1, INFO_HEADER_SIZE, imageFile);

  int i;
  for (i = 0; i < height; i++) {
    fwrite(image + (i * widthInBytes), BYTES_PER_PIXEL, width, imageFile);
    fwrite(padding, 1, paddingSize, imageFile);
  }
  fclose(imageFile);
}

/**
 * Bitmap file header.. Have not tooled with the settings of bitmaps,
 * this was the simplest way I could find to create bitmap images...
 * I'm sure there is a reference somewhere online that can help you
 * figure out how to make bitmaps with ... The comments sort of explain
 * the format of the File Header... Signature I'm not sure on.
 */
unsigned char* createBitmapFileHeader(int height, int stride) {
  int fileSize = FILE_HEADER_SIZE + INFO_HEADER_SIZE + (stride * height);

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
        0,0,0,0, /// header size
        0,0,0,0, /// image width
        0,0,0,0, /// image height
        0,0,     /// number of color planes
        0,0,     /// bits per pixel
        0,0,0,0, /// compression
        0,0,0,0, /// image size
        0,0,0,0, /// horizontal resolution
        0,0,0,0, /// vertical resolution
        0,0,0,0, /// colors in color table
        0,0,0,0, /// important color count
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
