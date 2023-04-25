/*
 * AUTHORS: Carlo Pinciroli <cpinciro@ulb.ac.be>
 *          Pierre-Yves Lajoie <lajoie.py@gmail.com>
 *
 * An example crazyflie drones sensing.
 *
 * This controller is meant to be used with the XML file:
 *    experiments/foraging.argos
 */

#ifndef CRAZYFLIE_SENSING_H
#define CRAZYFLIE_SENSING_H

/*
 * Include some necessary headers.
 */
/* Definition of the CCI_Controller class. */
#include <argos3/core/control_interface/ci_controller.h>
/* Definition of the crazyflie distance sensor */
#include <argos3/plugins/robots/crazyflie/control_interface/ci_crazyflie_distance_scanner_sensor.h>
/* Definition of the crazyflie position actuator */
#include <argos3/plugins/robots/generic/control_interface/ci_quadrotor_position_actuator.h>
/* Definition of the crazyflie position sensor */
#include <argos3/plugins/robots/generic/control_interface/ci_positioning_sensor.h>
/* Definition of the crazyflie range and bearing actuator */
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_actuator.h>
/* Definition of the crazyflie range and bearing sensor */
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_sensor.h>
/* Definition of the crazyflie battery sensor */
#include <argos3/plugins/robots/generic/control_interface/ci_battery_sensor.h>
/* Definitions for random number generation */
#include <argos3/core/utility/math/rng.h>
/* Definition of the crazyflie speed actuator */
// #include <argos3/plugins/robots/generic/control_interface/ci_quadrotor_speed_actuator.h>
/* Definition of the crazyflie speed actuator */
#include <argos3/plugins/robots/generic/simulator/quadrotor_speed_default_actuator.h>

#include "SocketManager.h"

/*
 * All the ARGoS stuff in the 'argos' namespace.
 * With this statement, you save typing argos:: every time.
 */
using namespace argos;
enum State {
      WAITING,
      EXPLORE,
      CRASHED
};

enum Command {
      TAKEOFF = 1,
      LAND = 2,
      BACKTOBASE = 3,
      SENDINFO = 4
};


/*
 * A controller is simply an implementation of the CCI_Controller class.
 */
class CCrazyflieSensing : public CCI_Controller {

public:

   /* Class constructor. */
   CCrazyflieSensing();
   /* Class destructor. */
   virtual ~CCrazyflieSensing() {delete m_oSocketManager;}

   /*
    * This function initializes the controller.
    * The 't_node' variable points to the <parameters> section in the XML
    * file in the <controllers><footbot_foraging_controller> section.
    */
   virtual void Init(TConfigurationNode& t_node);

   /*
    * This function is called once every time step.
    * The length of the time step is set in the XML file.
    */
   virtual void ControlStep();

   int CreateStatusMessage();

   Real GetDegreeAngle();

   bool TakeOff();

   void Land();

   virtual void Reset();

   CVector3 GenerateExplorePos();

   virtual void Destroy() {}

   void RandomExplore();

   void BackToBase();


   void DefaultExplore();

   bool IsAtBase();   

   bool BatteryIsEnough();

   void TakeOffAndExplore();

private:

   CCI_QuadRotorSpeedActuator* m_pcSpeedAct;

   /* Pointer to the crazyflie distance sensor */
   CCI_CrazyflieDistanceScannerSensor* m_pcDistance;

   /* Pointer to the position actuator */
   CCI_QuadRotorPositionActuator* m_pcPropellers;
   
   /* Pointer to the range and bearing actuator */
   CCI_RangeAndBearingActuator*  m_pcRABA;

   /* Pointer to the range and bearing sensor */
   CCI_RangeAndBearingSensor* m_pcRABS;

   /* Pointer to the positioning sensor */
   CCI_PositioningSensor* m_pcPos;

   /* Pointer to the battery sensor */
   CCI_BatterySensor* m_pcBattery;

   /* The random number generator */
   CRandom::CRNG* m_pcRNG;

   /* The socket manager */
   SocketManager* m_oSocketManager;
    
   /* Initial Position */ 
   CVector3 m_cInitialPosition;

   /* Final destination */ 
   CVector3 m_cFinalDestination;

   /* state */ 
   State m_state;

   /* if it has land for the last time */
   bool m_bDoneFinalLanding;
};

#endif
