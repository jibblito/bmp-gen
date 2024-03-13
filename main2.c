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
#include "shapes.h"
#include "canvas.h"
#include "colorvec.h"
#include "transformations.c"

int main (int argc, char **argv)
{
    // Generate square 480x480 image
    int height = 20;
    int width = height;

    char imageFileName[32];
    if (argc == 1)
    {
      fprintf(stderr, "Yo, you have one argument. This is an error!: %s\n",argv[0]);
      exit(1); }
    if (argc >= 2)
    {
      sprintf(imageFileName,"%s.bmp",argv[1]);
    }


    struct Canvas *beall = initCanvas(height, width, imageFileName);

    struct ColorVec easy,white;

//     struct ColorVec* blue = initColor(0,0,255);
//     struct ColorVec* red = initColor(255,0,0);
//     struct ColorVec* green = initColor(0,255,0);
//     struct ColorVec* slack_blue = initColor(50,30,177);
//     struct ColorVec* fuschia = initColor(230,3,102);


    initColor(&easy,   254,  1,    253);
    initColor(&white,  255,  255,  255);

    drawRect(beall,0,0,height-1,width-1,&easy);
    drawLine(beall,width-1,0,width-1,height,&white);
//    etchCircle(beall, 10, 10, 3, fuschia);
    
    generateBitmapImage(beall);

    printf("Image generated!!\n");

    free(beall);
}
