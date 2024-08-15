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
#include <X11/Xutil.h>

#include <unistd.h> // usleep()
#include <sys/time.h> // gettimeofday()
#include <string.h> // strlen()

/*
 * Get time of day
 */
double get_time(void)
{
  struct timeval timev;

  gettimeofday(&timev, NULL);

  return (double)timev.tv_sec + (((double)timev.tv_usec) / 1000000);
}

int main (int argc, char **argv)
{
    // Generate square 480x480 image
    int height = 50;
    int width = 80;

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


    struct Canvas *beall = initCanvas(width, height, imageFileName);

    struct ColorVec blue,red,green,slack_blue,fuschia;

    initColor(&blue,0,0,255);
    initColor(&red,255,0,0);
    initColor(&green,0,255,0);
    initColor(&slack_blue,50,30,177);
    initColor(&fuschia,230,3,102);

    /**
     * experiment zone
     */

    
    struct ColorVecGradient grnRedBlu;
    grnRedBlu.n_colors = 0;
    addColorToColorVecGradient(&grnRedBlu,&green);
    addColorToColorVecGradient(&grnRedBlu,&red);
    addColorToColorVecGradient(&grnRedBlu,&blue);

    int i,j;
    for (i = 0; i < beall->width; i++) {
      struct ColorVec clr = getColorFromGradient(&grnRedBlu,(float)i/(float)beall->width);
      drawLine(beall,i,0,i,60,&clr);
    }

    
    Display *dis;
    Window win;
    XEvent event;
    const char *msg = "Crenshack!";
    int scr;

    dis = XOpenDisplay(NULL);
    if (dis == NULL) {
      fprintf(stderr,"cannot open display!\n");
      exit(1);
    }

    scr = DefaultScreen(dis);
    int black = BlackPixel(dis,scr);
    int white = WhitePixel(dis,scr);
    Visual *vis = DefaultVisual(dis,scr);

    win = XCreateSimpleWindow(dis, RootWindow(dis,scr), 0,0, beall->width, beall->height,1,
        black, white);

    XSetStandardProperties(dis,win,"Boy Game","BGIcon",None,NULL,0,NULL);
    XSelectInput(dis, win, KeyPressMask | KeyReleaseMask | 
                 ButtonPressMask | ButtonReleaseMask);

    GC gc;
    gc = XCreateGC(dis,win,0,0);
    XSetBackground(dis,gc,white);
    XSetForeground(dis,gc,black);

    // XImage specs to match Canvas
      int depth = 24; // works fine with depth = 24
      int bitmap_pad = 32; // 32 for 24 and 32 bpp, 16, for 15&16
      int bytes_per_line = width*4; // number of bytes in the client image between the start of one scanline and the start of the next
      unsigned char *image32=(unsigned char *)malloc(width*height*4);
      XImage *xim= XCreateImage(dis, vis, depth, ZPixmap, 0, image32, width, height, bitmap_pad, bytes_per_line);
      if (xim == NULL) {
        printf("Nulled out on XCreateImage, yo! (xim)\n");
      }
    // end XImage specs 

    XMapRaised(dis,win);

    KeySym keyGet;

    double startFrameCalc, endFrameCalc, frameCalcTotal, ideal_frames_per_second, frame_length_in_usec;
    
    ideal_frames_per_second = 30;
    frame_length_in_usec = 1000000 / ideal_frames_per_second;

    char *quitmsg = "Press esc or q to quit";

    int running = 1;
    while (running) {
      startFrameCalc = get_time();
      // Register inputs
      int capture_this = 0;
      while (XPending (dis)) {
        XNextEvent (dis, &event);
        switch (event.type) {
          case KeyPress:
            printf("Key gotten!\n");
            XLookupString(&event.xkey,NULL,0,&keyGet,0);
            switch(keyGet) {
              case XK_Escape:
                running = 0;
                break;
              case XK_q:
                running = 0;
                break;
            }
        }
      }
      

      flashCanvasToXImage(beall,xim);
      int ret = XPutImage(dis,win,gc,xim,0,0,0,0,xim->width,xim->height);

      XDrawString(dis, win, gc, 10, 20, quitmsg, strlen(quitmsg));
      XDrawRectangle(dis,win, gc, 1, 1, 30, 30);

      endFrameCalc = get_time();
      frameCalcTotal = (endFrameCalc - startFrameCalc)*1000000.0f;

      double sleepTime = frame_length_in_usec - frameCalcTotal;
      if (sleepTime < 0)
        printf("Lag detected... frame not rendered in time!");
      else 
        usleep(sleepTime);

      
    }

    XDestroyImage(xim);
    XFreeGC(dis,gc);
    XCloseDisplay(dis);

    generateBitmapImage(beall);
    // printf("Image generated!!\n");

    free(beall);
}
