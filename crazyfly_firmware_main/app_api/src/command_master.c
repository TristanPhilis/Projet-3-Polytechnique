#include "../headers/main_commands.h"
#include "../headers/command_master.h"
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
/////////////////////////////////////////////////////
////////////////////// TESTING //////////////////////
/////////////////////////////////////////////////////

#include "app.h"

void launchCommand(command_t command)
{
    switch (command)
    {
    case TAKE_OFF:
        takeOff();
        break;
    
    case LANDING:
        landing();
        break;
            
    case RETURN_TO_BASE:
        returnToBase();
        break;

    case EXPLORATION:
        exploration();
        break;

    default:
        break;
    }
}

bool commandIsFinish()
{
    return crtpCommanderHighLevelIsTrajectoryFinished();
}

void createFindingAngleTimerInMainCommand()
{
    createFindingAngleTimer();
}

void prepareExplorationCommandMaster()
{
  prepareExplorationMainCommand();
}

void initFromCommandMaster()
{
    initFollowWallProjet3();
    createFindingAngleTimer();
    initReturnHome();
}

void prepareExplorationFromCommandMaster()
{
  prepareExplorationMainCommand();
}