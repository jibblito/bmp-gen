#include "plots.h"

int main (int argc, char* argv) {
  struct TimeSeries *ts = initTimeSeries(20);
  struct Canvas *cvs = initCanvas(100,100,"bin.jpg");
  graphTimeSeries(cvs,ts);
  generateBitmapImage(cvs);
}
