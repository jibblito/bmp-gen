#ifndef PLOTS_H
#define PLOTS_H

/**
 * Plots - Plot a time series of data, dude!
 *
 * Working on the formats, but they will be dope!
 */

#include "../canvas.h"
#include "../colorvec.h"

#define MAX_TS_SIZE 512
#define GRID_SIZE 64

// Float data, for now.. No reason to expand beyond just yet (float'll do)
struct TimeSeries {
  int length;
  float min;
  float max;
  float *data;
};

// DataGrid Time series
struct DataGridTimeSeries {
  int length;
  unsigned char dataGrid[MAX_TS_SIZE][GRID_SIZE];
};

// Robot Time series: Battery, X, Y for each individual robot
struct RobotTimeSeries {
  int length;
  float battery[MAX_TS_SIZE], x[MAX_TS_SIZE], y[MAX_TS_SIZE];
};

struct TimeSeries *initTimeSeries(char *data_file);
void graphTimeSeries(struct Canvas *cvs, struct TimeSeries *ts);

struct DataGridTimeSeries *initDataGridTimeSeries(char *data_file);
int addGridData(struct DataGridTimeSeries *dgts, unsigned char dataGrid[GRID_SIZE]);

struct RobotTimeSeries *initRobotTimeSeries(char *data_file);
int addRtsData(struct RobotTimeSeries *rts, float bat, float x, float y);
void graphRobotTimeSeries(struct Canvas *cvs, struct RobotTimeSeries *rts, int draw_bg, struct ColorVec *fn_clr);
void graphRobotTimeSeriesFrame(struct Canvas *cvs, struct RobotTimeSeries *rts, int draw_bg, struct ColorVec *fn_clr, int frame);

#endif
