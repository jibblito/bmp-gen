/**
 * Driver function for generate bitmap..
 *
 * Need to separate the file I/O into a separate file..
 * Want to support ability to read an input image in and
 * interpret it as a bitmap.
 *
 * Currently working with 480x480 images
 * Currently too lazy to do a separate file!!!! Shouldn't
 * take too long tho... but the functionality is quite simple...
 * This file could be made much simpler if its only function was
 * call read file, draw things, write file
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "shapes.c"
#include "canvas.h"

int main (int argc, char **argv)
{
    int height = 480;
    int width = height;
    printf("Generating %dx%d image for ya!\n",width,height);
    unsigned char *image;
    image =  malloc(height * width * BYTES_PER_PIXEL);


    char imageFileName[32];
    if (argc == 1)
    {
      fprintf(stderr, "Yo, you have one argument. This is an error!: %s\n",argv[0]);
      exit(1);
    }
    if (argc >= 2)
    {
      sprintf(imageFileName,"%s.bmp",argv[1]);
    }


    struct Canvas *beall = initCanvas(height, width, imageFileName);

    struct ColorVec* blue = initColor(0,0,255);


    /**
     * experiment zone
     */

    plot(beall->image, beall->height*3, 10, 10, blue);
    int i;
    for (i = 0; i < beall->width; i++) {
      plot(beall->image,beall->height*3,i,40,blue);
    }

    //int rowlength = height;
    //drawGradiSquare(beall->image,height*3,0,0,height);
    //drawSquare(image,height*3,1,100,height-200);
    // int i;
    // drawGradiSquare(image,rowlength,0,0,width);
    // drawLine(image,rowlength,1,1,300,13);
    // drawLine(image,rowlength,300,13,0,50);
    // drawLine(image,rowlength,0,50,200,200);
    // drawLineConColor(image,rowlength,200,200,300,100,blue);
    

    //generateBitmapImage((unsigned char*) image, height, width, imageFileName);
    generateBitmapImage(beall);
    printf("Image generated!!\n");
    free(image);
}



/**
 * Open and write the bitmap image file... Given a name, image, and a width.....
 */
/**
void generateBitmapImage (unsigned char* image, int height, int width, char* imageFileName)
{
    int widthInBytes = width * BYTES_PER_PIXEL;

    unsigned char padding[3] = {0, 0, 0};
    int paddingSize = (4 - (widthInBytes) % 4) % 4;

    int stride = (widthInBytes) + paddingSize;

    FILE* imageFile = fopen(imageFileName, "wb");

    unsigned char* fileHeader = createBitmapFileHeader(height, stride);
    fwrite(fileHeader, 1, FILE_HEADER_SIZE, imageFile);

    unsigned char* infoHeader = createBitmapInfoHeader(height, width);
    fwrite(infoHeader, 1, INFO_HEADER_SIZE, imageFile);

    int i;
    for (i = 0; i < height; i++) {
        fwrite(image + (i*widthInBytes), BYTES_PER_PIXEL, width, imageFile);
        fwrite(padding, 1, paddingSize, imageFile);
    }

    fclose(imageFile);
}

*/
