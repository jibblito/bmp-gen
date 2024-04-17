/**
 * Implementation of Robotarium.h
 *
 * Takes care of robots: Location, Time series data,
 * physical constants governing robotiq behaviour.
 *
 * Also manages charger locations, for sake of simplicity
 */

#include "robotarium.h"
#include <math.h>
#include <stdlib.h>

ARENA *initArena(int n_robots, int ARENA_WIDTH_IN_PIXELS) {
  ARENA *a = malloc(sizeof(ARENA));

  // Customize Arena conditions here
  a->n_robots = n_robots;
  a->max_battery = 100;
  a->n_chargers = n_robots;
  a->robot_speed = 3.5;

  int i;
  // Place chargers in the arena
  int frac = ARENA_WIDTH_IN_PIXELS / a->n_chargers;
  int offset = frac / 2;
  for (i = 0; i < 3; i++) {
    a->chargers[i].x = 0;
    a->chargers[i].y = i*frac*2 + offset*2;
  }
  for (i = 0; i < 3; i++) {
    a->chargers[i+3].x = ARENA_WIDTH_IN_PIXELS-1;
    a->chargers[i+3].y = i*frac*2 + offset*2;
  }

  // Initialize RobotTimeSeries: robots start at chargers
  for (i = 0; i < a->n_robots; i++) {
    a->states[i] = initRobotTimeSeries(NULL);
    Vec2d noMovement = { 0,0 };
    addRtsData(a->states[i],a->max_battery,a->chargers[i],noMovement);
  }

  return a;
}

void destroyArena(ARENA *a) {
  int i;
  for (i = 0; i < a->n_robots; i++) {
    free(a->states[i]);
  }
  free(a);
}


void validateLoc(Vec2d *location, float width) {
  if(location->x < 0) location->x = 0;
  if(location->x >= width) location->x = width-1;
  if(location->y < 0) location->y = 0;
  if(location->y >= width) location->y = width-1;
}

void normalizeVector(Vec2d *moment) {
  float vecScale = sqrt(pow(moment->x,2)+pow(moment->y,2));
  if (vecScale < 0.2f) {
    moment->x = 0;
    moment->y = 0;
  } else if (vecScale < 2.5f) {
    moment->x = moment->x/2;
    moment->y = moment->y/2;
  } else {
    float normal_ratio = 1/vecScale;
    moment->x = moment->x * normal_ratio;
    moment->y = moment->y * normal_ratio;
  }
}

void scaleVector(Vec2d *moment, float scale) {
  moment->x = moment->x * scale;
  moment->y = moment->y * scale;
}




