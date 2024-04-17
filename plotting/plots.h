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
#define GRID_SIZE 900

typedef struct Vec2dor {
  float x,y;
} Vec2d;

// Float data, for now.. No reason to expand beyond just yet (float'll do)
struct TimeSeries {
  int length;
  float min;
  float max;
  float *data;
};

// Data Gridde
struct DataGrid {
  unsigned char data[GRID_SIZE];
};

// DataGrid Time series
struct DataGridTimeSeries {
  int length;
  struct DataGrid grids[MAX_TS_SIZE];
};

// Robot Time series: Battery, X, Y for each individual robot
struct RobotTimeSeries {
  int length;
  float battery[MAX_TS_SIZE];
  Vec2d loc[MAX_TS_SIZE], moment[MAX_TS_SIZE];
};

struct TimeSeries *initTimeSeries(char *data_file);
void graphTimeSeries(struct Canvas *cvs, struct TimeSeries *ts);

struct DataGrid *initDataGrid(unsigned char dataGrid[GRID_SIZE]);

struct DataGridTimeSeries *initDataGridTimeSeries(char *data_file);
int addGridData(struct DataGridTimeSeries *dgts, unsigned char dataGrid[GRID_SIZE]);

struct RobotTimeSeries *initRobotTimeSeries(char *data_file);
int addRtsData(struct RobotTimeSeries *rts, float bat, Vec2d location, Vec2d moment);
void graphRobotTimeSeries(struct Canvas *cvs, struct RobotTimeSeries *rts, struct ColorVec *fn_clr);
void graphRobotTimeSeriesFrame(struct Canvas *cvs, struct RobotTimeSeries *rts, int draw_bg, struct ColorVec *fn_clr, int frame);

float vectorDistance(Vec2d from, Vec2d to);

float clamp255(float input);


#endif
