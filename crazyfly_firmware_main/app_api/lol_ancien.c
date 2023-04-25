#include "../headers/information.h"
#include "../headers/main_commands.h"
#include "../headers/wall_following_projet3.h"
#include "../headers/return_to_base.h"

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"

#include "debug.h"

#include "ledseq.h"
#include "commander.h"
#include "crtp_commander_high_level.h"
#include "locodeck.h"
#include "mem.h"
#include "log.h"
#include "param.h"
#include "pm.h"
#include "app_channel.h"
#include "system.h"
#include "../../vendor/FreeRTOS/include/FreeRTOS.h"
#include "../../vendor/FreeRTOS/include/timers.h"

// Log and param ids
static logVarId_t logIdStateEstimateX;
static logVarId_t logIdStateEstimateY;
static logVarId_t logIdStateEstimateZ;
static paramVarId_t paramIdCommanderEnHighLevel;
static float TAKE_OFF_HEIGHT = 0.5f;

static float padX = 0.0;
static float padY = 0.0;
static float padZ = 0.0;

static float getX() { return logGetFloat(logIdStateEstimateX); }
static float getY() { return logGetFloat(logIdStateEstimateY); }
static float getZ() { return logGetFloat(logIdStateEstimateZ); }

static void enableHighlevelCommander() { paramSetInt(paramIdCommanderEnHighLevel, 1); }

// Return to base parameters
// Counter and millisecond to ticks function, both for the timer.
TimerHandle_t AnglerTimer;
#ifndef pdMS_TO_TICKS
    #define pdMS_TO_TICKS( xTimeInMs )    ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000U ) )
#endif
enum state_return_base stateForReturnToBase = FINDING_ANGLE;

// We made a simple takeOff for demonstration. The real one (for exploration) is implemented
// in the wall_following function.
void takeOff()
{
  logIdStateEstimateX = logGetVarId("stateEstimate", "x");
  logIdStateEstimateY = logGetVarId("stateEstimate", "y");
  logIdStateEstimateZ = logGetVarId("stateEstimate", "z");
  paramIdCommanderEnHighLevel = paramGetVarId("commander", "enHighLevel");
  enableHighlevelCommander();

  padX = getX();
  padY = getY();
  padZ = getZ();
  crtpCommanderHighLevelTakeoff(padZ + TAKE_OFF_HEIGHT, 1.0f);
  setState(STATE_HOVERING);
}

void landing()
{
  // The landing is implemented in the wall_following of our project3 to reuse the 
  // initialized commander.
  endExploration();
  setState(STATE_IDLE);
}

void exploration() 
{
  followTheWall();
}

void setStateReturnHome()
{
  stateForReturnToBase = VERIFY_DISTANCE_TO_BASE;
}
bool turnAround = true;
bool goFoward = false;
void returnToBase()
{
  // Return home has its home state machine
  switch (stateForReturnToBase)
  {
  case VERIFY_DISTANCE_TO_BASE:
    verifyDistanceToBase();
    break;
  case FINDING_ANGLE:
    /* code */
    findAngle();
    turnAround = false;
    stateForReturnToBase = TURN_TO_ANGLE;
    break;
  case TURN_TO_ANGLE:
    turnAround = turnToAngle();
    if(turnAround)
      stateForReturnToBase = VERIFY_WAY_AHEAD;
    break;

  case VERIFY_WAY_AHEAD:
    goFoward = wayAheadIsClean();
    if(goFoward)
      stateForReturnToBase = GO_STRAIGHT_TO_BASE;
    else
      stateForReturnToBase = RE_INIT_WALL_FOLLER;
    break;
  case GO_STRAIGHT_TO_BASE:
    goStraightToBase();
    if (inOneMetterRadiusFromStartingPoint())
      setState(STATE_LANDING);
    else
      stateForReturnToBase = VERIFY_WAY_AHEAD;
    break;

  case RE_INIT_WALL_FOLLER:
    initstraightWallFollower();
    startFindingAngleTimer();
    stateForReturnToBase = WALL_FOLLOWER;
    break;
  
  case WALL_FOLLOWER:
    followTheWall();
    break;
  default:
    break;
  }
}


void verifyDistanceToBase()
{#include "../headers/information.h"
#include "../headers/main_commands.h"
#include "../headers/wall_following_projet3.h"
#include "../headers/return_to_base.h"

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"

#include "debug.h"

#include "ledseq.h"
#include "commander.h"
#include "crtp_commander_high_level.h"
#include "locodeck.h"
#include "mem.h"
#include "log.h"
#include "param.h"
#include "pm.h"
#include "app_channel.h"
#include "system.h"
#include "../../vendor/FreeRTOS/include/FreeRTOS.h"
#include "../../vendor/FreeRTOS/include/timers.h"

// Log and param ids
static logVarId_t logIdStateEstimateX;
static logVarId_t logIdStateEstimateY;
static logVarId_t logIdStateEstimateZ;
static paramVarId_t paramIdCommanderEnHighLevel;
static float TAKE_OFF_HEIGHT = 0.5f;

static float padX = 0.0;
static float padY = 0.0;
static float padZ = 0.0;

static float getX() { return logGetFloat(logIdStateEstimateX); }
static float getY() { return logGetFloat(logIdStateEstimateY); }
static float getZ() { return logGetFloat(logIdStateEstimateZ); }

static void enableHighlevelCommander() { paramSetInt(paramIdCommanderEnHighLevel, 1); }

// Return to base parameters
// Counter and millisecond to ticks function, both for the timer.
TimerHandle_t AnglerTimer;
#ifndef pdMS_TO_TICKS
    #define pdMS_TO_TICKS( xTimeInMs )    ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000U ) )
#endif
enum state_return_base stateForReturnToBase = FINDING_ANGLE;

// We made a simple takeOff for demonstration. The real one (for exploration) is implemented
// in the wall_following function.
void takeOff()
{
  logIdStateEstimateX = logGetVarId("stateEstimate", "x");
  logIdStateEstimateY = logGetVarId("stateEstimate", "y");
  logIdStateEstimateZ = logGetVarId("stateEstimate", "z");
  paramIdCommanderEnHighLevel = paramGetVarId("commander", "enHighLevel");
  enableHighlevelCommander();

  padX = getX();
  padY = getY();
  padZ = getZ();
  crtpCommanderHighLevelTakeoff(padZ + TAKE_OFF_HEIGHT, 1.0f);
  setState(STATE_HOVERING);
}

void landing()
{
  // The landing is implemented in the wall_following of our project3 to reuse the 
  // initialized commander.
  endExploration();
  setState(STATE_IDLE);
}

void exploration() 
{
  followTheWall();
}

void setStateReturnHome()
{
  stateForReturnToBase = VERIFY_DISTANCE_TO_BASE;
}
bool turnAround = true;
bool goFoward = false;
void returnToBase()
{
  // Return home has its home state machine
  switch (stateForReturnToBase)
  {
  case VERIFY_DISTANCE_TO_BASE:
    verifyDistanceToBase();
    break;
  case FINDING_ANGLE:
    /* code */
    findAngle();
    turnAround = false;
    stateForReturnToBase = TURN_TO_ANGLE;
    break;
  case TURN_TO_ANGLE:
    turnAround = turnToAngle();
    if(turnAround)
      stateForReturnToBase = VERIFY_WAY_AHEAD;
    break;

  case VERIFY_WAY_AHEAD:
    goFoward = wayAheadIsClean();
    if(goFoward)
      stateForReturnToBase = GO_STRAIGHT_TO_BASE;
    else
      stateForReturnToBase = RE_INIT_WALL_FOLLER;
    break;
  case GO_STRAIGHT_TO_BASE:
    goStraightToBase();
    if (inOneMetterRadiusFromStartingPoint())
      setState(STATE_LANDING);
    else
      stateForReturnToBase = VERIFY_WAY_AHEAD;
    break;

  case RE_INIT_WALL_FOLLER:
    initstraightWallFollower();
    startFindingAngleTimer();
    stateForReturnToBase = WALL_FOLLOWER;
    break;
  
  case WALL_FOLLOWER:
    followTheWall();
    break;
  default:
    break;
  }
}


void verifyDistanceToBase()
{
  if (inOneMetterRadiusFromStartingPoint())
    setState(STATE_LANDING);
  else
    stateForReturnToBase = FINDING_ANGLE;
}

void callWallFollower()
{
  followTheWall();
}

void findingAngle(xTimerHandle xTimer)
{
  stateForReturnToBase = FINDING_ANGLE;
  xTimerStop(xTimer, 0);
}


void createFindingAngleTimer() {
  AnglerTimer = xTimerCreate("FindingAngleTimer", pdMS_TO_TICKS(11000), pdTRUE, 0, findingAngle);
}

void startFindingAngleTimer() {
  xTimerStart(AnglerTimer, 0);
}
  if (inOneMetterRadiusFromStartingPoint())
    setState(STATE_LANDING);
  else
    stateForReturnToBase = FINDING_ANGLE;
}

void callWallFollower()
{
  followTheWall();
}

void findingAngle(xTimerHandle xTimer)
{
  stateForReturnToBase = FINDING_ANGLE;
  xTimerStop(xTimer, 0);
}


void createFindingAngleTimer() {
  AnglerTimer = xTimerCreate("FindingAngleTimer", pdMS_TO_TICKS(11000), pdTRUE, 0, findingAngle);
}

void startFindingAngleTimer() {
  xTimerStart(AnglerTimer, 0);
}