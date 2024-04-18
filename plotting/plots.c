#include "plots.h"
#include "../shapes.h"
#include "../colorvec.h"
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#define BUF_SIZE 2048

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

struct DataGrid *initDataGrid(unsigned char dataGrid[GRID_SIZE]) {
  size_t dg_size = sizeof(struct DataGrid);
  struct DataGrid *dg = malloc(dg_size);
  int i;
  for (i = 0; i < GRID_SIZE; i++) {
    dg->data[i] = dataGrid[i];
  }
  return dg;
}

struct DataGridTimeSeries *initDataGridTimeSeries(char *data_file) {
  // RobotTimeSeries struct | battery | x | y
  size_t dgts_size = sizeof(struct DataGridTimeSeries);
  struct DataGridTimeSeries *dgts = malloc(dgts_size); 
  int length = 0;

  if (data_file != NULL) {
    // Worry about this later, reading DATA from FILE -- copy from initRobotTimeSeries
  }
  dgts->length = length;
  return dgts;

}

int addGridData(struct DataGridTimeSeries *dgts, unsigned char dataGrid[GRID_SIZE]) {
  if (dgts->length == MAX_TS_SIZE) {
    printf("Time series exceeds MAX_TS_SIZE\n");
    return 12;
  }
  int i;
  for (i = 0; i < GRID_SIZE; i++) {
    dgts->grids[dgts->length].data[i] = dataGrid[i];
  }
  dgts->length = dgts->length + 1;
  return dgts->length;

}

struct RobotTimeSeries *initRobotTimeSeries (char* data_file) {
  // RobotTimeSeries struct | battery | x | y
  size_t rts_size = sizeof(struct RobotTimeSeries);
  struct RobotTimeSeries *rts = malloc(rts_size); 
  int length = 0;

  if (data_file != NULL) {
    int file = open(data_file, O_RDONLY);
    if (file == -1) {
      printf("open() failed: errno %d: %s\n",errno,strerror(errno));
      return NULL;
    }

    char buf[BUF_SIZE];
    char *saveptr1, *saveptr2;
    char* token = strtok_r(buf,"\n",&saveptr1);
    char *series_labels = token; // CSV header
   
    read(file,buf,BUF_SIZE);
    // Read major token for each time step
    while ((token = strtok_r(NULL,"\n",&saveptr1)) != NULL) {
      // Read Subtoken inside each time step
      float battery = atof(strtok_r(token,",",&saveptr2));
      float x = atof(strtok_r(NULL,",",&saveptr2));
      float y = atof(strtok_r(NULL,",",&saveptr2));

      rts->battery[length] = battery;
      rts->loc[length].x = x;
      rts->loc[length].y = y;
      length++;
    }
  }
  rts->length = length;
  return rts;
}

// add a RTS triplet to a data struct
int addRtsData(struct RobotTimeSeries *rts, float bat, Vec2d location, Vec2d moment) {
  if (rts->length == MAX_TS_SIZE) {
    printf("Time series exceeds MAX_TS_SIZE\n");
    return 12;
  }

  rts->battery[rts->length] = bat;
  rts->loc[rts->length] = location;
  rts->moment[rts->length] = moment;

  rts->length = rts->length + 1;
  return rts->length;
}

// Graph a regular ole time series
void graphTimeSeries(struct Canvas *cvs, struct TimeSeries *ts) {
  struct ColorVec bg_clr, axis_clr, fn_clr;
  initColor(&bg_clr,255,255,255);
  initColor(&axis_clr,0,0,0);
  initColor(&fn_clr,255,0,0);
  drawLine(cvs,0,0,0,cvs->width,&bg_clr);
  drawLine(cvs,0,0,cvs->width,0,&bg_clr);

  int i;
  for (i = 0; i < ts->length; i++) {
    plot(cvs,i,ts->data[i],&fn_clr);
  }

}

// Graph a robot time series
void graphRobotTimeSeries(struct Canvas *cvs, struct RobotTimeSeries *rts, struct ColorVec *fn_clr) {
  int i;
  for (i = 0; i < rts->length; i++) {
    int x = rts->loc[i].x;
    int y = rts->loc[i].y;
    if (rts->battery[i] <= 0.0f) {
      drawLine(cvs,x-2,y-2,x+3,y+3,fn_clr);
      drawLine(cvs,x-2,y+2,x+2,y-2,fn_clr);
    } else {
      etchCircle(cvs,x,y,3,fn_clr);
    }
  }
}

// Graph a robot time series (single-frame)
void graphRobotTimeSeriesFrame(struct Canvas *cvs, struct RobotTimeSeries *rts, int draw_bg, struct ColorVec *fn_clr, int frame) {
  int x = rts->loc[frame].x;
  int y = rts->loc[frame].y;
  if (rts->battery[frame] <= 0.0f) {
    drawLine(cvs,x-2,y-2,x+3,y+3,fn_clr);
    drawLine(cvs,x-2,y+2,x+2,y-2,fn_clr);
  } else {
    etchCircle(cvs,x,y,3,fn_clr);
  }
}

// Find vector distance
float vectorDistance(Vec2d *from, Vec2d *to) {
  return sqrt(pow(to->x-from->x,2) + pow(to->y-from->y,2));
}

// Find length of a vector
float vectorScale(Vec2d *vec) {
  return sqrt(pow(vec->x,2)+pow(vec->y,2));
}

// Clamp a float
float clamp255(float input) {
  if (input < 0) return 0;
  if (input > 255) return 255;
  return input;
}
