/**
 * ,---------,       ____  _ __
 * |  ,-^-,  |      / __ )(_) /_______________ _____  ___
 * | (  O  ) |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
 * | / ,--´  |    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
 *    +------`   /_____/_/\__/\___/_/   \__,_/ /___/\___/
 *
 * Crazyflie control firmware
 *
 * Copyright (C) 2020 Bitcraze AB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, in version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * api_app.c - App layer application that calls app API functions to make
 *             sure they are compiled in CI.
 */


#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "app.h"

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

// Projet 3 headers
#include "../headers/del_handler.h"
#include "../headers/command_master.h"
#include "../headers/information.h"
#include "../headers/return_to_base.h"
#include "../headers/main_commands.h"

#define DEBUG_MODULE "APPAPI"

typedef enum {DEL = 0, COMMAND, INFORMATION} request_type_t;

void appMain() {
  // Initializing section
  enum State state = getState();
  initFromCommandMaster();
  createIdentificationTimer();
  initiateInformation();
  

  while(1) {
    // Crashed verification
    if (getCrashedState())
      setState(STATE_CRASHED);
    else if(getState() == STATE_CRASHED)
      setState(STATE_IDLE);

    // Here, we receive the packets and prepare the next state accordingly.
    if (appchannelReceivePacket(&rxPacket, sizeof(rxPacket), 0)) {
      // We first get the old state
      state = getState();
      // Then, we verify which type of request as well as the type of command that are received.
      if(rxPacket.request_type == COMMAND && rxPacket.command == TAKE_OFF){
        if (state == STATE_IDLE)
          setState(STATE_TAKE_OFF);
      }
      else if(rxPacket.request_type == COMMAND && rxPacket.command == LANDING){
        if (state == STATE_LANDING && commandIsFinish())
          setState(STATE_IDLE);
        else if (state == STATE_TAKE_OFF || state == STATE_EXPLORATION || state == STATE_RETURN_HOME)
          setState(STATE_LANDING);
      }
      else if(rxPacket.request_type == COMMAND && rxPacket.command == EXPLORATION){          
        setTimeForLowBattery();
        setState(STATE_EXPLORATION);
        prepareExplorationCommandMaster();
      }
      else if(rxPacket.request_type == COMMAND && rxPacket.command == RETURN_TO_BASE){          
        setState(STATE_RETURN_HOME);
        setStateReturnHome();        
        prepareExplorationFromCommandMaster();
      }
      else if(rxPacket.request_type == DEL && rxPacket.command == IDENTIFICATION){
        del_handler(IDENTIFICATION);
      }        
    }

    // We get the new state.
    state = getState();
    ////////// STATE MACHINE //////////
    switch (state)
    {
    case STATE_CRASHED:
      // Not used, but useful for the web client.
      break;

    case STATE_IDLE:
      // No need to say more.
      break;

    case STATE_TAKE_OFF:
      launchCommand(TAKE_OFF);
      break;

    case STATE_LANDING:
      launchCommand(LANDING);
      break;

    case STATE_EXPLORATION:
      launchCommand(EXPLORATION);
      break;

    case STATE_RETURN_HOME:
      launchCommand(RETURN_TO_BASE);
      break;
    
    default:
      break;
    }
  }
}
