#ifndef INFORMATION_H_
#define INFORMATION_H_

#include <stdbool.h>
#include "log.h"

enum State {
  STATE_IDLE = 0,
  STATE_TAKE_OFF,
  STATE_EXPLORATION,
  STATE_RETURN_HOME,
  STATE_LANDING,
  STATE_CRASHED,
};

typedef enum {
    DRONE_STATE = 0,
    BATTERY_LEVEL,
    DRONE_COORDINATES,
} information_t;

struct commandPacketRX {
  int request_type;
  int command;
}__attribute__((packed));
struct commandPacketRX rxPacket;

// Drone position
typedef struct {
  float x;
  float y;
  float yaw;
} Drone_coordinates_t;
Drone_coordinates_t droneCoordinates;

// log variables functions
/**
 * @brief Return a bool that indicates if the drone crashed
 *
 * @param void none
 * @return According to bitcraze's documentation, it returns Nonzero if the system thinks it is tumbled/crashed.
 */
float getCrashedState();

/**
 * @brief Returns drone position in x with kalman stateX
 *
 * @param void None
 * @return drone's x position
 */
float getStateX();

/**
 * @brief Returns drone position in x with kalman stateY
 *
 * @param void None
 * @return drone's y position
 */
float getStateY();

/**
 * @brief Returns drone angle in z with stabilizer yaw
 *
 * @param void None
 * @return drone's yaw angle
 */
float getyaw();

/**
 * @brief Returns drone front sensor distance from range front
 *
 * @param void None
 * @return distance of object in front
 */
float getFrontDistance();

// State for app_main's state machine
/**
 * @brief Change app_main state machine's state.
 *
 * @param void new state to be set
 * @return noce
 */
void setState(enum State newState);

/**
 * @brief Returns app_main state machine's state.
 *
 * @param void none
 * @return state
 */
enum State getState();

/**
 * @brief Information init. Initializes variables
 *
 * @param void none
 * @return none
 */
void initiateInformation();

// Battery functions
/**
 * @brief Calculate battery level according to current voltage
 *
 * @param void none
 * @return battery level
 */
float getBatteryLevel();

/**
 * @brief To be able to estimate the time for a mission, we must initializes variables, namely a timestamp from usec_time
 *
 * @param void none
 * @return none
 */
void setTimeForLowBattery();

/**
 * @brief Calculate the time for the mission until we have 30 % battery left.
 *
 * @param void none
 * @return none
 */
void calculateTimeForMission();

/**
 * @brief Verify if mission is over with a time stamp
 *
 * @param void none
 * @return none
 */
bool timeForMissionHasPassed();

// Variables for battery voltage check
float timeStartMission;
float timeNow;
float estimatedTimeForMission;
float voltage;

// Log variables
logVarId_t idFrontReturnHome;
logVarId_t logIdKalmanStateX; // For position in X Projet 3 Genie informatique.
logVarId_t logIdKalmanStateY; // For position in Y Projet 3 Genie informatique.
logVarId_t idStabilizerYawReturnHome;
logVarId_t vbatId;
logVarId_t isTumbled;

#endif /* INFORMATION_H_ */