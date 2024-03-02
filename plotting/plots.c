
#include "plots.h"
#include "../shapes.h"
#include "../colorvec.h"
#include <stdlib.h>

struct TimeSeries *initTimeSeries (int length) {
  struct TimeSeries *ts = malloc(sizeof(struct TimeSeries) + sizeof(float) * length);
  ts->length = length;
  ts->data = (float*) ts + sizeof(float) * length;
  return ts;
}

void graphTimeSeries(struct Canvas *cvs, struct TimeSeries *ts) {

  struct ColorVec *bg_clr = initColor(255,255,255);
  struct ColorVec *axis_clr = initColor(0,0,0);
  struct ColorVec *fn_clr = initColor(255,0,0);
  drawLine(cvs,0,0,0,cvs->width,bg_clr);
  drawLine(cvs,0,0,cvs->width,0,bg_clr);

}

