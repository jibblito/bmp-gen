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

// Float data, for now.. No reason to expand beyond just yet (float'll do)
struct TimeSeries {
  int length;
  float min;
  float max;
  float *data;
};

// Robot Time series: Battery, X, Y for each individual robot
struct RobotTimeSeries {
  int length;
  float *battery, *x, *y;
};

struct TimeSeries *initTimeSeries(char* file);
void graphTimeSeries(struct Canvas *cvs, struct TimeSeries *ts);

struct RobotTimeSeries *initRobotTimeSeries(char* file);
int addRtsData(struct RobotTimeSeries *rts, float bat, float x, float y);
void graphRobotTimeSeries(struct Canvas *cvs, struct RobotTimeSeries *rts, int draw_bg, struct ColorVec *fn_clr);
void graphRobotTimeSeriesFrame(struct Canvas *cvs, struct RobotTimeSeries *rts, int draw_bg, struct ColorVec *fn_clr, int frame);

#endif
