#ifndef ROBOTARIUM_H
#define ROBOTARIUM_H 

/**
 * Robotarium - Robots!
 *
 * Working on the formats, but they will be dope!
 */

#include "plots.h"

#define MAX_ROBOTS 10


typedef struct Arena {
  int n_robots, max_battery, n_chargers;
  float robot_speed, residual_battery_loss;
  struct RobotTimeSeries *states[MAX_ROBOTS];
  Vec2d chargers[MAX_ROBOTS];
} ARENA;

ARENA *initArena(int n_robots, int ROBOTARIUM_WIDTH, int mode);
void destroyArena(ARENA *a);

// Helper functions
void validateLoc(Vec2d *location, float width);
void normalizeVector(Vec2d *moment);
void scaleVector(Vec2d *moment, float scale);

#endif
