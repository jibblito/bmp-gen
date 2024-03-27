#include "plots.h"
#include "../shapes.h"
#include "../colorvec.h"
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

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
    dgts->dataGrid[dgts->length][i] = dataGrid[i];
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
      rts->x[length] = x;
      rts->y[length] = y;
      length++;
    }
  }
  rts->length = length;
  return rts;
}

// add a RTS triplet to a data struct
int addRtsData(struct RobotTimeSeries *rts, float bat, float x,  float y, float x_vec, float y_vec) {
  if (rts->length == MAX_TS_SIZE) {
    printf("Time series exceeds MAX_TS_SIZE\n");
    return 12;
  }
  rts->battery[rts->length] = bat;
  rts->x[rts->length] = x;
  rts->y[rts->length] = y;
  rts->x_vec[rts->length] = x_vec;
  rts->y_vec[rts->length] = y_vec;
  rts->length = rts->length + 1;
  return rts->length;
}

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
void graphRobotTimeSeries(struct Canvas *cvs, struct RobotTimeSeries *rts, int draw_bg, struct ColorVec *fn_clr) {
  if (draw_bg == 1) {
    struct ColorVec bg_clr, axis_clr;
    initColor(&bg_clr,255,255,255);
    initColor(&axis_clr,0,0,0);

    drawRect(cvs,0,0,cvs->width-1,cvs->height-1,&bg_clr);
    drawLine(cvs,0,0,0,cvs->height,&axis_clr);
    drawLine(cvs,0,cvs->height-1,cvs->width-1,cvs->height-1,&axis_clr);
    drawLine(cvs,cvs->width-1,cvs->height-1,cvs->width-1,0,&axis_clr);
    drawLine(cvs,cvs->width-1,0,0,0,&axis_clr);
  }

  int i;
  for (i = 0; i < rts->length; i++) {
    // printf("rts[%d] (x,y): (%3.3f),(%3.3f)\n",i,rts->x[i],rts->y[i]);
    etchCircle(cvs,(int)rts->x[i],(int)rts->y[i],3,fn_clr);
  }
}



// Graph a robot time series (single-frame)
void graphRobotTimeSeriesFrame(struct Canvas *cvs, struct RobotTimeSeries *rts, int draw_bg, struct ColorVec *fn_clr, int frame) {
  etchCircle(cvs,(int)rts->x[frame],(int)rts->y[frame],3,fn_clr);
}
