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
    int height = 480;
    int width = 480;

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

    INIT_COLOR(red,0xff0000);
    INIT_COLOR(green,0x00ff00);
    INIT_COLOR(blue,0x0000ff);
    INIT_COLOR(slack_blue,0x321eb1);
    INIT_COLOR(fuschia,0xe60368);
    INIT_COLOR(yeller,0xf9db17);
    INIT_COLOR(indigo,0x6e00fd);
    INIT_COLOR(bluebird,0x1cc2b5);
    INIT_COLOR(whitxe,0xffffff);
    INIT_COLOR(blaq,0x000000);

    drawRect(workspaceWindow,0,0,workspaceWindow->width,workspaceWindow->height,&bluebird);

    struct ColorVecGradient gradient1;
    gradient1.n_colors = 0;
    addColorToColorVecGradient(&gradient1,&green);
    addColorToColorVecGradient(&gradient1,&blue);
		printGradient(&gradient1);

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
    beautiful.loop = 1;

    /**
     * experiment zone
     */
    
    struct ColorVecGradient grnRedBlu;
    grnRedBlu.n_colors = 0;
    addColorToColorVecGradient(&grnRedBlu,&green);
    addColorToColorVecGradient(&grnRedBlu,&red);
    addColorToColorVecGradient(&grnRedBlu,&blue);
    grnRedBlu.loop = 1;

    INIT_GRADIENT(blackwhite);
    addColorToColorVecGradient(&blackwhite,&blaq);
    addColorToColorVecGradient(&blackwhite,&whitxe);

    int i,j;
    for (i = 0; i < beall->width; i++) {
      struct ColorVec clr = getColorFromGradient(&beautiful,(float)i/(float)background->width);
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
                 ButtonPressMask | ButtonReleaseMask);

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

    /**
     * Run Loop
     */
    printf("Press q or esc on the window to quit! Or ctrl-c here!!!\n");
    while (running) {
      startFrameCalc = get_time();
      right_pressed = 0;
      left_pressed = 0;


      /**
       * Block: Register X Inputs
       */
      int p_watermark;
      while(p_watermark = XPending(dis)) {
        if (p_watermark > watermark) {
          watermark = p_watermark;
          printf("world record: max events per frame: %d\n",watermark);
        }
        XNextEvent(dis,&event);
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
          case ButtonPress:
            XButtonEvent xbutton = event.xbutton;
            switch(xbutton.button) {
              case Button1:
                addEntityToWorld(xbutton.x,xbutton.y,BALL);
                break;
              case Button3:
                addEntityToWorld(xbutton.x,xbutton.y,SQUARE);
                break;
              break;
            }
            break;
        }
        break;
      }
      XFlush(dis);

      /**
       * Block: Change state of game based on input
       */

      // if (boy_on_ground) {
      //   if (acceling_right || right_pressed) {
      //     boy_momentum_x += accel_x;
      //   }

      //   if (acceling_left || left_pressed) {
      //     boy_momentum_x -= accel_x;
      //   }
      // }

      // boy_x_prev = boy_x;
      // boy_y_prev = boy_y;
      // boy_y -= boy_momentum_y;
      // boy_x += boy_momentum_x;

      // if (boy_on_ground == 0)
      //   boy_momentum_y -= gravity_constant;
      // else {
      //   if (boy_momentum_x < 0-friction_constant)
      //     boy_momentum_x += friction_constant;
      //   else if (boy_momentum_x > friction_constant)
      //     boy_momentum_x -= friction_constant;
      //   else
      //     boy_momentum_x = 0;
      // }

      // if (doesCollide(boy_x,boy_y)) {
      //   boy_x++;
      // }

      // int correctionX = collidesX(boy_x,boy_x+boy_width);
      // int correctionY = collidesY(boy_y,boy_y+boy_width);
      // if (correctionX != 0) {
      //   boy_x += correctionX;
      //   boy_momentum_x = 0;
      // }

      // if (boy_y < 0) {
      //   boy_y = 0;
      //   boy_momentum_y = 0;
      // }
      // if (boy_y > height) {
      //   boy_on_ground = 1;
      //   boy_y = height;
      //   boy_momentum_y = 0;
      // }
      // if (boy_x <= 0) {
      //   boy_momentum_x = 0;
      //   boy_x = 0;
      // }
      // if (boy_x > width) {
      //   boy_momentum_x = 0;
      //   boy_x = width;
      // }

      // /**
      //  * Block: Change natural state of game
      //  */

      // for (i = 0; i < n_entities; i++) {
      //   Entity *e = &entities[i];
      //   // Move entities
      //   if (e->type==SQUARE) continue;
      //   e->x += e->m_x;
      //   e->y -= e->m_y;

      //   e->m_y -= gravity_constant;
      //   if (e->y < 0) {
      //     e->y = 0;
      //     e->m_y = 0;
      //   }
      //   if (e->y > height-1) {
      //     e->y = height-1;
      //     e->m_y = (e->m_y *-1) /2;
      //   }
      //   if (e->x <= 0) {
      //     e->x = 0;
      //     e->m_x *= -1;
      //   }
      //   if (e->x > width-1) {
      //     e->x = width-1;
      //     e->m_x *= -1;
      //   }
      //   if (e->m_x > 10 || e->m_x < -10) e->m_x = 0;
      // }

      /**
       * Block: Manipulate canvas
       */

      drawRect(beall,0,0,width,height,&blaq);
      drawRect(beall,width*9/10,width*9/10,width/10,height/10,&yeller);
      etchCircle(beall,width/3,height/3,width/4,&fuschia);

      // float clockrotation = (float)iter/(float)period1;
      // float sin1 = sinf((float)iter/48.3f);
      // float sin2 = sinf((float)iter/49.0f);
      // float iterstraight = (float)(iter % 100)/100.0f;
      // float iterstraight2 = (float)(iter % 1000)/1000.0f;
      // struct ColorVec clr = getColorFromGradient(&beautiful,sin1);
      // struct ColorVec clr2 = getColorFromGradient(&grnRedBlu,iterstraight);
      // struct ColorVec clr3 = getColorFromGradient(&beautiful,iterstraight);

      // etchCircle(beall,boy_x+boy_width/2,boy_y, boy_width/2,&clr);
      // etchCircle(beall,boy_x+boy_width/2,boy_y, boy_width/2-1,&blaq);
      // etchCircle(beall,boy_x+boy_width/2,boy_y, boy_width/2-2,&clr2);



      // drawLine(zeal,boy_x_prev,boy_y_prev,boy_x,boy_y,&clr2);
      drawRect(beall,30,30,40,40,&yeller);
      drawRect(beall,30,40,40,50,&indigo);
      drawRect(beall,30,50,40,60,&bluebird);
      drawSpriteToCanvas(beall,&googah,100,100);

      drawRect(beall,50,50,60,60,&red);
      drawRect(beall,50,50,60,60,&green);
      drawRect(beall,50,50,60,60,&blue);
      drawRect(beall,1,1,11,11,&red);

      // for (i = 0; i < n_entities; i++) {
      //   entities[i].drawEntity(&entities[i],beall,&yeller);
      // }
      

      /**
       * Block: Draw X Image to screen 
       */
      flashCanvasToXImage(beall,xim,0,0);
      // if (toggle_guy == 1 ) flashCanvasToXImage(zeal,xim,0,0);
      // if (workspace_display == 1 ) flashCanvasToXImage(workspaceWindow,xim,(width/3)*2,0);
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
    destroySprite(&googah);
}
