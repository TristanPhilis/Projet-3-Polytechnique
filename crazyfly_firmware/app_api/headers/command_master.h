#ifndef _COMMAND_MASTER_H
#define _COMMAND_MASTER_H
#include <stdbool.h>


// Commands used by the command_master
typedef enum {TAKE_OFF = 0, LANDING, RETURN_TO_BASE, EXPLORATION} command_t;

/**
 * @brief Launches the appropriate command. They are all related to motions
 * which is why we call the commands the main commands.
 *
 * @param command Mission command
 * @return void
 */
void launchCommand(command_t command);

/**
 * @brief Verifify if a highLevelComander function is over
 *
 * @param void None
 * @return void
 */
bool commandIsFinish();

/**
 * @brief Create the timer used in return to base. 
 * Used to periodically turn to return home angle.
 *
 * @param void None
 * @return void
 */
void createFindingAngleTimerInMainCommand();

/**
 * @brief Function to prepare a new exploration when command is received from groundstation
 *
 * @param void None
 * @return void
 */
void prepareExplorationCommandMaster();

/**
 * @brief CommandMaster's initialisation.
 *
 * @param void None
 * @return void
 */
void initFromCommandMaster();

/**
 * @brief Simply call prepareExploration() from MainCommand
 *
 * @param void None
 * @return void
 */
void prepareExplorationFromCommandMaster();

#endif