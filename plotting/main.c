#include "plots.h"
#include "../shapes.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

int main (int argc, char** argv) {
  if (argc < 2) {
    printf("Provide an ar-goo-ment, dude!\n");
    return 22;
  }

  char* filename = "shanahan.csv";
  int file = open(filename,O_CREAT| O_WRONLY | O_TRUNC);
  // CHANGE PERMS ON SHANAHAN CSV TODO!!!

  int i,j;
  dprintf(file,"DANG-SHEP\n");
  for (i = 0; i < 100; i++) {
    dprintf(file,"%1.3d\n",i/2);
  }
  close(file);

  // struct TimeSeries *ts = initTimeSeries(argv[1]);
  // // struct TimeSeries *ts2 = initTimeSeries(filename);
  // // graphTimeSeries(cvs,ts2);
  // // generateBitmapImage(cvs);
  // free(ts);
  //free(cvs);
  
  int n_robots = 6;
  // Initialise pointers
  struct RobotTimeSeries **robotarium = malloc(sizeof(struct RobotTimeSeries *) * n_robots);
  struct Canvas *cvs = initCanvas(100,100,"robotarium.bmp");

  for (i = 0; i < n_robots; i++) {
    robotarium[i] = initRobotTimeSeries(NULL);
    printf("Len[%d]: %d\n",i,robotarium[i]->length);
  }

  typedef struct location {
    float x,y;
  } LOC;


  int n_chargers = 4;
  LOC *chargers = malloc(sizeof(LOC) * n_chargers);

  int frac = cvs->height/n_chargers;
  int offset = frac/2;
  for (i = 0; i < n_chargers; i++) {
    chargers[i].x = 0;
    chargers[i].y = i*frac + offset;
  }

  // Control Loop: n robots, 
  int n_iterations = 250;
  float robot_speed = 0.11;
  for (i = 0; i < n_robots; i++) {
    addRtsData(robotarium[i],30,rand() % cvs->width, rand() % cvs->width);
  }
  for (i = 0; i < n_robots; i++) {
    struct RobotTimeSeries *robot = robotarium[i];
    LOC optimal_charger = chargers[i%n_chargers];
    for(j = 0; j <= n_iterations; j++) {
      float cur_x = robot->x[robot->length-1];
      float cur_y = robot->y[robot->length-1];
      float cur_bat = robot->y[robot->length-1];

      float x_vec = (optimal_charger.x - cur_x) * robot_speed;
      float y_vec = (optimal_charger.y - cur_y) * robot_speed;

      printf("Robo[%d] x: %3.3f, y: %3.3f\n",i, x_vec,y_vec);

      addRtsData(robot,cur_bat-1,cur_x + x_vec,cur_y + y_vec);
    }
  }
  printf("Done generating data\n");

  struct ColorVec *robot_colors[6] = 
    {
      initColor(255,0,0), initColor(255,255,0),
      initColor(30,255,123), initColor(150,200,200),
      initColor(66,59,201), initColor(200,100,110),
    };
  for (i = 0; i < n_robots; i++) {
    if (i == 0) {
      graphRobotTimeSeries(cvs,robotarium[i],1,robot_colors[i]);
    } else {
      graphRobotTimeSeries(cvs,robotarium[i],0,robot_colors[i]);
    }
  }
  struct ColorVec *charger_clr = initColor(230,210,10);
  for (i = 0; i < n_chargers; i++) {
    LOC l = chargers[i];
    printf("l%d: (%d,%d)\n",i,l.x,l.y);
    drawRect(cvs,l.x-1,l.y-1,l.x+1,l.y+1,charger_clr);
  }
  generateBitmapImage(cvs);


  struct RobotTimeSeries *rts = initRobotTimeSeries("robot_movement.csv");
  printf("main Pointer to rts: %p\n",rts);
  printf("main Pointer to rts->x: %p\n",rts->x);
  free(rts);

  //graphRobotTimeSeries(cvs,rts,1);
  for (i = 0; i < n_robots; i++) {
    free(robotarium[i]);
  }
  free(robotarium);

}
