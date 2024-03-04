#include "plots.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main (int argc, char** argv) {
  if (argc < 2) {
    printf("Provide an ar-goo-ment, dude!\n");
    return 22;
  }

  char* filename = "shanahan.csv";
  int file = open(filename,O_CREAT| O_WRONLY | O_TRUNC);
  // CHANGE PERMS ON SHANAHAN CSV TODO!!!

  int i;
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
  
  int n_robots = 3;
  // Initialise pointers
  struct RobotTimeSeries **robotarium = malloc(sizeof(struct RobotTimeSeries *) * n_robots);

  for (i = 0; i < n_robots; i++) {
    robotarium[i] = initRobotTimeSeries(NULL);
    printf("Len[%d]: %d\n",i,robotarium[i]->length);
  }

  

  


  struct RobotTimeSeries *rts = initRobotTimeSeries("robot_movement.csv");
  struct Canvas *cvs = initCanvas(100,100,"robot_plot.bmp");
  printf("main Pointer to rts: %p\n",rts);
  printf("main Pointer to rts->x: %p\n",rts->x);

  //graphRobotTimeSeries(cvs,rts,1);

  //generateBitmapImage(cvs);
  free(rts);
  free(robotarium);
}
