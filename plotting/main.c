#include "plots.h"
#include "../shapes.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

typedef struct location {
  float x,y;
} LOC;

void validateLoc(LOC *l, float width) {
  if(l->x < 0) l->x = 0;
  if(l->x >= width) l->x = width;
  if(l->y < 0) l->y = 0;
  if(l->y >= width) l->y = width;
}

typedef struct xyVector {
  float x_v,y_v;
} VEC;

void normalizeVector(VEC *v) {
  float full_vec = sqrt(pow(v->x_v,2)+pow(v->y_v,2));
  if (full_vec < 0.2f) {
    v->x_v = 0;
    v->y_v = 0;
  } else if (full_vec < 2.5f) {
    v->x_v = v->x_v/2;
    v->y_v = v->y_v/2;
  } else {
    float normal_ratio = 1/full_vec;
    v->x_v = v->x_v * normal_ratio;
    v->y_v = v->y_v * normal_ratio;
  }
}

void scaleVector(VEC *v, float scale) {
  v->x_v = v->x_v * scale;
  v->y_v = v->y_v * scale;
}


int main (int argc, char** argv) {
  
  // Limit of 10 for now
  int n_robots = 6;
  int max_battery = 100;
  int i,j,k,l;

  // Display constants
  int _ROWLENGTH, _ROBOTARIUM_WIDTH, _CELL_WIDTH;

  _ROWLENGTH = round(sqrt(GRID_SIZE));
  _ROBOTARIUM_WIDTH = 100 + _ROWLENGTH - (100 % _ROWLENGTH);
  _CELL_WIDTH = _ROBOTARIUM_WIDTH / _ROWLENGTH;

  // Initialise pointers
  struct RobotTimeSeries *robotarium[10];
  for (i = 0; i < n_robots; i++) {
    robotarium[i] = initRobotTimeSeries(NULL);
  }


  // Prepare charger locations
  int n_chargers = n_robots;
  LOC *chargers = malloc(sizeof(LOC) * n_chargers);
  int frac = _ROBOTARIUM_WIDTH/(n_chargers/2);
  int offset = frac/2;
  for (i = 0; i < 2; i++) {
    if (i%2==0) chargers[i].y = 0;
    else chargers[i].y = _ROBOTARIUM_WIDTH-1;
    chargers[i].x = _ROBOTARIUM_WIDTH/2;
  }
  for (;i<n_chargers;i++) {
    if (i%2==0) chargers[i].x = 0;
    else chargers[i].x = _ROBOTARIUM_WIDTH-1;
    chargers[i].y = (i/2)*frac + offset;
  }

  // Prepare time series data sheets for robot locations
  for (i = 0; i < n_robots; i++) {
    addRtsData(robotarium[i],max_battery,chargers[i].x,chargers[i].y,0,0);
  }

  // Prepare DataGrid time series
  struct DataGridTimeSeries *DATA_coverage = initDataGridTimeSeries(NULL);
  unsigned char emptyGrid[GRID_SIZE] ={0};
  addGridData(DATA_coverage,emptyGrid);

  int n_iterations = 500;
  float robot_speed = 2.5;
  float resolution = 0.1;
  int weights_x[10] = {0};
  int weights_y[10] = {0};
  int cell_count[10] = {0};
  LOC centroids[10];
  LOC cells_of_interest[GRID_SIZE] = {0};
  int interesting_cells;

  // Loop: Find centroids for each robot, the apply vector. Generate time series
  for (i = 0; i <= n_iterations; i++) {

    for (j = 0; j < n_robots; j++) {
      weights_x[j] = 0;
      weights_y[j] = 0;
      cell_count[j] = 0;
    }

    interesting_cells = 0;
    
    for (j = 0; j < GRID_SIZE; j++) {
      if(DATA_coverage->dataGrid[DATA_coverage->length-1][j] == 0) {
        LOC poi;
        poi.x = (j % _ROWLENGTH) * _CELL_WIDTH + _CELL_WIDTH/2;
        poi.y = (j / _ROWLENGTH) * _CELL_WIDTH + _CELL_WIDTH/2;
        cells_of_interest[interesting_cells] = poi;
        interesting_cells++;
      }
    }
    
    // Iterate cells of resolution `resolution`
    for (j = 1; j < _ROBOTARIUM_WIDTH-2; j = j + ((float)_ROBOTARIUM_WIDTH)*resolution) {
      for (k = 1; k < _ROBOTARIUM_WIDTH-2; k = k + ((float)_ROBOTARIUM_WIDTH)*resolution) {
        float closest_dist = 10000;
        int closest_dist_index = -1;
        for (l = 0; l < n_robots; l++) {
          struct RobotTimeSeries *robot = robotarium[l];
          float cur_x = robot->x[(robot->length)-1];
          float cur_y = robot->y[(robot->length)-1];
          float dist = sqrt((j-cur_x)*(j-cur_x)+(k-cur_y)*(k-cur_y));
          if (dist < closest_dist){
            closest_dist = dist;
            closest_dist_index = l;
          }
        }
        weights_x[closest_dist_index] += j;
        weights_y[closest_dist_index] += k;
        cell_count[closest_dist_index] += 1;
      }
    }

    // Assign centroids (centers of mass)
    for (j = 0; j < n_robots; j++) {
      if (cell_count[j] == 0) {
        centroids[j].x = robotarium[j]->x[robotarium[j]->length-1];
        centroids[j].y = robotarium[j]->y[robotarium[j]->length-1];
      } else {
        centroids[j].x = (float)weights_x[j] / (float)cell_count[j];
        centroids[j].y = (float)weights_y[j] / (float)cell_count[j];
      }
    }

    // Copy data from last iteration

    unsigned char boxes_covered[GRID_SIZE] = {0};
    if (i > 0) memcpy(boxes_covered, DATA_coverage->dataGrid[DATA_coverage->length-1], GRID_SIZE);

    // Control Loop
    
    struct RobotTimeSeries *robot;
    LOC cur_loc;
    VEC cur_vec;
    float cur_bat;
    float battery_weight, coverage_weight, info_weight;
    LOC optimal_charger, centroid, poi;

    for (j = 0; j < n_robots; j++) {
      robot = robotarium[j];
      optimal_charger = chargers[j%n_chargers];
      centroid = centroids[j];

      cur_loc.x = robot->x[(robot->length)-1];
      cur_loc.y = robot->y[(robot->length)-1];
      cur_bat = robot->battery[(robot->length)-1];

      poi = cells_of_interest[0];
      float poi_dist = sqrt(pow(poi.x-cur_loc.x,2)+pow(poi.y-cur_loc.y,2));
      for (k = 1; k < interesting_cells; k++) {
        LOC ppoi = cells_of_interest[k];
        float ppoi_dist = sqrt(pow(ppoi.x-cur_loc.x,2)+pow(ppoi.y-cur_loc.y,2));
        if(ppoi_dist < poi_dist) {
          poi_dist = ppoi_dist;
          poi = ppoi;
        }
        //if (poi_dist < 1.0f) break;
      }

      if (cur_bat < 20) {
        battery_weight = 1;
        coverage_weight = 0;
        info_weight = 0;
      } else if (interesting_cells == 0) {
        battery_weight = 0;
        coverage_weight = 1;
        info_weight = 0;
      } else {
        battery_weight = 0;
        coverage_weight = 0.2;
        info_weight = 0.8;
      }

      cur_vec.x_v = (optimal_charger.x - cur_loc.x) * battery_weight;
      cur_vec.y_v = (optimal_charger.y - cur_loc.y) * battery_weight;

      cur_vec.x_v += (centroid.x - cur_loc.x) * coverage_weight;
      cur_vec.y_v += (centroid.y - cur_loc.y) * coverage_weight;

      cur_vec.x_v += (poi.x - cur_loc.x) * info_weight;
      cur_vec.y_v += (poi.y - cur_loc.y) * info_weight;

      // Ensure barriers (no collisions)
      for(k = 0; k < n_robots; k++) {
        if(k != j) {
          float diff_x = robotarium[k]->x[robotarium[j]->length-1] - cur_loc.x;
          float diff_y = robotarium[k]->y[robotarium[j]->length-1] - cur_loc.y;
          float barrier_dist = sqrt(pow(diff_x,2)+pow(diff_y,2));
          if(barrier_dist < 5.0f) {
            cur_vec.x_v = -5 * diff_x;
            cur_vec.y_v = -5 * diff_y;
          }
        }
      }

      normalizeVector(&cur_vec);
      scaleVector(&cur_vec,robot_speed);

      if (roundf(cur_loc.x) == optimal_charger.x && roundf(cur_loc.y) == optimal_charger.y) {
        cur_bat = max_battery+1;
      }

      printf("i,j:%d,%d\n",i,j);
      int cell_index = (int)cur_loc.x/_CELL_WIDTH+((int)cur_loc.y/_CELL_WIDTH)*_ROWLENGTH;
      if (!boxes_covered[cell_index]) {
        boxes_covered[cell_index] = (((float)i+1)/(float)n_iterations)*255;
      }

      LOC nextLoc;
      nextLoc.x = cur_loc.x + cur_vec.x_v;
      nextLoc.y = cur_loc.y + cur_vec.y_v;
      validateLoc(&nextLoc,(float)_ROBOTARIUM_WIDTH);

      addRtsData(robot,cur_bat-1,nextLoc.x,nextLoc.y, cur_vec.x_v, cur_vec.y_v);
    }

    addGridData(DATA_coverage, boxes_covered);
  }

  // Initialize COLORS and GRADIENTS

  struct ColorVec bg_clr,axis_clr, charger_clr;
  initColor(&bg_clr,255,255,255);
  initColor(&axis_clr,0,0,0);
  initColor(&charger_clr,230,210,10);

  struct ColorVec green,red,yellow;
  initColor(&green,0,255,0);
  initColor(&red,255,0,0);
  initColor(&yellow,255,255,0);
  struct ColorVecGradient greenToRed;
  greenToRed.n_colors = 0;
  addColorToColorVecGradient(&greenToRed,&green);
  addColorToColorVecGradient(&greenToRed,&red);
  struct ColorVecGradient greenYelRed;
  greenYelRed.n_colors = 0;
  addColorToColorVecGradient(&greenYelRed,&green);
  addColorToColorVecGradient(&greenYelRed,&yellow);
  addColorToColorVecGradient(&greenYelRed,&red);

  struct ColorVec teal,blue,darkBlue;
  initColor(&teal,0,255,240);
  initColor(&blue,0,3,255);
  initColor(&darkBlue,0,2,55);
  struct ColorVecGradient infoGradient;
  infoGradient.n_colors = 0;
  addColorToColorVecGradient(&infoGradient,&teal);
  addColorToColorVecGradient(&infoGradient,&blue);
  addColorToColorVecGradient(&infoGradient,&darkBlue);

  // Generate Images of each frame of simulation

  printf("\nStarting bmp generation loop\n\n");
  struct Canvas *cvs;

  for (i = 0; i < robotarium[0]->length; i++) {

     char filename[32];
     sprintf(filename,"out/00%d.bmp\0",i);
     if (i > 9) sprintf(filename,"out/0%d.bmp\0",i);
     if (i > 99) sprintf(filename,"out/%d.bmp\0",i);
     cvs = initCanvas(_ROBOTARIUM_WIDTH,_ROBOTARIUM_WIDTH,filename);

     for (j = 0; j < GRID_SIZE; j++) {
       int x_lvl = (j % _ROWLENGTH)*_CELL_WIDTH;
       int y_lvl = (j / _ROWLENGTH)*_CELL_WIDTH;

       unsigned char data = DATA_coverage->dataGrid[i][j];
       float degree = (float)data/255.0f;
       struct ColorVec infoLvl = getColorFromGradient(&infoGradient,degree);
       if (data == 0) infoLvl = bg_clr;

       drawRect(cvs,x_lvl,y_lvl,x_lvl+_CELL_WIDTH-1,y_lvl+_CELL_WIDTH-1,&infoLvl);
     }

     drawLine(cvs,0,0,0,cvs->height,&axis_clr);
     drawLine(cvs,0,cvs->height-1,cvs->width-1,cvs->height-1,&axis_clr);
     drawLine(cvs,cvs->width-1,cvs->height-1,cvs->width-1,0,&axis_clr);
     drawLine(cvs,cvs->width-1,0,0,0,&axis_clr);

    for (j = 0; j < n_robots; j++) {
      float battery = robotarium[j]->battery[i];
      float degree = battery/(float)max_battery;
      struct ColorVec battery_lvl = getColorFromGradient(&greenToRed,degree);
      graphRobotTimeSeriesFrame(cvs,robotarium[j],0,&battery_lvl,i);
    }

    for (j = 0; j < n_chargers; j++) {
      LOC l = chargers[j];
      drawRect(cvs,l.x,l.y,l.x,l.y,&charger_clr);
    }
    generateBitmapImage(cvs);
  }

  // END
  for (i = 0; i < n_robots; i++) {
    free(robotarium[i]);
  }
  free(cvs);
  free(chargers);
  return 0;
  // END
}
