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
    int height = 201;
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

    struct ColorVec easy,white,blue,red,green,slack_blue,fuschia;

    initColor(&easy,   254,  1,    253);
    initColor(&white,  255,  255,  255);
    initColor(&blue,     0,    0,   255);
    initColor(&red,    255,   0,    0);
    initColor(&green,    0,   255,  0);
    initColor(&slack_blue,    50,    30,    177);
    initColor(&fuschia,  230,   3,    102);

    drawRect(beall,0,0,height-1,width-1,&easy);
    drawLine(beall,width-1,0,width-1,height,&white);
    etchCircle(beall, 30, 30, 10, &blue);

    int i;
    for (i = 0; i < beall->width; i++) {
      plot(beall,i,10,&blue);
      plot(beall,90,i,&green);
      plot(beall,i,i/2,&slack_blue);
      plot(beall,i,i,&red);
    }

    int r, g, b;
    float offset = 1;

    struct ColorVec giggle;
    for (i = -50; i < 50; i++) {
      int grade = 255/2 + (int)(sin(offset+(float)i/5)*(255/2));
      initColor(&giggle,grade,grade,grade);
      drawLine(beall,20,100+i,180,100-i,&giggle);
    }

    // Joining of four Diamonds
    for (i = 0; i <= width; i++) {
      float shade_fac = (float)((width/2)-abs(width/2 - i))/(float)(width/2);
      r = 155 * shade_fac;
      g = 153 * shade_fac;
      b = 251 * shade_fac;
      initColor(&giggle,r,g,b);
      drawLine(beall,i,0,width-i,width,&giggle);
    }

    
    generateBitmapImage(beall);

    printf("Image generated!!\n");

    free(beall);
}
