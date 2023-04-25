#include "../headers/return_to_base.h"
#include "../headers/information.h"


#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "app.h"

#include "commander.h"

#include "FreeRTOS.h"
#include "task.h"

#include "debug.h"

#include "log.h"
#include "param.h"
#include <math.h>
#include "usec_time.h"

#include "../../src/modules/interface/estimator_kalman.h"

// const variables directly declared and defined here.
const float PI = 3.141592654f;
const float angleToStartingPointDeg = 45.0f;
const float spHeight = 0.3f;

void setVelocitySetpoint(setpoint_t *setpoint, float vx, float vy, float z, float yawrate)
{
  setpoint->mode.z = modeAbs;
  setpoint->position.z = z;
  setpoint->mode.yaw = modeVelocity;
  setpoint->attitudeRate.yaw = yawrate;
  setpoint->mode.x = modeVelocity;
  setpoint->mode.y = modeVelocity;
  setpoint->velocity.x = vx;
  setpoint->velocity.y = vy;

  setpoint->velocity_body = true;
}

void initReturnHome()
{
  speedReturnHome = 0.5f;
  directionYawReturnBase = 1.0f;
}

void findAngle()
{
  xFromBase = getStateX();
  // float xFromBase = (float)logGetUint(logIdKalmanStateX);
  yFromBase = getStateY();

  angleFromBase = (float)atan2((double)yFromBase, (double)xFromBase);
  angleFromBase = angleFromBase * 180.0f/PI;
  

  if(angleFromBase <= 0.0f)
    angleToBase = angleFromBase + 180.0f;
  else
    angleToBase = angleFromBase - 180.0f;
  
  // The direction is for the yaw rotation speed. It is not perfect, but it should
  // point most of the time in the good direction
  findDirection(angleToBase);
}

void findDirection(float angleToBase)
{
  currentAngleDeg = getyaw();
  
  // Not all cases are covered, but most are.
  // Otherwise, the logic complexity would be too high.
  // case 1
  if (angleToBase < 0.0f && currentAngleDeg > 0.0f)
  {
    if ((float)fabs((double)currentAngleDeg) > 90.0f && (float)fabs((double)angleToBase) > 90.0f) directionYawReturnBase = 1.0f;
    else if ((float)fabs((double)currentAngleDeg) < 90.0f && (float)fabs((double)angleToBase) < 90.0f) directionYawReturnBase = -1.0f;
    // others, arbitrarely positives angle
    else directionYawReturnBase = 1.0f;

  }
  // case 2
  else if (angleToBase > 0.0f && currentAngleDeg < 0.0f)
  {
    if ((float)fabs((double)currentAngleDeg) > 90.0f && (float)fabs((double)angleToBase) > 90.0f) directionYawReturnBase = -1.0f;
    else if ((float)fabs((double)currentAngleDeg) < 90.0f && (float)fabs((double)angleToBase) < 90.0f) directionYawReturnBase = 1.0f;
    // others, arbitrarely positives angle
    else directionYawReturnBase = 1.0f;
  }

  // Case that both angle are positives.
  else if (angleToBase < 0.0f && currentAngleDeg < 0.0f) 
  {
    if ((float)fabs(currentAngleDeg) < (float)fabs(angleToBase)) directionYawReturnBase = -1.0f;
    else directionYawReturnBase = 1.0f;
  }
  // Case that both angle are negatives.
  else if (angleToBase > 0.0f && currentAngleDeg > 0.0f)
  {
    if (currentAngleDeg < angleToBase) directionYawReturnBase = 1.0f;
    else directionYawReturnBase = -1.0f;
  }
  // ALL others, arbitrarely positives angle
  else directionYawReturnBase = 1.0f;

  
}

bool turnToAngle() {
  currentAngleDeg = getyaw();
  angle_diff = (float)fabs((double)(angleToBase - currentAngleDeg));
  // angle_diff = (float)fabs((double)(currentAngleDeg - angleToBase));

  if (angle_diff <= 1.0f)
  {
    setVelocitySetpoint(&setpoint, 0.0f,  0.0f, spHeight, 0.0f);
    commanderSetSetpoint(&setpoint, 3);
    return true;

  }
  else 
  {
    setVelocitySetpoint(&setpoint, 0.0f,  0.0f, spHeight, angleToStartingPointDeg * directionYawReturnBase);
    commanderSetSetpoint(&setpoint, 3);
    return false;
  }
}

void goStraightToBase()
{

  setVelocitySetpoint(&setpoint, speedReturnHome, 0.0f, spHeight, 0.0f);
  commanderSetSetpoint(&setpoint, 3);

}

bool inOneMetterRadiusFromStartingPoint()
{
  xFromBase = getStateX();
  yFromBase = getStateY();
  distanceFromHome = hypotf(xFromBase, yFromBase); // Hypotenuse
  return (distanceFromHome <= 0.5f);
}

bool wayAheadIsClean()
{
  xFromBase = getStateX();
  yFromBase = getStateY();
  distanceFromHome = hypotf(xFromBase, yFromBase); // Hypotenuse
  float frontRange = getFrontDistance() / 1000.0f;
  return frontRange >= 0.5f;
}
