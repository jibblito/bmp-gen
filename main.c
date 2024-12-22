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
#include "sprites.h"
#include "transformations.c"
#include <X11/Xutil.h>

#include <unistd.h> // usleep()
#include <sys/time.h> // gettimeofday()
#include <string.h> // strlen()


int toggleAnimation();
double get_time(void);

int main (int argc, char **argv)
{
    // Generate square 480x480 image
    int height = 600;
    int width = 600;
    struct Canvas *beall = initCanvas(width, height, "Bealle!");

		// Palette!
    INIT_COLOR(C_red,0xff0000);
    INIT_COLOR(C_green,0x00ff00);
    INIT_COLOR(C_blue,0x0000ff);
    INIT_COLOR(C_slackblue,0x321eb1);
    INIT_COLOR(C_fuschia,0xe60368);
    INIT_COLOR(C_yeller,0xf9db17);
    INIT_COLOR(C_indigo,0x6e00fd);
    INIT_COLOR(C_bluebird,0x1cc2b5);
    INIT_COLOR(C_white,0xffffff);
    INIT_COLOR(C_black,0x000000);

    struct ColorVecGradient gradient1;
    gradient1.n_colors = 0;
    addColorToColorVecGradient(&gradient1,&C_green);
    addColorToColorVecGradient(&gradient1,&C_blue);
		printGradient(&gradient1);

    struct ColorVecGradient gradient2;
    gradient2.n_colors = 0;
    addColorToColorVecGradient(&gradient2,&C_red);
    addColorToColorVecGradient(&gradient2,&C_fuschia);

    INIT_GRADIENT(G_beautiful);
    addColorToColorVecGradient(&G_beautiful,&C_yeller);
    addColorToColorVecGradient(&G_beautiful,&C_indigo);
    addColorToColorVecGradient(&G_beautiful,&C_bluebird);
    addColorToColorVecGradient(&G_beautiful,&C_green);
    addColorToColorVecGradient(&G_beautiful,&C_red);
    addColorToColorVecGradient(&G_beautiful,&C_blue);
    G_beautiful.loop = 1;

    INIT_GRADIENT(G_rgb);
    addColorToColorVecGradient(&G_rgb,&C_red);
    addColorToColorVecGradient(&G_rgb,&C_green);
    addColorToColorVecGradient(&G_rgb,&C_blue);

    /**
     * experiment zone
     */
    
    struct ColorVecGradient grnRedBlu;
    grnRedBlu.n_colors = 0;
    addColorToColorVecGradient(&grnRedBlu,&C_green);
    addColorToColorVecGradient(&grnRedBlu,&C_red);
    addColorToColorVecGradient(&grnRedBlu,&C_blue);
    grnRedBlu.loop = 1;

    INIT_GRADIENT(G_blackwhite);
    addColorToColorVecGradient(&G_blackwhite,&C_black);
    addColorToColorVecGradient(&G_blackwhite,&C_white);

    /**
     * end setup
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
    Visual *vis = DefaultVisual(dis,scr);
    win = XCreateSimpleWindow(dis, RootWindow(dis,scr), 0,0, beall->width, beall->height,1,
        BlackPixel(dis,scr), WhitePixel(dis,scr));
    XSetStandardProperties(dis,win,"Boy Game","BGIcon",None,NULL,0,NULL);
    XSelectInput(dis, win, KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | FocusChangeMask);
    GC gc;
    gc = XCreateGC(dis,win,0,0);

    // XImage specs to match Canvas

		int depth = 24; // works fine with depth = 24
		int bitmap_pad = 32; // 32 for 24 and 32 bpp, 16, for 15&16
		unsigned char *image32=(unsigned char *)malloc(width*height*4);
		XImage *xim= XCreateImage(dis, vis, depth, ZPixmap, 0, image32, width, height, bitmap_pad, 0);
		if (xim == NULL) {
			printf("Nulled out on XCreateImage, yo! (xim)\n");
		}

    // end that!

    XMapRaised(dis,win);

    double startFrameCalc, endFrameCalc, frameCalcTotal, ideal_frames_per_second, frame_length_in_usec;
    
    ideal_frames_per_second = 30;
    frame_length_in_usec = 1000000 / ideal_frames_per_second;
		int lag_detected = 0;


		int plib_width = 40, plib_height = 80;
		int plib_x = 20, plib_y = 100;
		int plib_active;

		int sqrt = 60;
		int hl_x = -1;
		int hl_y = -1;

    KeySym keyGet;
		int iter = 0, running = 1;
		int i;

    /**
     * Run Loop
     */
    printf("Press q or esc on the window to quit! Or ctrl-c here!!!\n");
    while (running) {
      startFrameCalc = get_time();
			int clk_x = -1, clk_y = -1;
      /**
       * Block: Register X Inputs
       */
      int p_watermark;
			p_watermark = XPending(dis);

			while (p_watermark > 0) {
        XNextEvent(dis,&event);
				p_watermark--;
        switch (event.type) {
          case KeyPress:
            XLookupString(&event.xkey,NULL,0,&keyGet,0);
            switch(keyGet) {
              case XK_Escape:
                running = 0;
                break;
              case XK_q:
                running = 0;
                break;
              case XK_space:
                break;
              case XK_a:
								plib_x-=10;
								if (plib_x < 10) plib_x = 10;
                break;
              case XK_d:
								plib_x+=10;
								if (plib_x + plib_width > beall->width-10) plib_x = beall->width-plib_width-10;
                break;
              case XK_w:
								plib_y-=10;
								if (plib_y < 10) plib_y = 10;
                break;
              case XK_s:
								plib_y+=10;
								if (plib_y + plib_height > beall->height-10) plib_y = beall->height-plib_height-10;
                break;
              case XK_Left:
								do
									sqrt--;
								while (sqrt != 0 && width % sqrt != 0);
								if (sqrt < 1) sqrt = 1;
								printf("Sqrt chang'd to %d.\n",sqrt);
                break;
              case XK_Right:
								do
									sqrt++;
								while (width % sqrt != 0 && sqrt < width/4);
								if (sqrt > width/4) sqrt = width/4;
								printf("Sqrt chang'd to %d.\n",sqrt);
                break;
            }
            break;
          case KeyRelease:
            break;
					case MotionNotify:
						hl_x = event.xmotion.x;
						hl_y = event.xmotion.y;
						break;
					case FocusOut:
						hl_x = -1;
						hl_y = -1;
						break;
          case ButtonPress:
            XButtonEvent xbutton = event.xbutton;
            switch(xbutton.button) {
              case Button1:
								clk_x = event.xbutton.x;
								clk_y = event.xbutton.y;
                break;
              case Button3:
                break;
              break;
            }
            break;
        }
      }
      XFlush(dis);

      /**
       * Block: Change state of game based on input
       */

			// Check our little button for pressage!
			if (hl_x>=plib_x&&hl_x<plib_x+plib_width&&hl_y>=plib_y&&hl_y<plib_y+plib_height)
				plib_active = 1;
			else
				plib_active = 0;

			if (clk_x>=plib_x&&clk_x<plib_x+plib_width&&clk_y>=plib_y&&clk_y<plib_y+plib_height)
				toggleAnimation();

      /**
       * Block: Manipulate canvas
       */

			int chunckwid = beall->width/sqrt;
			int chunckhei = beall->height/sqrt;
			for (i = 0; i < sqrt*sqrt; i++)
				sqrt%2==1?
					etchRectWH(beall,i%sqrt*chunckwid,i/sqrt*chunckhei,chunckwid,chunckhei,i%2==0?&C_white:&C_black):
					i/sqrt%2==0?
						etchRectWH(beall,i%sqrt*chunckwid,i/sqrt*chunckhei,chunckwid,chunckhei,i%2==0?&C_white:&C_black):
						etchRectWH(beall,i%sqrt*chunckwid,i/sqrt*chunckhei,chunckwid,chunckhei,i%2!=0?&C_white:&C_black);
			for (i = 0; i < sqrt*sqrt; i++)
				sqrt%2==1?
					etchRectWH(beall,i%sqrt*chunckwid+1,i/sqrt*chunckhei+1,chunckwid-2,chunckhei-2,i%2!=0?&C_white:&C_black):
					i/sqrt%2==1?
						etchRectWH(beall,i%sqrt*chunckwid+1,i/sqrt*chunckhei+1,chunckwid-2,chunckhei-2,i%2==0?&C_white:&C_black):
						etchRectWH(beall,i%sqrt*chunckwid+1,i/sqrt*chunckhei+1,chunckwid-2,chunckhei-2,i%2!=0?&C_white:&C_black);
			for (i = 0; i < sqrt*sqrt; i++)
				sqrt%2==1?
					fillRectWH(beall,i%sqrt*chunckwid+2,i/sqrt*chunckhei+2,chunckwid-4,chunckhei-4,i%2==0?&C_yeller:i/sqrt%2==0?&C_bluebird:&C_indigo):
					i/sqrt%2==0?
						fillRectWH(beall,i%sqrt*chunckwid+2,i/sqrt*chunckhei+2,chunckwid-4,chunckhei-4,i%2==0?&C_yeller:&C_bluebird):
						fillRectWH(beall,i%sqrt*chunckwid+2,i/sqrt*chunckhei+2,chunckwid-4,chunckhei-4,i%2!=0?&C_yeller:&C_indigo);


drawplib:
			etchRectWH(beall,plib_x,plib_y,plib_width,plib_height,&C_black);
			plib_active?
			fillRectWH(beall,plib_x+1,plib_y+1,plib_width-2,plib_height-2,&C_red):
			fillRectWH(beall,plib_x+1,plib_y+1,plib_width-2,plib_height-2,&C_white);
      
drawdebuglogo:
			etchRectWH(beall,41,39,20,42,&C_red);
			int logoRange = 40;
			ColorVec temp;
			for (i = 0; i < logoRange; i++) {
				temp = getColorFromGradient(&G_blackwhite,(float)i/(float)(logoRange-1));
				drawLine(beall,41,40+i,61,40+i,&temp);
			}
			etchRectWH(beall,29,29,12,62,&C_red);
			fillRectWH(beall,30,30,10,10,&C_yeller);
			fillRectWH(beall,30,40,10,10,&C_indigo);
			fillRectWH(beall,30,50,10,10,&C_bluebird);
			fillRectWH(beall,30,60,10,10,&C_fuschia);
			fillRectWH(beall,30,70,10,10,&C_white);
			fillRectWH(beall,30,80,10,10,&C_black);

			etchRectWH(beall,49,29,22,62,&C_red);
      fillRectWH(beall,50,30,20,20,&C_yeller);
      fillRectWH(beall,50,50,20,20,&C_indigo);
      fillRectWH(beall,50,70,20,20,&C_bluebird);
      etchRectWH(beall,50,30,20,20,&C_white);
      etchRectWH(beall,50,50,20,20,&C_black);
      etchRectWH(beall,50,70,20,20,&C_white);


      /**
       * Block: Draw X Image to screen 
       */
			
      flashCanvasToXImage(beall,xim,0,0);
      int ret = XPutImage(dis,win,gc,xim,0,0,0,0,xim->width,xim->height);

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
      iter++;
    }

    XDestroyImage(xim);
    XFreeGC(dis,gc);
    XCloseDisplay(dis);
    free(beall);
}

// time of day
double get_time(void)
{
  struct timeval timev;

  gettimeofday(&timev, NULL);

  return (double)timev.tv_sec + (((double)timev.tv_usec) / 1000000);
}

// set things in motion
int toggleAnimation(void) {
	printf("Write the function ToggleAnimation!!! Oh, and develop the class, yo!\n");
	return 0;
}
