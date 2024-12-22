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

#define MAX_ENTITIES 10
enum EntityTypes { PLAYER, BALL, SQUARE };

/*
 * Get time of day
 */
double get_time(void)
{
  struct timeval timev;

  gettimeofday(&timev, NULL);

  return (double)timev.tv_sec + (((double)timev.tv_usec) / 1000000);
}

/**
 * Entity object
 */
typedef struct Entity Entity;
struct Entity {
  int x,y;
  int m_x, m_y;       // momentum
  int width,height;
  int type;
  int (*drawEntity) (Entity *, struct Canvas *, struct ColorVec *);
}; 

Entity entities[MAX_ENTITIES] = { 0 };
int n_entities = 0;

int toggleAnimation();

int drawPlayer(Entity *self, struct Canvas *cvs, struct ColorVec *clr) {
  drawRect(cvs,self->x,self->y,self->width,self->height,clr);
}

int drawBall(Entity *self, struct Canvas *cvs, struct ColorVec *clr) {
  etchCircle(cvs,self->x,self->y,self->width/2,clr);
}

int drawSquareEnt(Entity *self, struct Canvas *cvs, struct ColorVec *clr) {
  int x2 = self->x + self->width;
  int y2 = self->y + self->height;
  int width = self->width;
  etchRect(cvs,self->x,self->y,x2,y2,clr);
  etchCircle(cvs,self->x+width/3,self->y+width/5,width/7,clr);
  etchCircle(cvs,self->x+(width/3)*2,self->y+(width/5)*4,width/8,clr);
  etchCircle(cvs,self->x+(width/6)*5,self->y+width/4,width/9,clr);
}

int doesCollide(int x, int y) {
  int i;
  for (i = 0; i < n_entities; i++) {
    Entity *e = &entities[i];
    int xFix;
  }
  return 0;
}

/*
 * Returns the amount in the x direction that an object intersects badly
 */
int collidesX(int t_lbound, int t_rbound) {
  int i;
  for (i = 0; i < n_entities; i++) {
    Entity *e = &entities[i];
    if (e->type != SQUARE) continue;
    int e_lbound = e->x;
    int e_rbound = e_lbound + e->width;

    // case: target left w/ entity bounds: return +x on intersect
    int left_rightof_left = t_lbound - e_lbound;
    int left_leftof_right = e_rbound - t_lbound;
    if (left_rightof_left >= 0 && left_leftof_right >= 0) return left_leftof_right;

    // case: target right w/ entity bounds: return -x on intersect
    int el_minus_tr = e_lbound - t_rbound;
    int tr_minus_er = t_rbound - e_rbound;
    if (el_minus_tr <= 0 && tr_minus_er <= 0) return el_minus_tr;
  }
  return 0;
}

int collidesY(int t_lbound, int t_ubound) {
  int i;
  for (i = 0; i < n_entities; i++) {
    Entity *e = &entities[i];
    if (e->type != SQUARE) continue;
    int e_lbound = e->y;
    int e_ubound = e_lbound + e->height;

    // case: target lower: return +y on intersect
    int tl_min_el = t_lbound-e_lbound;
    int eu_min_tl = e_ubound-t_lbound;
    if (tl_min_el >= 0 && eu_min_tl >= 0) return eu_min_tl;

    // case: target upper: return -y on intersect
    int el_minus_tu = e_lbound - t_ubound;
    int tu_minus_eu = t_ubound - e_ubound;
    if (el_minus_tu <= 0 && tu_minus_eu <= 0) return el_minus_tu;
  }
  return 0;

}

/**
 * Entity control in the world. Kind of has nothing to do with bmp-gen, so we
 * gonna move it later. This all has nothing to do with bmp-gen really.
 *
 * It's more of a game, lol. But hey, soon we will move bmp-gen files to
 * /usr/lib/ so that we can be awesome by saying we have created software.
 */
int addEntityToWorld(int x, int y, int type) {
  if (n_entities < MAX_ENTITIES) {
    Entity newEntity = { .x = x, .y = y, .m_x = 0, .m_y = 0, .type = type};
    switch(type) {
      case PLAYER:
        newEntity.width = 10;
        newEntity.height = 10;
        newEntity.drawEntity = drawPlayer;
        break;
      case BALL:
        newEntity.width = 20;
        newEntity.height = 20;
        newEntity.drawEntity = drawBall;
        break;
      case SQUARE:
        newEntity.width = 50;
        newEntity.height = 50;
        newEntity.drawEntity = drawSquareEnt;
        break;
      default:
        return 1;
    }
    entities[n_entities] = newEntity;
    n_entities++;
    fprintf(stdout,"Entity (type: %d) successfully added\n", type);
    return 0;
  }
  fprintf(stderr,"Maximum number of entities reached\n");
  return 1;
}

int main (int argc, char **argv)
{
    // Generate square 480x480 image
    int height = 600;
    int width = 600;

    struct Sprite googah;
    loadSpriteFromFile(&googah, "cheese.bmp");

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
    struct Canvas *background= initCanvas(width, height, "background.bmp");
    struct Canvas *zeal = initCanvas(width, height, "zeal.bmp");
    int explorerWidth = width / 3;
    struct Canvas *workspaceWindow = initCanvas(explorerWidth, height, "explorer.bmp");

    INIT_COLOR(C_red,0xff0000);
    INIT_COLOR(C_green,0x00ff00);
    INIT_COLOR(C_blue,0x0000ff);
    INIT_COLOR(slack_blue,0x321eb1);
    INIT_COLOR(C_fuschia,0xe60368);
    INIT_COLOR(C_yeller,0xf9db17);
    INIT_COLOR(C_indigo,0x6e00fd);
    INIT_COLOR(C_bluebird,0x1cc2b5);
    INIT_COLOR(C_white,0xffffff);
    INIT_COLOR(C_black,0x000000);

    drawRect(workspaceWindow,0,0,workspaceWindow->width,workspaceWindow->height,&C_bluebird);

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

    int i,j;
    for (i = 0; i < beall->width; i++) {
      ColorVec clr = getColorFromGradient(&G_beautiful,(float)i/(float)background->width);
      drawLine(background,i,0,i,background->height,&clr);
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
                 ButtonPressMask | ButtonReleaseMask | PointerMotionMask | FocusChangeMask);

    GC gc;
    gc = XCreateGC(dis,win,0,0);
    XSetBackground(dis,gc,white);
    XSetForeground(dis,gc,black);

    // XImage specs to match Canvas
      int depth = 24; // works fine with depth = 24
      int bitmap_pad = 32; // 32 for 24 and 32 bpp, 16, for 15&16
      unsigned char *image32=(unsigned char *)malloc(width*height*4);
      XImage *xim= XCreateImage(dis, vis, depth, ZPixmap, 0, image32, width, height, bitmap_pad, 0);
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
    int boy_x_prev = boy_x, boy_y_prev = boy_y;


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
    int right_pressed;
    int left_pressed;
    int acceling_right = 0;
    int acceling_left = 0;
    int watermark = 0;

    char workspace_display = 0;
    char toggle_guy = 0;

    int pressedX, pressedY;
    int releasedX, releasedY;

		int sqrt= 6;
		int hl_x = -1;
		int hl_y = -1;

    /**
     * Run Loop
     */
    printf("Press q or esc on the window to quit! Or ctrl-c here!!!\n");
    while (running) {
      startFrameCalc = get_time();
      right_pressed = 0;
      left_pressed = 0;

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
                boy_on_ground = 0;
                boy_momentum_y = 10;
                break;
              case XK_a:
                left_pressed = 1;
                acceling_left = 1;
                break;
              case XK_d:
                right_pressed = 1;
                acceling_right = 1;
                break;
              case XK_Left:
                sqrt--;
								if (sqrt < 1) sqrt= 1;
								printf("Sqrt chang'd to %d.\n",sqrt);
                break;
              case XK_Right:
                sqrt++;
								printf("Sqrt chang'd to %d.\n",sqrt);
                break;
              case XK_p:
                workspace_display = (workspace_display==1) ? 0 : 1;
                break;
              case XK_z:
                toggle_guy = (toggle_guy==1) ? 0 : 1;
                break;
            }
            break;
          case KeyRelease:
            XLookupString(&event.xkey,NULL,0,&keyGet,0);
            switch(keyGet) {
              case XK_a:
              acceling_left = 0;
              break;
              case XK_d:
              acceling_right = 0;
              break;
            }
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
								printf("Rclick registered at (%d,%d).\n",event.xbutton.x,event.xbutton.y);
                break;
              case Button3:
								printf("Haha, you left clicked at (%d,%d)! L O L!!!\n",event.xbutton.x,event.xbutton.y);
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

			int plib_x = 115, plib_y = 115, plib_width = 70, plib_height = 70;
			int plib_active;

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
					fillRectWH(beall,i%sqrt*chunckwid+1,i/sqrt*chunckhei+1,chunckwid-2,chunckhei-2,i%2==0?&C_yeller:i/sqrt%2==0?&C_bluebird:&C_indigo):
					i/sqrt%2==0?
						fillRectWH(beall,i%sqrt*chunckwid+1,i/sqrt*chunckhei+1,chunckwid-2,chunckhei-2,i%2==0?&C_yeller:&C_bluebird):
						fillRectWH(beall,i%sqrt*chunckwid+1,i/sqrt*chunckhei+1,chunckwid-2,chunckhei-2,i%2!=0?&C_yeller:&C_indigo);

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

			etchRectWH(beall,plib_x,plib_y,plib_width,plib_height,&C_black);
			plib_active?
			fillRectWH(beall,plib_x+1,plib_y+1,plib_width-2,plib_height-2,&C_red):
			fillRectWH(beall,plib_x+1,plib_y+1,plib_width-2,plib_height-2,&C_white);

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

    if (save_file==1) {
      generateBitmapImage(beall);
      printf("Image generated!!\n");
    }

    free(beall);
    destroySprite(&googah);
}

int toggleAnimation(void) {
	printf("Write the function ToggleAnimation!!! Oh, and develop the class, yo!\n");
	return 0;
}
