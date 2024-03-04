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
    int height = 203;
    int width = height;

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
    struct ColorVec* red = initColor(255,0,0);
    struct ColorVec* green = initColor(0,255,0);
    struct ColorVec* slack_blue = initColor(50,30,177);
    struct ColorVec* fuschia = initColor(230,3,102);

    /**
     * experiment zone
     */

    int i;
    for (i = 0; i < beall->width; i++) {
      plot(beall,i,10,blue);
      plot(beall,90,i,green);
      plot(beall,i,i/2,slack_blue);
      plot(beall,i,i,red);
    }
    plot(beall,30,30,blue);
    etchCircle(beall, 50, 50, 10, fuschia);

    drawLine(beall, 29,5,29,189,fuschia);


    int r, g, b;
    float offset = 1;

    /**
    for (i = -50; i < 50; i++) {
      int grade = 255/2 + (int)(sin(offset+(float)i/5)*(255/2));
      struct ColorVec* giggle= initColor(grade,grade,grade);
      drawLine(beall,20,100+i,180,100-i,giggle);
    }
    */

    // Joining of four Diamonds
    for (i = 0; i <= width; i++) {
      float shade_fac = (float)((width/2)-abs(width/2 - i))/(float)(width/2);
      r = 155 * shade_fac;
      g = 153 * shade_fac;
      b = 251 * shade_fac;
      struct ColorVec* giggle= initColor(r,g,b);
      drawLine(beall,i,0,width-i,width,giggle);
    }

    drawRect(beall,30,30,20,20,green);

    /*
    for (i = 0; i <= width; i++) {
      float shade_fac = (float)((width/2)-abs(width/2 - i))/(float)(width/2);
      r = (int)(((float)i/(float)width) * 255);
      g = (int)(((float)i/(float)width) * 243);
      b = (int)(((float)i/(float)width) * 231);
      r = 155 * shade_fac;
      g = 193 * shade_fac;
      b = 1 * shade_fac;

      struct ColorVec* giggle= initColor(r,g,b);
      drawLine(beall,0,i,width,width-i,slack_blue);
    }
    */

    /*
    // Idea for diamonded paddurn
    for (i = 0; i < width; i++) {
      for (j = 0; j < width; j++) {
        r = 155 * ((float)(j * i)/(float)(width*width));
        g = 33;
        b = sin(j);

        int centroid_x = (float)(i/10) * (width/10);
        int centroid_y = (float)(j/10) * (width/10);

        drawLine(beall,i,j,
      }
    }
    */

    int x_runner = 0, y_runner = 0;
    int center = width/2;
    r=0;
    g=0;
    b=0;

    /**
    // Random Runner
    while (x_runner < width) {
      r = rand() % 155;
      g = rand() % 155;
      b = rand() % 155;
      struct ColorVec* santiago = initColor(r,g,b);
      drawLine(beall,x_runner,y_runner,center,center,santiago);
      x_runner += 5;
    }
    */


    /**
    //Sine Wave, dude!
    for (i = 0; i < beall->width/2; i++) {
      r = 255/2 + (int)(sin(offset+(float)i/5)*(255/2));
      g = 255/2 + (int)(sin(offset+(float)i/6)*(255/2));
      b = 255/2 + (int)(sin(offset+(float)i/7)*(255/2));
      struct ColorVec* giggle= initColor(r,g,b);
      // etchCircle(beall, beall->width/2,beall->width/2,i,giggle);
    }
    for (i = 0; i < beall->width; i++) {
      plot(beall,i,beall->width/2+sin((float)i/100)*50,slack_blue);
    }
    */



    //int rowlength = height;
    //drawGradiSquare(beall->image,height*3,0,0,height);
    //drawSquare(image,height*3,1,100,height-200);
    // int i;
    // drawGradiSquare(image,rowlength,0,0,width);
    // drawLine(image,rowlength,1,1,300,13);
    // drawLine(image,rowlength,300,13,0,50);
    // drawLine(image,rowlength,0,50,200,200);
    // drawLineConColor(image,rowlength,200,200,300,100,blue);
    
    generateBitmapImage(beall);
    printf("Image generated!!\n");
}
