#include "../headers/information.h"
#include "../headers/wall_following_projet3.h"
// #include "log.h"
#include "../headers/return_to_base.h"
#include "usec_time.h"

// Main state machine state
enum State state = STATE_IDLE;

// battery not in information.h because it already exists
float battery;
// Variables for battery voltage check
const float MINIMUM_BATTERY_LEVEL_FOR_EXPLORATION = 30.0f;
const float TIME_FOR_EXPLORATION_WHEN_BATTERY_IS_FULL = 360.0f; // in 6 minutes, so 360 seconds.
const float FULL_CHARGE = 100.0f;

// log variables functions
float getCrashedState() { return logGetFloat(isTumbled); }
float getStateX() { return logGetFloat(logIdKalmanStateX); }
float getStateY() { return logGetFloat(logIdKalmanStateY); }
float getyaw() { return logGetFloat(idStabilizerYawReturnHome); }
float getFrontDistance() { return (float)logGetFloat(idFrontReturnHome); }

void setState(enum State newState)
{
    state = newState;
}

enum State getState()
{
    return state;
}

void initiateInformation()
{
    estimatedTimeForMission = 0.0f;
    logIdKalmanStateX = logGetVarId("kalman", "stateX");
    logIdKalmanStateY = logGetVarId("kalman", "stateY");
    idFrontReturnHome = logGetVarId("range", "front");
    // Getting the Logging IDs of the state estimates
    idStabilizerYawReturnHome = logGetVarId("stabilizer", "yaw");

    vbatId = logGetVarId("pm", "vbat");
    isTumbled = logGetVarId("sys", "isTumbled");
}

float getBatteryLevel()
{
    voltage = logGetFloat(vbatId);
    battery = 0.0f;

    // Getting the charge % from the voltage table mentionned above.
    if (voltage <= 4.2f && voltage > 4.15f) battery = 100.0f;
    else if( voltage <= 4.15f && voltage > 4.11f ) battery = 95.0f;
    else if( voltage <= 4.11f && voltage > 4.08f ) battery = 90.0f;
    else if( voltage <= 4.08f && voltage > 4.02f ) battery = 85.0f;
    else if( voltage <= 4.02f && voltage > 3.98f ) battery = 80.0f;

    else if( voltage <= 3.98f && voltage > 3.95f ) battery = 75.0f;
    else if( voltage <= 3.95f && voltage > 3.91f ) battery = 70.0f;
    else if( voltage <= 3.91f && voltage > 3.87f ) battery = 65.0f;
    else if( voltage <= 3.87f && voltage > 3.85f ) battery = 60.0f;

    else if( voltage <= 3.85f && voltage > 3.84f ) battery = 55.0f;
    else if( voltage <= 3.84f && voltage > 3.82f ) battery = 50.0f;
    else if( voltage <= 3.82f && voltage > 3.80f ) battery = 45.0f;
    else if( voltage <= 3.80f && voltage > 3.79f ) battery = 40.0f;
    
    else if( voltage <= 3.79f && voltage > 3.77f ) battery = 35.0f;
    else if( voltage <= 3.77f && voltage > 3.75f ) battery = 30.0f;
    else if( voltage <= 3.75f && voltage > 3.73f ) battery = 25.0f;
    else if( voltage <= 3.73f && voltage > 3.71f ) battery = 20.0f;

    else if( voltage <= 3.71f && voltage > 3.69f ) battery = 15.0f;
    else if( voltage <= 3.69f && voltage > 3.61f ) battery = 10.0f;
    else if( voltage <= 3.61f && voltage > 3.27f ) battery = 5.0f;
    else if( voltage <= 3.27f) battery = 0.0f;



    return battery;
}


void setTimeForLowBattery()
{
    // Battery Check
    if (getState() != STATE_EXPLORATION)
    {
        timeStartMission = usecTimestamp() / 1e6;
        calculateTimeForMission();
    }
}

void calculateTimeForMission()
{
    battery = getBatteryLevel();
    // 6 minutes = 360s
    // From next equation: X remaining seconds / battery = 360 seconds / 100 % battery charge
    // We get X remaining seconds = 360 seconds * battery / 100 % battery charge
    float estimatedTimeFlightForThirtySeconds = (TIME_FOR_EXPLORATION_WHEN_BATTERY_IS_FULL * MINIMUM_BATTERY_LEVEL_FOR_EXPLORATION) / FULL_CHARGE;  
    estimatedTimeForMission = (TIME_FOR_EXPLORATION_WHEN_BATTERY_IS_FULL * battery) / FULL_CHARGE - estimatedTimeFlightForThirtySeconds;
}

bool timeForMissionHasPassed()
{
    timeNow = usecTimestamp() / 1e6;
    if (timeNow - timeStartMission > estimatedTimeForMission)
    {
        timeNow = timeStartMission;
        return true;
    }
    return false;
}
    


LOG_GROUP_START(information)
LOG_ADD(LOG_INT8, state, &state)
LOG_GROUP_STOP(information)
