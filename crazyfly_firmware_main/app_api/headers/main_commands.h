#ifndef _MAIN_COMMAND__
#define _MAIN_COMMAND__

enum state_return_base {
    VERIFY_DISTANCE_TO_BASE = 0,
    FINDING_ANGLE,
    TURN_TO_ANGLE,
    VERIFY_WAY_AHEAD,
    GO_STRAIGHT_TO_BASE,
    RE_INIT_WALL_FOLLER,
    WALL_FOLLOWER,
};

/**
 * @brief Init function for return to base
 *
 * @param void None
 * @return void
 */
void prepareExplorationMainCommand();


/**
 * @brief takeOff before the exploration.
 * We made a simple takeOff for demonstration. The real one (for exploration) is implemented
 * in the wall_following function.
 *
 * @param void None
 * @return void
 */
void takeOff();

/**
 * @brief landing after the exploration or after a simple take off.
 *
 * @param void None
 * @return void
 */
void landing();

/**
 * @brief Our exploration which is wall following
 *
 * @param void None
 * @return void
 */
void exploration() ;


/**
 * @brief We initiate the state machine for return to base.
 *
 * @param void None
 * @return void
 */
void setStateReturnHome();

/**
 * @brief Our return to base function
 * It uses its own state machine
 *
 * @param void None
 * @return void
 */
void returnToBase();

/**
 * @brief Here, we verify how far is the base
 * When the drone is in the 0.5 meters radius from base,
 * we switch to landing state
 *
 * @param void None
 * @return void
 */
void verifyDistanceToBase();

/**
 * @brief Function to call wall follower in the state machine
 *
 * @param void None
 * @return void
 */
void callWallFollower();

/**
 * @brief Create the timer to find the return to base angle
 * It is needed to verify if we can go directly home every X seconds
 *
 * @param void None
 * @return void
 */
void createFindingAngleTimer();

/**
 * @brief Here, we find the angle to go to the base (starting point)
 *
 * @param void None
 * @return void
 */
void findingAngle();

/**
 * @brief Here, we start the finding angle timer
 *
 * @param void None
 * @return void
 */
void startFindingAngleTimer();

// STATE MACHINE FOR RETURN HOME Projet3
/**
 * @brief state FINDING_ANGLE logic
 *
 * @param void None
 * @return void
 */
void findAngleState();

/**
 * @brief state TURN_TO_ANGLE logic
 *
 * @param void None
 * @return void
 */
void turnToAngleState();

/**
 * @brief state VERIFY_WAY_AHEAD logic
 *
 * @param void None
 * @return void
 */
void verifyWayAheadState();

/**
 * @brief state GO_STRAIGHT_TO_BASE logic
 *
 * @param void None
 * @return void
 */
void goStraightToBaseState();

/**
 * @brief state RE_INIT_WALL_FOLLER logic
 *
 * @param void None
 * @return void
 */
void reInitWallFollowerState();

#endif