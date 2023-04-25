/* Include the controller definition */
#include "crazyflie_sensing.h"
/* Function definitions for XML parsing */
#include <argos3/core/utility/configuration/argos_configuration.h>
/* 2D vector definition */
#include <argos3/core/utility/math/vector2.h>
#include <argos3/core/utility/math/range.h>
/* Logging */
#include <argos3/core/utility/logging/argos_log.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

static Real arenaPosMin = -2.5f;
static Real arenaPosMax = 2.5f;
static Real hoverHeight = 0.8f;
static int command;

/****************************************/
/****************************************/

CCrazyflieSensing::CCrazyflieSensing() :
   m_pcDistance(NULL),
   m_pcPropellers(NULL),
   m_pcRNG(NULL),
   m_pcRABA(NULL),
   m_pcRABS(NULL),
   m_pcPos(NULL),
   m_pcBattery(NULL),
   m_oSocketManager(NULL) {
   }

/****************************************/
/****************************************/

void CCrazyflieSensing::Init(TConfigurationNode& t_node) {
   try {
      m_pcDistance   = GetSensor  <CCI_CrazyflieDistanceScannerSensor>("crazyflie_distance_scanner");
      m_pcPropellers = GetActuator  <CCI_QuadRotorPositionActuator>("quadrotor_position");
      /* Get pointers to devices */
      m_pcRABA   = GetActuator<CCI_RangeAndBearingActuator>("range_and_bearing");
      m_pcRABS   = GetSensor  <CCI_RangeAndBearingSensor  >("range_and_bearing");
      try {
         m_pcPos = GetSensor  <CCI_PositioningSensor>("positioning");
      }
      catch(CARGoSException& ex) {}
      try {
         m_pcBattery = GetSensor<CCI_BatterySensor>("battery");
      }
      catch(CARGoSException& ex) {}

      m_oSocketManager = new SocketManager();

      if(m_oSocketManager->ConnectSocket() != -1){ // connecting and verifying if it's succesful
         LOG << "Connection Successful" << std::endl;
      }
   }      
          
   catch(CARGoSException& ex) {
      THROW_ARGOSEXCEPTION_NESTED("Error initializing the crazyflie sensing controller for robot \"" << GetId() << "\"", ex);
   }
   /*
    * Initialize other stuff
    */
   /* Create a random number generator. We use the 'argos' category so
      that creation, reset, seeding and cleanup are managed by ARGoS. */
   m_pcRNG = CRandom::CreateRNG("argos");
   m_state = WAITING;
   m_cInitialPosition = m_pcPos->GetReading().Position;
   m_bDoneFinalLanding = false;
}

/****************************************/
/****************************************/

void CCrazyflieSensing::ControlStep() {

   struct dataObject data;
   dataObject *receivedData = &data;
   int incMessage = m_oSocketManager->ReceiveValue(receivedData);
   
   if (incMessage >= TAKEOFF && incMessage != SENDINFO) {
      command = incMessage;
   }

   if (incMessage == SENDINFO){
      CreateStatusMessage();
   }
   
   if (BatteryIsEnough())
   {  
      switch (command)
      {
         case TAKEOFF: //takeOff command
            TakeOffAndExplore();
            break;

         case LAND: //landing command
            Land();
            break;

         case BACKTOBASE: //back to base
            BackToBase();
            break;

         default:
            RandomExplore();
            break;
      }
   }
   else if (!m_bDoneFinalLanding && ! !IsAtBase())   
   {      
      BackToBase();
      m_bDoneFinalLanding = true;    
   }
   else if (!m_bDoneFinalLanding)
   {
      Land();
      m_bDoneFinalLanding = true;
   }
   
   // Print current position.
   LOG << "Position (x,y,z) = (" <<m_pcPos->GetReading().Position.GetX() << ","
       << m_pcPos->GetReading().Position.GetY() << ","
       << m_pcPos->GetReading().Position.GetZ() << ")" << std::endl; 

   // Print current battery level
   const CCI_BatterySensor::SReading& sBatRead = m_pcBattery->GetReading();
   LOG << "Battery level: " << sBatRead.AvailableCharge  << std::endl;
   
}

int CCrazyflieSensing::CreateStatusMessage() {
   char message[1024];
   strcpy(message, (GetId().substr(1)).c_str());

   strcat(message, ",");
   CVector3 currentPosition = m_pcPos->GetReading().Position;
   strcat(message, std::to_string(currentPosition.GetX()*1000).c_str());   
   strcat(message, ",");
   strcat(message, std::to_string(currentPosition.GetY()*1000).c_str());

   strcat(message, ",");
   strcat(message, std::to_string(m_pcBattery->GetReading().AvailableCharge*100).c_str());
   
   strcat(message, ",");
   strcat(message, std::to_string(m_state).c_str());
   
   CCI_CrazyflieDistanceScannerSensor::TReadingsMap sDistRead = m_pcDistance->GetReadingsMap();
   auto iterDistRead = sDistRead.begin();
   strcat(message, ",");
   strcat(message, std::to_string((iterDistRead++)->second*10).c_str()); //front
   strcat(message, ",");
   strcat(message, std::to_string((iterDistRead++)->second*10).c_str()); //left
   strcat(message, ",");
   strcat(message, std::to_string((iterDistRead++)->second*10).c_str()); //back
   strcat(message, ",");
   strcat(message, std::to_string((iterDistRead++)->second).c_str()); //right

   strcat(message, ",");
   strcat(message, std::to_string(GetDegreeAngle()).c_str()); 

   return this->m_oSocketManager->SendMessage(message);
}

Real CCrazyflieSensing::GetDegreeAngle() {
   CRadians p_zRadAngle, p_yRadAngle, p_xRadAngle;
   m_pcPos->GetReading().Orientation.ToEulerAngles(p_zRadAngle, p_yRadAngle, p_xRadAngle);
   CDegrees p_zDegAngle = ToDegrees(p_zRadAngle);
   return p_zDegAngle.GetValue();
}

bool CCrazyflieSensing::TakeOff() {
   CVector3 cPos = m_pcPos->GetReading().Position;
   CVector3 takeOffPos(cPos.GetX(), cPos.GetY(), hoverHeight);
   m_pcPropellers->SetAbsolutePosition(takeOffPos); 
   
   if(cPos.GetZ() >= hoverHeight) 
   {
      return true;
   }
   return false;
}

/****************************************/
/****************************************/

void CCrazyflieSensing::Land() {
   if (m_state == EXPLORE)
   {
      CVector3 cPos = m_pcPos->GetReading().Position;
      cPos.SetZ(0.0f);
      m_pcPropellers->SetAbsolutePosition(cPos);
   }
   m_state = WAITING;
}

/****************************************/
/****************************************/

void CCrazyflieSensing::Reset() {
   try {
      /*
       * Initialize sensors/actuators
       */
      m_pcDistance   = GetSensor  <CCI_CrazyflieDistanceScannerSensor>("crazyflie_distance_scanner");
      m_pcPropellers = GetActuator  <CCI_QuadRotorPositionActuator>("quadrotor_position");
      /* Get pointers to devices */
      m_pcRABA   = GetActuator<CCI_RangeAndBearingActuator>("range_and_bearing");
      m_pcRABS   = GetSensor  <CCI_RangeAndBearingSensor  >("range_and_bearing");
      try {
         m_pcPos = GetSensor  <CCI_PositioningSensor>("positioning");
      }
      catch(CARGoSException& ex) {}
      try {
         m_pcBattery = GetSensor<CCI_BatterySensor>("battery");
      }
      catch(CARGoSException& ex) {}

      m_oSocketManager = new SocketManager();
      
      if(m_oSocketManager->ConnectSocket() != -1){ // connecting and verifying if it's succesful
         LOG << "Connection Successful" << std::endl;
      }      
   }
   catch(CARGoSException& ex) {
      THROW_ARGOSEXCEPTION_NESTED("Error initializing the crazyflie sensing controller for robot \"" << GetId() << "\"", ex);
   }
   /*
    * Initialize other stuff
    */
   /* Create a random number generator. We use the 'argos' category so
      that creation, reset, seeding and cleanup are managed by ARGoS. */
   m_pcRNG = CRandom::CreateRNG("argos");
   m_state = WAITING;
   m_cInitialPosition = m_pcPos->GetReading().Position;
   m_bDoneFinalLanding = false;
}

CVector3 CCrazyflieSensing::GenerateExplorePos() {
   CVector3 translation(0.0f, 0.0f, hoverHeight);
   CRange<Real> range = CRange<Real>(arenaPosMin,arenaPosMax);

   float dirX = m_pcRNG->Uniform(range); //get a position in the arena
   float dirY = m_pcRNG->Uniform(range);
   translation.SetX(dirX);
   translation.SetY(dirY);
   return translation;
}


void CCrazyflieSensing::RandomExplore() {
   if (m_state == EXPLORE) {
      if (m_cFinalDestination.GetX() == 0.0f && m_cFinalDestination.GetY() == 0.0f) {
         m_cFinalDestination = GenerateExplorePos();
      }

      CVector3 currentPosition = m_pcPos->GetReading().Position;
      Real deltaDistX = abs(currentPosition.GetX() - m_cFinalDestination.GetX());
      Real deltaDistY = abs(currentPosition.GetY() - m_cFinalDestination.GetY());
      if (deltaDistX < 0.1f && deltaDistY < 0.1f) {
         m_cFinalDestination = GenerateExplorePos();
      }
      m_pcPropellers->SetAbsolutePosition(m_cFinalDestination);
   }
}

void CCrazyflieSensing::BackToBase() {
   CVector3 cPos = m_cInitialPosition;
   if (m_state == EXPLORE )
   {
      m_pcPropellers->SetAbsolutePosition(cPos);
   }
   else if (m_state == WAITING)
   {
      TakeOff();
      m_pcPropellers->SetAbsolutePosition(cPos);
   }
   m_state = WAITING;
}

bool CCrazyflieSensing::IsAtBase() {
   CVector3 cPos = m_pcPos->GetReading().Position;
   Real diffX = cPos.GetX() - m_cInitialPosition.GetX();
   Real diffY = cPos.GetY() - m_cInitialPosition.GetY();
   return (((diffX*diffX) + (diffY*diffY)) <= 1);
}

bool CCrazyflieSensing::BatteryIsEnough() {
   const CCI_BatterySensor::SReading& sBatRead = m_pcBattery->GetReading();
   return (sBatRead.AvailableCharge >= 0.3f);
} 

void CCrazyflieSensing::TakeOffAndExplore() {
   if (m_state == EXPLORE)
   {
      RandomExplore();
   }
   else {
      if(TakeOff()) {
         m_state = EXPLORE;  
      }
   }
}

/*
 * This statement notifies ARGoS of the existence of the controller.
 * It binds the class passed as first argument to the string passed as
 * second argument.
 * The string is then usable in the XML configuration file to refer to
 * this controller.
 * When ARGoS reads that string in the XML file, it knows which controller
 * class to instantiate.
 * See also the XML configuration files for an example of how this is used.
 */
REGISTER_CONTROLLER(CCrazyflieSensing, "crazyflie_sensing_controller")
