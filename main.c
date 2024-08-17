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
    int height = 600;
    int width = 600;

    int save_file = 0;

    char *usage = "Usage: %s <optional filename>\n(to save da file)\n";

    char imageFileName[32];
    //if (argc == 1)
    //{
    //  fprintf(stderr, "Yo, you have one argument. This is an error!: %s\n",argv[0]);
    //  exit(1);
    //}
    if (argc >= 2)
    {
      if (strcmp(argv[1],"-h") == 0 || strcmp(argv[1],"--help") == 0) {
          printf("Need help? Don't fret, young child... Do not fret..\n");
          printf(usage,argv[0]);
          exit(0);
      } else {
        save_file = 1;
        sprintf(imageFileName,"%s.bmp",argv[1]);
      }
    }


    struct Canvas *beall = initCanvas(width, height, imageFileName);

    struct ColorVec blue,red,green,slack_blue,fuschia;

    initColor(&blue,0,0,255);
    initColor(&red,255,0,0);
    initColor(&green,0,255,0);
    initColor(&slack_blue,50,30,177);
    initColor(&fuschia,230,3,102);
    INIT_COLOR(yeller,0xf9db17);
    INIT_COLOR(indigo,0x6e00fd);
    INIT_COLOR(bluebird,0x1cc2b5);

    struct ColorVec whitxe;
    initColor(&whitxe,255,255,255);
    struct ColorVec blaq;
    initColor(&blaq,0,0,0);

    struct ColorVecGradient gradient1;
    gradient1.n_colors = 0;
    addColorToColorVecGradient(&gradient1,&green);
    addColorToColorVecGradient(&gradient1,&blue);

    struct ColorVecGradient gradient2;
    gradient2.n_colors = 0;
    addColorToColorVecGradient(&gradient2,&red);
    addColorToColorVecGradient(&gradient2,&fuschia);

    INIT_GRADIENT(beautiful);
    addColorToColorVecGradient(&beautiful,&yeller);
    addColorToColorVecGradient(&beautiful,&indigo);
    addColorToColorVecGradient(&beautiful,&bluebird);
    addColorToColorVecGradient(&beautiful,&green);
    addColorToColorVecGradient(&beautiful,&red);
    addColorToColorVecGradient(&beautiful,&blue);

    /**
     * experiment zone
     */

    
    struct ColorVecGradient grnRedBlu;
    grnRedBlu.n_colors = 0;
    addColorToColorVecGradient(&grnRedBlu,&green);
    addColorToColorVecGradient(&grnRedBlu,&red);
    addColorToColorVecGradient(&grnRedBlu,&blue);

    INIT_GRADIENT(blackwhite);
    addColorToColorVecGradient(&blackwhite,&blaq);
    addColorToColorVecGradient(&blackwhite,&whitxe);

    int i,j;
    for (i = 0; i < beall->width; i++) {
      struct ColorVec clr = getColorFromGradient(&beautiful,(float)i/(float)beall->width);
      drawLine(beall,i,0,i,beall->height,&clr);
    }

    /**
     * end experiment zone
     */

    
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


    // Start Game Environment things!
    
    int boy_x=width/2,boy_y=width/2;
    int boy_width = 30;
    int boy_momentum_y = 0; // momentum, yo!
    int gravity_constant = 1;
    int boy_on_ground = 0;
    int boy_momentum_x = 0;
    int friction_constant = 1;
    int max_speed=10;
    int accel_x = 2;


    // end that!

    XMapRaised(dis,win);

    KeySym keyGet;

    double startFrameCalc, endFrameCalc, frameCalcTotal, ideal_frames_per_second, frame_length_in_usec;
    
    ideal_frames_per_second = 30;
    frame_length_in_usec = 1000000 / ideal_frames_per_second;

    char *quitmsg = "Press esc or q to quit";
    int running = 1;
    int iter = 0;
    int period1 = 2000;
    int period2 = 60;
    int lag_detected = 0;

    /**
     * Run Loop
     */
    printf("Press q or esc on the window to quit! Or ctrl-c here!!!\n");
    while (running) {
      startFrameCalc = get_time();

      /**
       * Block: Register X Inputs
       */
      while(XPending(dis)) {
        XNextEvent(dis,&event);
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
              case XK_space:
                boy_on_ground = 0;
                boy_momentum_y = 10;
                break;
              case XK_a:
                boy_momentum_x -= accel_x;
                break;
              case XK_d:
                boy_momentum_x += accel_x;
                break;
            }
            break;
        }
      }

      /**
       * Block: Manipulate canvas
       */

      

      float clockrotation = (float)iter/(float)period1;
      float sin1 = sinf((float)iter/48.3f);
      float sin2 = sinf((float)iter/49.0f);
      struct ColorVec clr = getColorFromGradient(&beautiful,sin1);

      // float armLength = (float)(width/4)*(sin2+1.0f);
      // drawLineAngleSec(beall,width/2,height/2,clockrotation*6.282,width/8,armLength,&clr);
      // for (i = 1; i < 9; i++) {
      //   etchCircle(beall,width/2,height/2,width/i,&whitxe);
      // }
      boy_y -= boy_momentum_y;
      boy_x += boy_momentum_x;
      etchCircle(beall,boy_x,boy_y, boy_width/2,&clr);
      etchCircle(beall,boy_x,boy_y, boy_width/2-1,&blaq);
      etchCircle(beall,boy_x,boy_y, boy_width/2-2,&clr);
      if (boy_on_ground == 0)
        boy_momentum_y -= gravity_constant;
      else {
        if (boy_momentum_x < 0-friction_constant)
          boy_momentum_x += friction_constant;
        else if (boy_momentum_x > friction_constant)
          boy_momentum_x -= friction_constant;
        else
          boy_momentum_x = 0;
      }

      if (boy_y < 0) {
        boy_y = 0;
        boy_momentum_y = 0;
      }
      if (boy_y > height) {
        boy_on_ground = 1;
        boy_y = height;
        boy_momentum_y = 0;
      }
      if (boy_x <= 0) {
        boy_momentum_x = 0;
        boy_x = 0;
      }
      if (boy_x > width) {
        boy_momentum_x = 0;
        boy_x = width;
      }

      drawRect(beall,30,30,40,40,&yeller);
      drawRect(beall,30,40,40,50,&indigo);
      drawRect(beall,30,50,40,60,&bluebird);



      

      /**
       * Block: Draw X Image to screen 
       */
      flashCanvasToXImage(beall,xim);
      int ret = XPutImage(dis,win,gc,xim,0,0,0,0,xim->width,xim->height);
      

      iter++;

      endFrameCalc = get_time();
      frameCalcTotal = (endFrameCalc - startFrameCalc)*1000000.0f;

      double sleepTime = frame_length_in_usec - frameCalcTotal;
      if (sleepTime < 0) {
        lag_detected += 1;
        if (lag_detected < 10)
          printf("Lag detected... frame not rendered in time!\n");
        else if (lag_detected == 10)
          printf("10 Laggy frames detected... woah.. stopping output tho,\n");

      } else 
        usleep(sleepTime);

      
    }

    XDestroyImage(xim);
    XFreeGC(dis,gc);
    XCloseDisplay(dis);

    if (save_file==1) {
      generateBitmapImage(beall);
      printf("Image generated!!\n");
    }

    free(beall);
}
