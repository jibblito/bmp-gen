#include "plots.h"
#include "../shapes.h"
#include "../colorvec.h"
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#define MAX_FILE_SIZE 2048
#define BUF_SIZE 2048
#define MAX_TS_SIZE 512

struct TimeSeries *initTimeSeries (char* data_file) {
  int file = open(data_file, O_RDONLY);

  if (file == -1) {
    printf("open() failed: errno %d: %s\n",errno,strerror(errno));
    return NULL;
  }

  char buf[BUF_SIZE];
  read(file,buf,BUF_SIZE);

  struct TimeSeries *ts = malloc(sizeof(struct TimeSeries) + sizeof(float) * MAX_TS_SIZE);
  ts->data = (float*) ts + sizeof(struct TimeSeries);

  int length = 0;
  char* token = strtok(buf,"\n");
  char *series_label = strtok(NULL,"\n"); // First line of file
  while ((token = strtok(NULL,"\n")) != NULL) {
    float dingus = (float) atol(token);

    if (length == 0) {
      ts->min = dingus;
      ts->max = dingus;
    } else {
      if (dingus < ts->min) ts->min = dingus;
      if (dingus > ts->max) ts->max = dingus;
    }

    ts->data[length] = dingus;
    length++;
  }

  ts->length = length;
  return ts;
}

void graphTimeSeries(struct Canvas *cvs, struct TimeSeries *ts) {
  struct ColorVec *bg_clr = initColor(255,255,255);
  struct ColorVec *axis_clr = initColor(0,0,0);
  struct ColorVec *fn_clr = initColor(255,0,0);
  drawLine(cvs,0,0,0,cvs->width,bg_clr);
  drawLine(cvs,0,0,cvs->width,0,bg_clr);

  int i;
  for (i = 0; i < ts->length; i++) {
    plot(cvs,i,ts->data[i],fn_clr);
  }

}

struct RobotTimeSeries *initRobotTimeSeries (char* data_file) {
  int file = open(data_file, O_RDONLY);

  if (file == -1) {
    printf("open() failed: errno %d: %s\n",errno,strerror(errno));
    return NULL;
  }

  char buf[BUF_SIZE];
  read(file,buf,BUF_SIZE);

  struct RobotTimeSeries *rts = malloc(sizeof(struct RobotTimeSeries) +
                                       sizeof(float) * MAX_TS_SIZE * 3); // 3 float series: battery, x, y
  // Three data set pointers, each equi-sized
  rts->battery = (float*) rts + sizeof(struct RobotTimeSeries);
  rts->x = (float*) rts->battery + sizeof(float) * MAX_TS_SIZE;
  rts->y = (float*) rts->x + sizeof(float) * MAX_TS_SIZE;

  printf("s:%p, bat:%p, x:%p, y:%p\n",rts,rts->battery,rts->x,rts->y);

  char *saveptr1, *saveptr2;
  int length = 0;
  char* token = strtok_r(buf,"\n",&saveptr1);
  char *series_label = strtok_r(NULL,"\n",&saveptr1); // First line of file (csv header)
 
  // Read major token for each time step
  while ((token = strtok_r(NULL,"\n",&saveptr1)) != NULL) {
    // Read Subtoken inside each time step
    char *subtoken = token;
    float battery = atof(strtok_r(subtoken,",",&saveptr2));
    float x = atof(strtok_r(NULL,",",&saveptr2));
    float y = atof(strtok_r(NULL,",",&saveptr2));

    rts->battery[length] = battery;
    rts->x[length] = x;
    rts->y[length] = y;
    length++;
  }
  rts->length = length;
  return rts;
}

