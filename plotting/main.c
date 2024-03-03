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

  int i;
  dprintf(file,"DANG-SHEP\n");
  for (i = 0; i < 100; i++) {
    dprintf(file,"%1.3d\n",i/2);
  }
  close(file);

  struct TimeSeries *ts = initTimeSeries(argv[1]);
  struct TimeSeries *ts2 = initTimeSeries(filename);
  struct Canvas *cvs = initCanvas(ts2->length + 1,(int)(ts2->max - ts2->min),"bin.bmp");
  graphTimeSeries(cvs,ts2);
  generateBitmapImage(cvs);
  free(ts);
  free(cvs);
}
