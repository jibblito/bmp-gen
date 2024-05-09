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

  // Initialize Task Locations
  
  /*
  int n_tasks = 3;
  Vec2d tasks_1[3] = {
     {ARENA_WIDTH_IN_PIXELS/2,ARENA_WIDTH_IN_PIXELS/4*3},
     {ARENA_WIDTH_IN_PIXELS/4*1,ARENA_WIDTH_IN_PIXELS/4*1},
     {ARENA_WIDTH_IN_PIXELS/4*3,ARENA_WIDTH_IN_PIXELS/4*1}
  };
  Vec2d tasks_2[3] = {
     {ARENA_WIDTH_IN_PIXELS/2,ARENA_WIDTH_IN_PIXELS/4*1},
     {ARENA_WIDTH_IN_PIXELS/2,ARENA_WIDTH_IN_PIXELS/4*2},
     {ARENA_WIDTH_IN_PIXELS/2,ARENA_WIDTH_IN_PIXELS/4*3}
  };
  float pi_desired[3] = {
    0.33, 0.33, 0.33
  };
  */

  /*
  int n_tasks = 2;
  Vec2d tasks_1[2] = {
     {ARENA_WIDTH_IN_PIXELS/2,ARENA_WIDTH_IN_PIXELS/3*1},
     {ARENA_WIDTH_IN_PIXELS/2,ARENA_WIDTH_IN_PIXELS/3*2}
  };
  Vec2d tasks_2[2] = {
     {ARENA_WIDTH_IN_PIXELS/3*1,ARENA_WIDTH_IN_PIXELS/2},
     {ARENA_WIDTH_IN_PIXELS/3*2,ARENA_WIDTH_IN_PIXELS/2}
  };
  float pi_desired[2] = {
    0.5,0.5
  };
  */

  int n_tasks = 4;
  Vec2d tasks_1[4] = {
     {ARENA_WIDTH_IN_PIXELS/4*1,ARENA_WIDTH_IN_PIXELS/4*1},
     {ARENA_WIDTH_IN_PIXELS/4*1,ARENA_WIDTH_IN_PIXELS/4*3},
     {ARENA_WIDTH_IN_PIXELS/4*3,ARENA_WIDTH_IN_PIXELS/4*1},
     {ARENA_WIDTH_IN_PIXELS/4*3,ARENA_WIDTH_IN_PIXELS/4*3}
  };
  Vec2d tasks_2[4] = {
     {ARENA_WIDTH_IN_PIXELS/2,ARENA_WIDTH_IN_PIXELS/5*1},
     {ARENA_WIDTH_IN_PIXELS/2,ARENA_WIDTH_IN_PIXELS/5*2},
     {ARENA_WIDTH_IN_PIXELS/2,ARENA_WIDTH_IN_PIXELS/5*3},
     {ARENA_WIDTH_IN_PIXELS/2,ARENA_WIDTH_IN_PIXELS/5*4}
  };
  float pi_desired[4] = {
    0.25,0.25,0.25,0.25
  };

  int n_robots = n_tasks;

  ARENA *arena = initArena(n_robots,ARENA_WIDTH_IN_PIXELS,1);

  float max_slack = 100, min_slack = 1;
  float slack_factor = 10;

  // Slack for all robots for all tasks;
  float slack[MAX_ROBOTS][MAX_TASKS] = { 0 };

  // Initialize tasks data series
  struct TaskTimeSeries *DATA_tasks = initTaskTimeSeries(NULL);
  int initial_tasks_status[MAX_TASKS] = { 0 };
  addTaskData(DATA_tasks,initial_tasks_status);

  // Set initial slack
  for (i = 0; i < arena->n_robots; i++) {
    for (j = 0; j < n_tasks; j++) {
      slack[i][j] = max_slack / 2;
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

  int n_iterations = 200;

  Vec2d *tasks = tasks_1;

  // Loop each iteration
  for (i = 0; i <= n_iterations; i++) {

    if(i == n_iterations/2) {
      tasks = tasks_2;
      for (j = 0; j < n_tasks; j++) {
        cur_tasks_status[j] = 0;
      }
    }

    int pi[MAX_TASKS];
    for (j = 0; j < MAX_TASKS; j++) pi[j] = -1;
    int alpha[MAX_ROBOTS] = {0};
    for (j = 0; j < MAX_ROBOTS; j++) alpha[j] = -1;

    // For each robot, adjust slack based on nearest tasks
    for (j = 0; j < arena->n_robots; j++) {
      float distance_robot_to_task;
      float taskDistances[MAX_TASKS] = {0};
      float max_dist = 0, min_dist = 99999;

      // Will add a metric to weights: the closer a robot is, the higher its weight (alpha)
      int closest_task_index = -1;
      for (k = 0; k < n_tasks; k++) {
        distance_robot_to_task = vectorDistance(&cur_loc[j],&tasks[k]);
        taskDistances[k] = distance_robot_to_task;
        if (distance_robot_to_task > max_dist) max_dist = distance_robot_to_task;
        if (distance_robot_to_task < min_dist) {
          min_dist = distance_robot_to_task;
          closest_task_index = k;
        }
      }

      alpha[j] = closest_task_index; // assign that task to the robot
      pi[closest_task_index] = j; // assign that robot to the task 
      float range = max_dist - min_dist;

      float taskDistanceRelativeWeights[MAX_TASKS] = {0};
      for (k = 0; k < n_tasks; k++) {
        float relativeWeight = (taskDistances[k] - min_dist) / range; // (0-1) := (shortest-longest)
        relativeWeight -= 0.1; // (-0.5,0.5) := (shortest,longest)
        taskDistanceRelativeWeights[k] = relativeWeight;
      }

      for (k = 0; k < n_tasks; k++) {
        slack[j][k] += slack_factor * taskDistanceRelativeWeights[k]; 
        if (slack[j][k] > max_slack) slack[j][k] = max_slack;
        if (slack[j][k] < min_slack) slack[j][k] = min_slack;
      }
    }

    // Correct for Pi (Make sure all tasks are assigned)
    float pi_actual[MAX_TASKS] = { 0 };
    for (j = 0; j < arena->n_robots; j++) {
      pi_actual[alpha[j]] += 1.0f/(float)arena->n_robots;
    }
    
    for (k = 0; k < n_tasks; k++) {
      if (pi_actual[k] > (pi_desired[k] + 0.05)) {
        while (pi_actual[k] > (pi_desired[k] + 0.05)) {
          float alpha_distances[MAX_ROBOTS];
          float furthest_robot_distance = 0;
          int furthest_robot_index = -1;
          for (j = 0; j < arena->n_robots; j++) {
            if (alpha[j] == k) {
              float distance = vectorDistance(&cur_loc[j],&tasks[k]);
              if (distance > furthest_robot_distance) {
                furthest_robot_distance = distance;
                furthest_robot_index = j;
              }
            }
          }
          alpha[furthest_robot_index] = -1;
          pi_actual[k] = pi_actual[k] - (1.0f/(float)arena->n_robots);
          slack[furthest_robot_index][k] = max_slack;
        }
      }
    }

    // For each robot, adjust slack based on empty jobs
    for (k = 0; k < n_tasks; k++) {
      for(j = 0; j < arena->n_robots; j++) {
        if (alpha[j] == -1) {
          if (pi[k] == -1) {
            slack[j][k] = min_slack;
          } else {
            slack[j][k] = max_slack;
          }
        }
      }
    }


    // probe potential moments to satisfy choosing u_i for moment
    float circleRadius = 3;
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
            float diff_x = potentialTarget.x - cur_loc[j].x;
            float diff_y = potentialTarget.y - cur_loc[j].y;
            diff_x = diff_x * distanceRatio;
            diff_y = diff_y * distanceRatio;
            potentialTarget.x = cur_loc[j].x + diff_x;
            potentialTarget.y =  cur_loc[j].y + diff_y;
          }
          if(distanceRatio * circleRadius < 1) {
            potentialTarget = cur_loc[j];
            cur_tasks_status[l] = 1;
          } 
          float distanceToTask = vectorDistance(&potentialTarget,&tasks[l]);
          float slackOfTask = slack[j][l];
          float summation = distanceToTask * (max_slack-slackOfTask);
          costSummationAllTasks += summation;
        }

        // Compare cost sum to lowest cost; if lower, assign that target to the robot
        if (costSummationAllTasks < lowestCost) {
          lowestCost = costSummationAllTasks;
          target = potentialTarget;
        }
      }

      struct RobotTimeSeries *robot = arena->states[j];
      
      Vec2d control_vec = { 0 };
      control_vec.x = target.x - cur_loc[j].x;
      control_vec.y = target.y - cur_loc[j].y;

      normalizeVector(&control_vec);
      scaleVector(&control_vec,arena->robot_speed);

      Vec2d nextLoc;
      nextLoc.x = cur_loc[j].x + control_vec.x;
      nextLoc.y = cur_loc[j].y + control_vec.y;
      validateLoc(&nextLoc,ARENA_WIDTH_IN_PIXELS);

      cur_loc[j] = nextLoc;
      cur_moment[j] = control_vec;

      addRtsData(robot,100,cur_loc[j],cur_moment[j]);
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

  tasks = tasks_1;
  for (i = 0; i < arena->states[0]->length; i++) {
    if(i == n_iterations/2) {
      tasks = tasks_2;
    }

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

