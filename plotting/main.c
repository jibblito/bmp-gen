#include "plots.h"
#include "../shapes.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

int main (int argc, char** argv) {
  
  // Limit of 10 for now
  int n_robots = 6;
  int max_battery = 100;
  int i,j,k,l;

  // Initialise pointers
  struct RobotTimeSeries *robotarium[10];
  struct Canvas *cvs = initCanvas(100,100,"robotarium.bmp");

  for (i = 0; i < n_robots; i++) {
    robotarium[i] = initRobotTimeSeries(NULL);
  }

  typedef struct location {
    float x,y;
  } LOC;

  // Prepare charger locations
  int n_chargers = n_robots;
  LOC *chargers = malloc(sizeof(LOC) * n_chargers);
  int frac = cvs->height/(n_chargers/2);
  int offset = frac/2;
  for (i = 0; i < 2; i++) {
    if (i%2==0) chargers[i].y = 0;
    else chargers[i].y = cvs->height-1;
    chargers[i].x = cvs->width/2;
  }
  for (;i<n_chargers;i++) {
    if (i%2==0) chargers[i].x = 0;
    else chargers[i].x = cvs->width-1;
    chargers[i].y = (i/2)*frac + offset;
  }


  // Prepare time series data sheets for robot locations
  for (i = 0; i < n_robots; i++) {
    addRtsData(robotarium[i],max_battery,chargers[i].x,chargers[i].y);
  }

  int n_iterations = 200;
  float robot_speed = 0.2;
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

      if (cur_bat < 20) {
        battery_weight = 1;
        coverage_weight = 0;
      } else {
        // battery_weight = ((float)cur_bat/(float)max_battery) * 0.5;
        // coverage_weight = 1-battery_weight;
        battery_weight = 0;
        coverage_weight = 1;
      }

      // float dist_to_charger_x = optimal_charger.x-cur_x;
      // float dist_to_charger_y = optimal_charger.x-cur_y;

      x_vec = (optimal_charger.x - cur_x) * battery_weight;
      y_vec = (optimal_charger.y - cur_y) * battery_weight;

      x_vec += (centroid.x - cur_x) * coverage_weight;
      y_vec += (centroid.y - cur_y) * coverage_weight;

      x_vec *= robot_speed;
      y_vec *= robot_speed;

      if (roundf(cur_x) == optimal_charger.x && roundf(cur_y) == optimal_charger.y) {
        printf("robot %d charged up!\n",j);
        cur_bat = max_battery+1;
      }

      addRtsData(robot,cur_bat-1,cur_x + x_vec,cur_y + y_vec);
    }
  }
  printf("Done generating data\n");


  struct ColorVec robot_colors[10]; 
  initColor(&robot_colors[0],255,0,0);      initColor(&robot_colors[1],255,255,0);
  initColor(&robot_colors[2],30,255,123);   initColor(&robot_colors[3],150,200,200);
  initColor(&robot_colors[4],66,59,201);    initColor(&robot_colors[5],200,100,110);
  initColor(&robot_colors[6],200,100,200);  initColor(&robot_colors[7],88,199,130);
  initColor(&robot_colors[8],80,120,101);   initColor(&robot_colors[9],255,100,100);

  struct ColorVec charger_clr,finish_clr;
  initColor(&charger_clr,230,210,10);
  initColor(&finish_clr,70,50,130);

  // Generate frames of animation
  for (i = 0; i < n_robots; i++) {
    if (i == 0) {
      graphRobotTimeSeries(cvs,robotarium[i],1,&robot_colors[i]);
    } else {
      graphRobotTimeSeries(cvs,robotarium[i],0,&robot_colors[i]);
    }
    etchCircle(cvs,robotarium[i]->x[0],robotarium[i]->y[0],3,&charger_clr);
    etchCircle(cvs,robotarium[i]->x[robotarium[0]->length-1],robotarium[i]->y[robotarium[0]->length-1],3,&finish_clr);
  }
  for (i = 0; i < n_chargers; i++) {
    LOC l = chargers[i];
    drawRect(cvs,l.x-1,l.y-1,l.x+1,l.y+1,&charger_clr);
  }
  generateBitmapImage(cvs);

//  // END EARLY
//   for (i = 0; i < n_robots; i++) {
//     free(robotarium[i]);
//   }
//   free(cvs);
//   free(chargers);
//   return 0;
//   // END EARLY


  printf("\nStarting bmp generation loop\n\n");
  struct ColorVec bg_clr,axis_clr;
  initColor(&bg_clr,255,255,255);
  initColor(&axis_clr,0,0,0);

  struct ColorVec green,red;
  initColor(&green,0,255,0);
  initColor(&red,255,0,0);
  struct ColorVecGradient greenToRed;
  greenToRed.n_colors = 0;
  addColorToColorVecGradient(&greenToRed,&green);
  addColorToColorVecGradient(&greenToRed,&red);

  struct ColorVec teal,blue,darkBlue;
  initColor(&teal,0,255,240);
  initColor(&blue,0,3,255);
  initColor(&darkBlue,0,2,55);
  struct ColorVecGradient infoGradient;
  infoGradient.n_colors = 0;
  addColorToColorVecGradient(&infoGradient,&teal);
  addColorToColorVecGradient(&infoGradient,&blue);
  addColorToColorVecGradient(&infoGradient,&darkBlue);
  struct ColorVecGradient yelRedBlu;
  yelRedBlu.n_colors = 0;
  addColorToColorVecGradient(&yelRedBlu,&red);
  addColorToColorVecGradient(&yelRedBlu,&blue);

  for (i = 0; i < robotarium[0]->length; i++) {

     char filename[32];
     sprintf(filename,"out/00%d.bmp\0",i);
     if (i > 9) sprintf(filename,"out/0%d.bmp\0",i);
     if (i > 99) sprintf(filename,"out/%d.bmp\0",i);
     cvs = initCanvas(100,100,filename);

     struct ColorVec time_passage = getColorFromGradient(&infoGradient,(float)i/(float)n_iterations);

//     drawRect(cvs,0,0,cvs->width-1,cvs->height-1,&bg_clr);
     drawRect(cvs,0,0,cvs->width-1,cvs->height-1,&time_passage);
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
      drawRect(cvs,l.x-1,l.y-1,l.x+1,l.y+1,&charger_clr);
    }
    generateBitmapImage(cvs);
  }

  // ENDY EARLY
  for (i = 0; i < n_robots; i++) {
    free(robotarium[i]);
  }
  free(cvs);
  free(chargers);
  return 0;
  // ENDY EARLY
}
