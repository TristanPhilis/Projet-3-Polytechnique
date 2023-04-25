#ifndef _RETURN_TO_BASE__
#define _RETURN_TO_BASE__
#include <stdbool.h>
#include "stabilizer_types.h"

// Return to base attributes
setpoint_t setpoint;
float currentAngleDeg;
float xFromBase;
float yFromBase;
float angleFromBase;
float angleToBase;
float angle_diff;
float distanceFromHome;
float speedReturnHome;
float directionYawReturnBase;

//////// Return to base functions ////////

/**
 * @brief Prepare velocity informations for movement.
 * Function taken from wall_following.c
 *
 * @param setpoint velocity point
 * @param vx speed in x 
 * @param vy speed in y
 * @param z height
 * @param yawrate z rotation speed (yaw)
 * @return void
 */
void setVelocitySetpoint(setpoint_t *setpoint, float vx, float vy, float z, float yawrate);

/**
 * @brief Return home init.
 *
 * @param void none
 * @return void
 */
void initReturnHome();

/**
 * @brief find angle to return home.
 *
 * @param void none
 * @return void
 */
void findAngle();

/**
 * @brief Find the shortest direction to turn to the return home angle. 
 * Not all case are covered, but most are. It saves some battery.
 *
 * @param angleToBase return home angle
 * @return void
 */
void findDirection(float angleToBase);

/**
 * @brief Drone turns around itself until it is in the return home angle.
 *
 * @param void none
 * @return void
 */
bool turnToAngle();

/**
 * @brief Drone go in a straight line to base unless it finds obstacles.
 *
 * @param void none
 * @return void
 */
void goStraightToBase();

/**
 * @brief Finds if it is in one meter from home.
 *
 * @param void none
 * @return void
 */
bool inOneMetterRadiusFromStartingPoint();

/**
 * @brief Verify if nothing is in front of the drone in a 0.5 meters range.
 *
 * @param void none
 * @return void
 */
bool wayAheadIsClean();

#endif