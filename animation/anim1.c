/**
 * Animation file!
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "../shapes.h"
#include "../canvas.h"
#include "../colorvec.h"

int main (int argc, char **argv)
{

    int height = 100;
    int width = height;
    int n_frames = 100;

    struct Canvas** frames;

    frames = malloc(sizeof(struct Canvas) * n_frames);

    int i;

    // Frame initialization Loop
    fprintf(stdout,"Initializing Frames!\n");
    for(i = 0; i < n_frames; i++) {
      char filename[32];
      sprintf(filename,"out/frame%d.bmp\0",i);
      frames[i] = initCanvas(height,width,filename);
    }


    struct ColorVec* blue = initColor(0,0,255);
    struct ColorVec* red = initColor(255,0,0);
    struct ColorVec* green = initColor(0,255,0);
    struct ColorVec* slack_blue = initColor(50,30,177);
    struct ColorVec* fuschia = initColor(230,3,102);

    // Frame Animation Loop
    fprintf(stdout,"Animating Frames!\n");
    for (i = 0; i < n_frames; i++) {
      etchCircle(frames[i],10+i,width/2,5,fuschia);


      int j;
      int r,g,b;
      for (j = 0; j < width/2; j++) {
        r = 255/2 + 255/2 * sinf((float)j/19 + (float)i/17);
        g = 255/2 + 255/2 * sinf((float)j/17 + (float)i/19);
        b = 255/2 + 255/2 * sinf((float)j/23 + (float)i/5);
        struct ColorVec* darn = initColor(r,g,b);
        etchCircle(frames[i],width/2,width/2,j,darn);
      }


      float grade = ((float)i / (float)n_frames) * 255;
      struct ColorVec* dillard = initColor(100,grade,100);

      drawLine(frames[i],0,0,width,width,dillard);

      etchCircle(frames[i],10+i,width/2,10 + i,fuschia);
    }

    // Frame file-writing loop
    fprintf(stdout,"Writing Frames!\n");
    for (i = 0; i < n_frames; i++) {
      generateBitmapImage(frames[i]);
    }

    free(frames);
}
