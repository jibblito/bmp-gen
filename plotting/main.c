#include "plots.h"
#include "../shapes.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

int main (int argc, char** argv) {
  // if (argc < 2) {
  //   printf("Provide an argument!\n");
  //   return 22;
  // }
  
  // Limit of 10 for now
  int n_robots = 3;
  int max_battery = 50;
  int i,j,k,l;

  // Initialise pointers
  struct RobotTimeSeries **robotarium = malloc(sizeof(int *) * 10);
  struct Canvas *cvs = initCanvas(100,100,"robotarium.bmp");

  for (i = 0; i < n_robots; i++) {
    robotarium[i] = initRobotTimeSeries(NULL);
    printf("Len[%d]: %d\n",i,robotarium[i]->length);
  }

  typedef struct location {
    float x,y;
  } LOC;


  // Prepare charger locations
  int n_chargers = 4;
  LOC *chargers = malloc(sizeof(LOC) * n_chargers);
  int frac = cvs->height/n_chargers;
  int offset = frac/2;
  for (i = 0; i < n_chargers; i++) {
    chargers[i].x = 0;
    chargers[i].y = i*frac + offset;
  }


  // Prepare time series data sheets for robot locations
  for (i = 0; i < n_robots; i++) {
    addRtsData(robotarium[i],max_battery,rand() % cvs->width, rand() % cvs->width);
  }

  int n_iterations = 100;
  float robot_speed = 0.1;
  float resolution = 0.1;
  int weights_x[10] = {0};
  int weights_y[10] = {0};
  int cell_count[10] = {0};
  LOC centroids[10];

  // Loop: Find centroids for each robot, the apply vector. Generate time series
  for (i = 0; i <= n_iterations; i++) {
    printf("=============\nITERATION %d\n==============\n\n",i);

    for (j = 0; j < n_robots; j++) {
      weights_x[j] = 0;
      weights_y[j] = 0;
      cell_count[j] = 0;
    }
    
    printf("bout to iterate cells!\n");
    // Iterate cells of resolution
    for (j = 1; j < cvs->width-2; j = j + ((float)cvs->width)*resolution) {
      for (k = 1; k < cvs->width-2; k = k + ((float)cvs->width)*resolution) {
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
      centroids[j].x = (float)weights_x[j] / (float)cell_count[j];
      centroids[j].y = (float)weights_y[j] / (float)cell_count[j];
    }

    // Control Loop
    for (j = 0; j < n_robots; j++) {
      float x_vec,y_vec;

      struct RobotTimeSeries *robot = robotarium[j];
      LOC optimal_charger = chargers[j%n_chargers];
      LOC centroid = centroids[j];

      float cur_x = robot->x[(robot->length)-1];
      float cur_y = robot->y[(robot->length)-1];
      float cur_bat = robot->battery[(robot->length)-1];
    
      float battery_weight, coverage_weight;

      if (cur_bat < 10) {
        battery_weight = 1;
        coverage_weight = 0;
      } else {
        battery_weight = ((float)cur_bat/(float)max_battery) * 0.5;
        coverage_weight = 1-battery_weight;
      }
      battery_weight = 0;
      coverage_weight = 1;

      x_vec = (optimal_charger.x - cur_x) * battery_weight;
      y_vec = (optimal_charger.y - cur_y) * battery_weight;

      x_vec += (centroid.x - cur_x) * coverage_weight;
      y_vec += (centroid.y - cur_y) * coverage_weight;

      x_vec *= robot_speed;
      y_vec *= robot_speed;

      printf("Robo_loc[%d] x: %3.3f, y: %3.3f\n",j, cur_x,cur_y);
      printf("Robo_vec[%d] x: %3.3f, y: %3.3f\n",j, x_vec,y_vec);
      printf("Robo[%d] centroid: (%3.3f,%3.3f)\n",j,centroids[j].x,centroids[j].y);

      addRtsData(robot,cur_bat-1,cur_x + x_vec,cur_y + y_vec);
    }
  }
  printf("Done generating data\n");


  struct ColorVec *robot_colors[10] = 
    {
      initColor(255,0,0), initColor(255,255,0),
      initColor(30,255,123), initColor(150,200,200),
      initColor(66,59,201), initColor(200,100,110),
      initColor(200,100,200), initColor(88,199,130),
      initColor(80,120,101), initColor(255,100,100),
    };

  struct ColorVec *charger_clr = initColor(230,210,10);


  printf("\bGO 1\n\n");
  for (i = 0; i < robotarium[0]->length; i++) {
    for (j = 0; j < n_robots; j++) {

      printf("(%f,%f)\n",robotarium[j]->x[i],robotarium[j]->y[i]);
    }
  }


  struct ColorVec *finish_clr = initColor(70,50,130);

  // Generate frames of animation
  for (i = 0; i < n_robots; i++) {
    if (i == 0) {
      graphRobotTimeSeries(cvs,robotarium[i],1,robot_colors[i]);
    } else {
      graphRobotTimeSeries(cvs,robotarium[i],0,robot_colors[i]);
    }
    etchCircle(cvs,robotarium[i]->x[0],robotarium[i]->y[0],3,charger_clr);
    etchCircle(cvs,robotarium[i]->x[robotarium[0]->length-1],robotarium[i]->y[robotarium[0]->length-1],3,finish_clr);
  }
  for (i = 0; i < n_chargers; i++) {
    LOC l = chargers[i];
    // drawRect(cvs,l.x-1,l.y-1,l.x+1,l.y+1,charger_clr);
  }
  generateBitmapImage(cvs);

  return 0;



  printf("\nStarting bmp generation loop\n\n");
  struct ColorVec *bg_clr = initColor(255,255,255);
  struct ColorVec *axis_clr = initColor(0,0,0);

  for (i = 0; i < robotarium[0]->length; i++) {

     char filename[32];
     sprintf(filename,"out/00%d.bmp\0",i);
     if (i > 9) sprintf(filename,"out/0%d.bmp\0",i);
     if (i > 99) sprintf(filename,"out/%d.bmp\0",i);
     cvs = initCanvas(100,100,filename);

     drawRect(cvs,0,0,cvs->width-1,cvs->height-1,bg_clr);
     drawLine(cvs,0,0,0,cvs->height,axis_clr);
     drawLine(cvs,0,cvs->height-1,cvs->width-1,cvs->height-1,axis_clr);
     drawLine(cvs,cvs->width-1,cvs->height-1,cvs->width-1,0,axis_clr);
     drawLine(cvs,cvs->width-1,0,0,0,axis_clr);

    for (j = 0; j < n_robots; j++) {
      graphRobotTimeSeriesFrame(cvs,robotarium[j],0,robot_colors[j],i);
    }

    for (j = 0; j < n_chargers; j++) {
      LOC l = chargers[j];
      //drawRect(cvs,l.x-1,l.y-1,l.x+1,l.y+1,charger_clr);
    }

    // generateBitmapImage(cvs);
  }

  printf("\bGO 3\n\n");
  for (i = 0; i < robotarium[0]->length; i++) {
    for (j = 0; j < n_robots; j++) {
      printf("(%f,%f)\n",robotarium[j]->x[i],robotarium[j]->y[i]);
    }
  }

  for (i = 0; i < n_robots; i++) {
    free(robotarium[i]);
  }

  free(cvs);
}
