#ifndef DEL_HANDLER_H_
#define DEL_HANDLER_H_

#define IDENTIFICATION_BLINK_TIME 100 // 10 seconds.

// Commands used by the del_handler
typedef enum {IDENTIFICATION = 0, P2P_DEL_COLOR_CHANGING} del_command_t;

/**
 * @brief Manage the del command.
 *
 * @param del_command the del command
 * @return void
 */
void del_handler(del_command_t del_command);

/**
 * @brief The identification is done by making the green led blink for 10 seconds.
 * It is a callback function activated by startDroneIdentificationTimer().
 *
 * @param void None
 * @return void
 */
void identifyDrone();

/**
 * @brief This function creates the a timer for identifyDrone().
 *
 * @param void None
 * @return void
 */
void createIdentificationTimer();

/**
 * @brief This function activates the a timer for identifyDrone().
 * It also set the timer to 0.
 *
 * @param void None
 * @return void
 */
void startDroneIdentificationTimer();

#endif