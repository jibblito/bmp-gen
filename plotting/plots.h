#ifndef PLOTS_H
#define PLOTS_H

/**
 * Plots - Plot a time series of data, dude!
 *
 * Working on the formats, but they will be dope!
 */

#include "../canvas.h"

// Float data, for now.. No reason to expand beyond just yet (float'll do)
struct TimeSeries {
  int length;
  float min;
  float max;
  float *data;
};

struct TimeSeries *initTimeSeries(char* file);
void graphTimeSeries(struct Canvas *cvs, struct TimeSeries *ts);

#endif
