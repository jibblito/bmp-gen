#include "plots.h"
#include "robotarium.h"
#include "../shapes.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#define ALPHA_RESOLUTION 10

int main (int argc, char** argv) {

  int i,j,k,l;

  int ARENA_WIDTH_IN_PIXELS = 150;
  ARENA_WIDTH_IN_PIXELS = 150;

  int n_robots = 3;
  ARENA *arena = initArena(n_robots,ARENA_WIDTH_IN_PIXELS,1);

  float K = 10;
  float max_slack = 10, min_slack = 0;
  float slack_increment = 0.2;

  float alpha[ALPHA_RESOLUTION][MAX_ROBOTS][MAX_TASKS] = { 0 };

  // Slack for all robots for all tasks;
  float slack[MAX_ROBOTS][MAX_TASKS] = { 0 };

  // Initialize tasks data series
  struct TaskTimeSeries *DATA_tasks = initTaskTimeSeries(NULL);
  int initial_tasks_status[MAX_TASKS] = { 0 };
  addTaskData(DATA_tasks,initial_tasks_status);

  // Initialize Task Locations

  
  int n_tasks = 3;
  Vec2d tasks[3] = {
     {ARENA_WIDTH_IN_PIXELS/2,ARENA_WIDTH_IN_PIXELS/4*3},
     {ARENA_WIDTH_IN_PIXELS/4*1,ARENA_WIDTH_IN_PIXELS/4*1},
     {ARENA_WIDTH_IN_PIXELS/4*3,ARENA_WIDTH_IN_PIXELS/4*1}
  };
  float pi_desired[3] = {
    0.33, 0.33, 0.33
  };
  

  /*
  int n_tasks = 15;
  Vec2d tasks[15] = {
     {ARENA_WIDTH_IN_PIXELS/6*1,ARENA_WIDTH_IN_PIXELS/4*1},
     {ARENA_WIDTH_IN_PIXELS/6*2,ARENA_WIDTH_IN_PIXELS/4*1},
     {ARENA_WIDTH_IN_PIXELS/6*3,ARENA_WIDTH_IN_PIXELS/4*1},
     {ARENA_WIDTH_IN_PIXELS/6*4,ARENA_WIDTH_IN_PIXELS/4*1},
     {ARENA_WIDTH_IN_PIXELS/6*5,ARENA_WIDTH_IN_PIXELS/4*1},
     {ARENA_WIDTH_IN_PIXELS/6*1,ARENA_WIDTH_IN_PIXELS/4*2},
     {ARENA_WIDTH_IN_PIXELS/6*2,ARENA_WIDTH_IN_PIXELS/4*2},
     {ARENA_WIDTH_IN_PIXELS/6*3,ARENA_WIDTH_IN_PIXELS/4*2},
     {ARENA_WIDTH_IN_PIXELS/6*4,ARENA_WIDTH_IN_PIXELS/4*2},
     {ARENA_WIDTH_IN_PIXELS/6*5,ARENA_WIDTH_IN_PIXELS/4*2},
     {ARENA_WIDTH_IN_PIXELS/6*1,ARENA_WIDTH_IN_PIXELS/4*3},
     {ARENA_WIDTH_IN_PIXELS/6*2,ARENA_WIDTH_IN_PIXELS/4*3},
     {ARENA_WIDTH_IN_PIXELS/6*3,ARENA_WIDTH_IN_PIXELS/4*3},
     {ARENA_WIDTH_IN_PIXELS/6*4,ARENA_WIDTH_IN_PIXELS/4*3},
     {ARENA_WIDTH_IN_PIXELS/6*5,ARENA_WIDTH_IN_PIXELS/4*3},
  };
  float pi_desired[15] = {
    0.1, 0.1, 0.1, 0.1, 0.1,
    0.5, 0.5, 0.5, 0.5, 0.5,
    0.5, 0.5, 0.5, 0.5, 0.5
  };
  */



  // Set initial slack
  for (i = 0; i < arena->n_robots; i++) {
    for (j = 0; j < n_tasks; j++) {
      slack[i][j] = 5;
    }
  }

  Vec2d cur_loc[MAX_ROBOTS], cur_moment[MAX_ROBOTS];
  int cur_tasks_status[MAX_TASKS];

  for (i = 0; i < arena->n_robots; i++) {
    cur_loc[i] = arena->states[i]->loc[0];
    cur_moment[i] = arena->states[i]->moment[0];
  }

  for (i = 0; i < n_tasks; i++) {
    cur_tasks_status[i] = DATA_tasks->states[0][i];
  }

  int n_iterations = 100;

  // Loop each iteration
  for (i = 0; i <= n_iterations; i++) {

    struct RobotTimeSeries *robot;

    // For each task, adjust slack based on nearest robots
    for (j = 0; j < n_tasks; j++) {
      int tokens = roundf(pi_desired[j] * (float)n_robots);    
      int robotsSelected[MAX_ROBOTS] = {0};
      while (tokens > 0) {
        float closestDist = 99999, potentialDist;
        int selectedIndex = 0;
        for (k = 0; k < arena->n_robots; k++) {
          if (robotsSelected[k] == 1) continue;
          potentialDist = vectorDistance(&cur_loc[k],&tasks[j]);
          if (potentialDist < closestDist) {
            closestDist = potentialDist;
            selectedIndex = k;
          }
        }
        robotsSelected[selectedIndex] = 1; // Mark robot as selected
        tokens--;
      }
      for (k = 0; k < arena->n_robots; k++) {
        if (robotsSelected[k] == 1) {
          if (slack[k][j] > 0) {
            slack[k][j] -= slack_increment; // Decrease slack on selected robot
          }
        } else if (robotsSelected[k] == 0) {
          if (slack[k][j] < max_slack) {
            slack[k][j] += slack_increment; // Increase slack on unselected robot
          }
        }
      }
    }

    printf("Slack Time!");

    float circleRadius = 5;
    int circleResolution = 12;
    for (j = 0; j < arena->n_robots; j++) {
      Vec2d target = cur_loc[j];
      float lowestCost = 99999;
      for (k = 0; k < circleResolution; k++) { // Probe in a circle around the robot for targets
        float theta = ((float)k / (float)circleResolution) * 6.282f;
        Vec2d potentialTarget = cur_loc[j];
        potentialTarget.x = potentialTarget.x + circleRadius*cosf(theta); 
        potentialTarget.y = potentialTarget.y + circleRadius*sinf(theta); 
        
        // Sum cost to get to all tasks
        float costSummationAllTasks = 0;
        for (l = 0; l < n_tasks; l++) {
          float distanceRatio = vectorDistance(&cur_loc[j],&tasks[l]) / circleRadius;
          if (distanceRatio < 1) {
            scaleVector(&potentialTarget,distanceRatio); // Scale down so as to not miss task
          }
          costSummationAllTasks += vectorDistance(&potentialTarget,&tasks[l]);
          costSummationAllTasks += slack[j][l];
        }

        // Compare cost sum to lowest cost; if lower, assign that target to the robot
        if (costSummationAllTasks < lowestCost) {
          lowestCost = costSummationAllTasks;
          target = potentialTarget;
        }

        Vec2d control_vec;
        normalizeVector(&target);
        scaleVector(&target,arena->robot_speed);

        robot = arena->states[j];

        Vec2d nextLoc;

        nextLoc.x = cur_loc[j].x + target.x;
        nextLoc.y = cur_loc[j].y + target.y;
        validateLoc(&nextLoc,ARENA_WIDTH_IN_PIXELS);

        cur_loc[j] = nextLoc;
        cur_moment[j] = target;

        addRtsData(robot,100,cur_loc[j],cur_moment[j]);
      }
    }

    addTaskData(DATA_tasks,cur_tasks_status);
  }

  // Initialize COLORS

  struct ColorVec bg_clr,axis_clr, charger_clr;
  initColor(&bg_clr,255,255,255);
  initColor(&axis_clr,0,0,0);
  initColor(&charger_clr,230,210,10);

  struct ColorVec green,red,yellow;
  initColor(&green,0,255,0);
  initColor(&red,255,0,0);
  initColor(&yellow,255,255,0);

  struct ColorVec teal,blue,darkBlue;
  initColor(&teal,200,255,240);
  initColor(&blue,0,150,255);
  initColor(&darkBlue,0,2,55);

  // Generate Images of each frame of simulation

  printf("\nStarting bmp generation loop\n\n");
  struct Canvas *cvs;

  for (i = 0; i < arena->states[0]->length; i++) {

     char filename[32];
     sprintf(filename,"out/00%d.bmp\0",i);
     if (i > 9) sprintf(filename,"out/0%d.bmp\0",i);
     if (i > 99) sprintf(filename,"out/%d.bmp\0",i);
     cvs = initCanvas(ARENA_WIDTH_IN_PIXELS,ARENA_WIDTH_IN_PIXELS,filename);

     drawRect(cvs,0,0,ARENA_WIDTH_IN_PIXELS,ARENA_WIDTH_IN_PIXELS,&yellow);
     drawLine(cvs,0,0,0,cvs->height,&axis_clr);
     drawLine(cvs,0,cvs->height-1,cvs->width-1,cvs->height-1,&axis_clr);
     drawLine(cvs,cvs->width-1,cvs->height-1,cvs->width-1,0,&axis_clr);
     drawLine(cvs,cvs->width-1,0,0,0,&axis_clr);

    for (j = 0; j < arena->n_robots; j++) {
      graphRobotTimeSeriesFrame(cvs,arena->states[j],0,&darkBlue,i);
    }

    for (j = 0; j < n_tasks; j++) {
      Vec2d loc = tasks[j];
      if (DATA_tasks->states[i][j] == 0) {
        drawRect(cvs,loc.x-1,loc.y-1,loc.x+1,loc.y+1,&red);
      } else {
        drawRect(cvs,loc.x-1,loc.y-1,loc.x+1,loc.y+1,&green);
      }
    }
    generateBitmapImage(cvs);
  }

  // Gather Data!

  printf("Gathering Data from simulation for %d iterations...\n",n_iterations);

  FILE* csvFile = fopen("Simulation_Stats.csv", "wb");
  char buf[128] = { 0 };
  sprintf(buf,"LocationalCost,MapUncovered,MeanEnergy,CumulativeDistance\n");
  fwrite(buf, 1, strlen(buf), csvFile);
  memset(buf,'\0',128);

  /*
  float cumulativeDistance = 0;

  for (i = 0; i < arena->states[0]->length; i++) {

    // Locational Cost: Distance from each distribution to nearest robot
    float locationalCost = 0;
    for (j = 0; j < n_distributions; j++) {
      float shortest_distance = vectorDistance(&distributions[j],&arena->states[0]->loc[i]);
      for (k = 1; k < arena->n_robots; k++) {
        float robot_distance = vectorDistance(&distributions[j],&arena->states[k]->loc[i]);
        if (robot_distance < shortest_distance) shortest_distance = robot_distance;
      }
      locationalCost += shortest_distance;
    }

    // MapUncovered: Percentage of map uncovered
    float explorationCost = 0;
    for (j = 0; j < GRID_SIZE; j++) {
      unsigned char coverage_data = DATA_coverage->grids[i].data[j];
      float exploration_degree = 1 - (float)coverage_data/255;
      explorationCost += exploration_degree;
    }
    explorationCost = explorationCost / (float)GRID_SIZE;
    explorationCost = explorationCost * 100.0f;

    // Energy Mean: Mean energy level of all robots

    float energyMean = 0;
    float energySum = 0;
    for (j = 0; j < arena->n_robots; j++) {
      float battery = arena->states[j]->battery[i];
      energySum += battery;
    }
    energyMean = energySum / (float)arena->n_robots;

    // Cumulative Distance: Distance traveled by all robots
    float distanceCost = 0;
    for (j = 0; j < arena->n_robots; j++) {
      float distanceCovered = vectorScale(&arena->states[j]->moment[i]);
      cumulativeDistance += distanceCovered;
    }

    sprintf(buf,"%3.3f,%3.3f,%3.3f,%3.3f\n",locationalCost,explorationCost,energyMean,cumulativeDistance);
    fwrite(buf, 1, strlen(buf), csvFile);
  }
  */
  fclose(csvFile);
  printf("simulation data written to Simulation_Stats.csv\n");

  // END
  destroyArena(arena);
  free(cvs);
  return 0;
  // END
}

