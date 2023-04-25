
#include "led.h"
#include "../headers/del_handler.h"



#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"

#include "debug.h"

#include "ledseq.h"
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

// Counter and millisecond to ticks function, both for the timer.
uint16_t onCounter = 0;
TimerHandle_t timer;
#ifndef pdMS_TO_TICKS
    #define pdMS_TO_TICKS( xTimeInMs )    ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000U ) )
#endif

void del_handler(del_command_t del_command)
{
  switch (del_command)
  {
  case IDENTIFICATION:
    startDroneIdentificationTimer();
    break;

  case P2P_DEL_COLOR_CHANGING:
    // code ...
    break;
  
  default:
    break;
  }
}

void identifyDrone(xTimerHandle xTimer)
{

  bool isOn = (onCounter % 2 == 0);
  if (isOn)
    ledSet(LED_GREEN_R, 1);
  else
    ledSet(LED_GREEN_R, 0);
  
  onCounter++;
  if (onCounter >= IDENTIFICATION_BLINK_TIME) {
    onCounter = 0;
    xTimerStop(xTimer, 0);
  }
}

void createIdentificationTimer() {
  timer = xTimerCreate("droneIdentificationTimer", pdMS_TO_TICKS(100), pdTRUE, 0, identifyDrone);
}

void startDroneIdentificationTimer() {
  onCounter = 0;
  xTimerStart(timer, 0);
}