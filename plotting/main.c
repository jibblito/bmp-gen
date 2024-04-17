#include "plots.h"
#include "robotarium.h"
#include "../shapes.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

void simple_splat(struct DataGrid *dg, Vec2d loc, int CELL_WIDTH, int CELLS_PER_ROW) {
  int cell_y_index = loc.y/CELL_WIDTH;
  int cell_x_index = loc.x/CELL_WIDTH;
  int i,j;

  int radius = 2;
  for (i = -1*radius; i < radius+1; i++) {
    if (cell_x_index + i < 0 || cell_x_index + i > CELLS_PER_ROW-1) continue;

    for (j = -1*radius; j < radius+1; j++) {
      if (cell_y_index + j < 0 || cell_y_index + j > CELLS_PER_ROW-1) continue;
      int full_index = cell_x_index+i + (cell_y_index+j)*CELLS_PER_ROW;
      float current = dg->data[full_index];
      float potential = current/4 + 255 - 30*abs(i) - 30*abs(j);
      if (current < potential) dg->data[full_index] = clamp255(potential);
    }
  }
}

int main (int argc, char** argv) {

  int i,j,k,l;
  // Constants for calculating centroid and display cells

  int CELLS_PER_ROW, ARENA_WIDTH_IN_PIXELS, CELL_WIDTH;

  CELLS_PER_ROW = round(sqrt(GRID_SIZE));
  ARENA_WIDTH_IN_PIXELS = 150 - (150 % CELLS_PER_ROW);
  CELL_WIDTH = ARENA_WIDTH_IN_PIXELS / CELLS_PER_ROW;

  // Initialize arena

  int n_robots = 6;
  ARENA *arena = initArena(n_robots,ARENA_WIDTH_IN_PIXELS);

  // Prepare DataGrid time series

  struct DataGridTimeSeries *DATA_coverage = initDataGridTimeSeries(NULL);
  struct DataGridTimeSeries *DATA_phi = initDataGridTimeSeries(NULL);
  unsigned char emptyGrid[GRID_SIZE] = {0};
  addGridData(DATA_coverage, emptyGrid);
  addGridData(DATA_phi, emptyGrid);

  // Initialize current state to track between iterations

  struct DataGrid cur_coverage, cur_phi;
  float cur_bat[MAX_ROBOTS];
  Vec2d cur_loc[MAX_ROBOTS], cur_moment[MAX_ROBOTS];

  for (i = 0; i < GRID_SIZE; i++) {
    cur_coverage.data[i] = DATA_coverage->grids[0].data[i];
    cur_phi.data[i] = DATA_phi->grids[0].data[i];
  }

  for (i = 0; i < arena->n_robots; i++) {
    cur_bat[i] = arena->states[i]->battery[0];
    cur_loc[i] = arena->states[i]->loc[0];
    cur_moment[i] = arena->states[i]->moment[0];
  }

  // Initialize Centroid and Seeking utility arrays

  int weights_x[MAX_ROBOTS]   = {0};
  int weights_y[MAX_ROBOTS]   = {0};
  int cell_count[MAX_ROBOTS]  = {0};
  Vec2d centroids[MAX_ROBOTS] = {0};

  int interesting_cells;
  Vec2d cells_of_interest[GRID_SIZE] = {0};

  // Initialize Real Phi distribution
  struct DataGrid real_phi;

  int n_distributions = 3;
  Vec2d distributions[3] = {
    { 40, 80 },
    { 100,10 },
    { 33.1, 50 }
  };

  //  int n_distributions = 9;
  //  Vec2d distributions[9] = {
  //    { 3, 12 },
  //    { 30,30 },
  //    { 78, 125 },
  //    { 28, 50 },
  //    { 71, 50 },
  //    { 20, 90 },
  //    { 100, 45 },
  //    { 124, 109 },
  //    { 99, 8 }
  //  };

  for (i = 0; i < GRID_SIZE; i++) {
    int x = (i % CELLS_PER_ROW)*CELL_WIDTH + CELL_WIDTH/2;
    int y = (i / CELLS_PER_ROW)*CELL_WIDTH + CELL_WIDTH/2;

    Vec2d cell = { x, y };
    float distance = 0; 
    for (j = 0; j < n_distributions; j++) {
      float distFromDistribution = 800 / vectorDistance(cell,distributions[j]); 
      distance += clamp255(distFromDistribution);
      distance = clamp255(distance);
    }
    unsigned char phi = clamp255(distance);
    real_phi.data[i] = phi;
  }


  int n_iterations = 300;

  // Loop each iteration
  for (i = 0; i <= n_iterations; i++) {

    // Find cells of interest to explore (Seeking)

    interesting_cells = 0;

    for (j = 0; j < GRID_SIZE; j++) {
      if(cur_coverage.data[j] < 200) {
        Vec2d poi;
        poi.x = (j % CELLS_PER_ROW) * CELL_WIDTH + CELL_WIDTH/2;
        poi.y = (j / CELLS_PER_ROW) * CELL_WIDTH + CELL_WIDTH/2;
        cells_of_interest[interesting_cells] = poi;
        interesting_cells++;
      }
    }

    // Find center of mass for each robot (Centroid)

    for (j = 0; j < arena->n_robots; j++) {
      weights_x[j] = 0;
      weights_y[j] = 0;
      cell_count[j] = 0;
    }

    for (j = 0; j < CELLS_PER_ROW; j++) {
      for (k = 0; k < CELLS_PER_ROW; k++) {
        float closest_dist = 99999;
        int closest_dist_index = -1;
        Vec2d cell_center = { j * CELL_WIDTH + CELL_WIDTH/2, k * CELL_WIDTH + CELL_WIDTH/2 };
        for (l = 0; l < arena->n_robots; l++) {
          Vec2d robot_loc = cur_loc[l];
          float dist = vectorDistance(robot_loc,cell_center);
          if (dist < closest_dist){
            closest_dist = dist;
            closest_dist_index = l;
          }
        }
        weights_x[closest_dist_index] += cell_center.x;
        weights_y[closest_dist_index] += cell_center.y;
        cell_count[closest_dist_index] += 1;
      }
    }

    // Assign centroids (centers of mass)
    for (j = 0; j < arena->n_robots; j++) {
      if (cell_count[j] == 0) {
        centroids[j] = cur_loc[j];
      } else {
        centroids[j].x = (float)weights_x[j] / (float)cell_count[j];
        centroids[j].y = (float)weights_y[j] / (float)cell_count[j];
      }
    }

    struct RobotTimeSeries *robot;
    float battery_weight, coverage_weight, info_weight;
    Vec2d optimal_charger, centroid, poi;

    // Assign moment for each robot
    for (j = 0; j < arena->n_robots; j++) {

      robot = arena->states[j];
      optimal_charger = arena->chargers[j];
      centroid = centroids[j];
      Vec2d control_vec;

      // Find best cell to seek to (Seeking)
      poi = cells_of_interest[0];
      float poi_dist = vectorDistance(cur_loc[j],poi);
      for (k = 1; k < interesting_cells; k++) {
        Vec2d ppoi = cells_of_interest[k];
        float ppoi_dist = vectorDistance(cur_loc[j],ppoi);
        if(ppoi_dist < poi_dist) {
          poi_dist = ppoi_dist;
          poi = ppoi;
        }
        if(poi_dist < 1.5f) break;
      }

      // float cells_bias = (float) interesting_cells / (float)GRID_SIZE;
      // float battery_bias = (float) cur_bat / (float)max_battery;

      if (cur_bat[j] < 40) {
        battery_weight = 1;
        coverage_weight = 0;
        info_weight = 0;
      } else if (interesting_cells == 0) {
        battery_weight = 0.0;
        coverage_weight = 1-battery_weight;
        info_weight = 0;
      } else {
        battery_weight = 0;
        info_weight = 0.9;
        coverage_weight = 1-info_weight;
      }

      
      // battery_weight = 0.3;
      // coverage_weight = 0.0;
      // info_weight = 0.7;

      control_vec.x = (optimal_charger.x - cur_loc[j].x) * battery_weight;
      control_vec.y = (optimal_charger.y - cur_loc[j].y) * battery_weight;

      control_vec.x += (centroid.x - cur_loc[j].x) * coverage_weight;
      control_vec.y += (centroid.y - cur_loc[j].y) * coverage_weight;

      control_vec.x += (poi.x - cur_loc[j].x) * info_weight;
      control_vec.y += (poi.y - cur_loc[j].y) * info_weight;

      //control_vec.x = 1;
      //control_vec.y = 0;

      // Ensure barriers (no collisions)
      for(k = 0; k < arena->n_robots; k++) {
        if(k != j) {
          float barrier_dist = vectorDistance(cur_loc[j],cur_loc[k]);
          if(barrier_dist < 1.0f) {
            control_vec.x = -5 * cur_moment[j].x;
            control_vec.y = -5 * cur_moment[j].y;
          }
        }
      }

      normalizeVector(&control_vec);
      scaleVector(&control_vec,arena->robot_speed);

      if (roundf(cur_loc[j].x) == optimal_charger.x && roundf(cur_loc[j].y) == optimal_charger.y) {
        cur_bat[j] = arena->max_battery+1;
      }

      if (cur_bat[j] <= 0.0f) {
        control_vec.x = 0;
        control_vec.y = 0;
      }

      simple_splat(&cur_coverage,cur_loc[j],CELL_WIDTH,CELLS_PER_ROW);

      Vec2d nextLoc;
      nextLoc.x = cur_loc[j].x + control_vec.x;
      nextLoc.y = cur_loc[j].y + control_vec.y;
      validateLoc(&nextLoc,ARENA_WIDTH_IN_PIXELS);

      addRtsData(robot,cur_bat[j]-1,nextLoc,control_vec);

      cur_bat[j] = cur_bat[j]-1;
      cur_loc[j] = nextLoc;
      cur_moment[j] = control_vec;
    }
    addGridData(DATA_coverage, cur_coverage.data);
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
  initColor(&teal,200,255,240);
  initColor(&blue,0,150,255);
  initColor(&darkBlue,0,2,55);
  struct ColorVecGradient infoGradient;
  infoGradient.n_colors = 0;
  addColorToColorVecGradient(&infoGradient,&teal);
  addColorToColorVecGradient(&infoGradient,&blue);
  addColorToColorVecGradient(&infoGradient,&darkBlue);

  // Generate Images of each frame of simulation

  printf("\nStarting bmp generation loop\n\n");
  struct Canvas *cvs;


  for (i = 0; i < arena->states[0]->length; i++) {

     char filename[32];
     sprintf(filename,"out/00%d.bmp\0",i);
     if (i > 9) sprintf(filename,"out/0%d.bmp\0",i);
     if (i > 99) sprintf(filename,"out/%d.bmp\0",i);
     cvs = initCanvas(ARENA_WIDTH_IN_PIXELS,ARENA_WIDTH_IN_PIXELS,filename);

     for (j = 0; j < GRID_SIZE; j++) {
       int x_lvl = (j % CELLS_PER_ROW)*CELL_WIDTH;
       int y_lvl = (j / CELLS_PER_ROW)*CELL_WIDTH;

       unsigned char coverage_data = DATA_coverage->grids[i].data[j];
       unsigned char phi_data = real_phi.data[j];
       float degree = (float)phi_data/255.0f;
       struct ColorVec infoLvl = getColorFromGradient(&infoGradient,degree);
       float exploration_degree = (float)coverage_data/255;
       struct ColorVec displayLvl = combineColors(&infoLvl,&bg_clr,exploration_degree);

       drawRect(cvs,x_lvl,y_lvl,x_lvl+CELL_WIDTH-1,y_lvl+CELL_WIDTH-1,&displayLvl);
     }

     drawLine(cvs,0,0,0,cvs->height,&axis_clr);
     drawLine(cvs,0,cvs->height-1,cvs->width-1,cvs->height-1,&axis_clr);
     drawLine(cvs,cvs->width-1,cvs->height-1,cvs->width-1,0,&axis_clr);
     drawLine(cvs,cvs->width-1,0,0,0,&axis_clr);

    for (j = 0; j < arena->n_robots; j++) {
      float battery = arena->states[j]->battery[i];
      float degree = battery/(float)arena->max_battery;
      struct ColorVec battery_lvl = getColorFromGradient(&greenToRed,degree);
      graphRobotTimeSeriesFrame(cvs,arena->states[j],0,&battery_lvl,i);
    }

    for (j = 0; j < arena->n_chargers; j++) {
      Vec2d loc = arena->chargers[j];
      drawRect(cvs,loc.x,loc.y,loc.x,loc.y,&charger_clr);
    }
    generateBitmapImage(cvs);
  }

  // END
  destroyArena(arena);
  free(cvs);
  return 0;
  // END
}

