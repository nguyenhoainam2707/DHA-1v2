#ifndef _Service_
#define _Service_
/* _____PROJECT INCLUDES____________________________________________________ */

#include "Arduino.h"
#include "../apps/App.h"
/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____DEFINATIONS__________________________________________________________ */
enum SER_Run_Mode
  {
    SER_RUN_MODE_MANUAL,
    SER_RUN_MODE_AUTO
  };
 enum SER_State
  {
    SER_STATE_STARTING,
    SER_STATE_EXECUTING,
    SER_STATE_ENDING,
    SER_STATE_ERROR
  };
enum SER_User_Mode
  {
    SER_USER_MODE_NORMAL,
    SER_USER_MODE_DEBUG
  };
/* _____CLASS DEFINITION_____________________________________________________ */
/**
 * This Service class is base class for implement another class by inheritance 
 */
class Service
{
public:
  Service();
  
  void      Run_Service(bool autoRun);
  // 
  char*     Name_Service = (char*)"Service Name";
  uint16_t  ID_Service = 0;

  // user can existing state of app by change Service_State into below state
 
  uint8_t Service_State = SER_STATE_STARTING;
  
  // enter task debug
  void Debug();
  // exit task debug
  void Debug_Exit();
  // 
  uint8_t User_Mode =  SER_USER_MODE_NORMAL;
  // this var will be set to 1 whenever ser done a state execution 
  bool Step_Forward = 0;
protected:
  void (*_Start_User)();
  void (*_Execute_User)();
  void (*_End_User)();
  
  uint8_t   State_Service();
  char*     State_Service_String();
  void      Information();  
private:
  bool Run_Mode = Run_Mode_Auto;// the executing task will run APP_RUN_MODE_AUTOally

  static const bool Run_Mode_Auto = 1;
  static const bool Run_Mode_Manual = 0;

  //  Tasks default of Service
  void Start(void);
  void Execute(void);
  void End(void);
};

/**
 */
Service::Service(void)
{
  _Start_User   = 0;
  _Execute_User = 0;
  _End_User     = 0;
}
/**
 * Enter debug mode
 */
void Service::Debug()
{
  User_Mode =  SER_USER_MODE_DEBUG;
}
/**
 * exit debug mode
 */
void Service::Debug_Exit()
{
  User_Mode = SER_USER_MODE_NORMAL;
}

/**
 * The main task of Service, need to bee call regularly for attack like a 
 * task in freertos. There are 3 state of an Service.  Starting is initing the prerequisite condition to
 * execute. Executing state is main state to hanble the task of Service. Service will finish all task and save all
 * parameter, stop and remove all parameter. Error is the indicator for user when the 
 * Service is executing wrongly.
 * @param autoRun  Service_RUN_MODE_AUTO for auto execute and Service_RUN_MODE_MANUAL for wait for execute manually by user
 */
void Service::Run_Service(bool autoRun = SER_RUN_MODE_AUTO)
{
  switch (Service_State)
  {
    // starting the task
    case SER_STATE_STARTING:
      Run_Mode = autoRun;
      Start();
      break;

    //the task is executing 
    case SER_STATE_EXECUTING:
      Execute();
      break;

    // the task is been ending
    case SER_STATE_ENDING:
      End();
      break;

    // default
    default:

      break;
  }
}
/**
 * This function will return a unsigned integer of Service state at the present 
 * @return Service_State the
 */
uint8_t Service::State_Service()
{
  return Service_State;
}
char* Service::State_Service_String()
{
  switch (Service_State)
  {
  case SER_STATE_STARTING:
    return (char*)"Starting State";
    break;
  case SER_STATE_EXECUTING:
    return (char*)"Executing State";
    break;
  case SER_STATE_ENDING:
    return (char*)"Ending State";
    break;
  default:
    return (char*)"Error state";
    break;
  }
}

/**
 *  start is the first task of this Service it will do prerequisite
 *  condition. In the debug mode, task will send information of Service to 
 * terminal to start the Service.
 * @param (*_End_User)() this function pointer will be define by user
 * @param User_Mode will be specified by user
 */
void Service::Start()
{
  if (User_Mode == SER_USER_MODE_DEBUG) Information();
  // the user function
  (*_Start_User)();
  if (User_Mode == SER_USER_MODE_DEBUG) 
  {
    // if the serial port is not open, thus open it
    if(!Serial_Hardware_Port_Is_Opened) 
    {
      Serial.begin(Serial_Hardware_Port_Baudrate);
      Serial_Hardware_Port_Is_Opened = 1;
    }
    // Information();
  }
  // if(Run_Mode == SER_RUN_MODE_AUTO)
  // {
    Service_State = SER_STATE_EXECUTING;
  // }
  Step_Forward = 1;
}
/**
 *  Pend to start is the first task of this Service it will do prerequisite
 *  condition. In the debug mode, task will send Information of Service to 
 * terminal to start the Service.
 * @param (*_End_User)() this function pointer will be define by user
 * @param User_Mode will be specified by user
 */
void Service::Execute()
{
  if (User_Mode == SER_USER_MODE_DEBUG) Information();
  // the user function
  (*_Execute_User)();
  Step_Forward = 1;
}

/**
 *  Pend to start is the first task of this Service it will do prerequisite
 *  condition. In the debug mode, task will send information of Service to 
 * terminal to start the Service.
 * @param (*_End_User)() this function pointer will be define by user
 * @param User_Mode will be specified by user
 */
void Service::End()
{
  if (User_Mode == SER_USER_MODE_DEBUG) Information();
  // the user function
  (*_End_User)();
  Step_Forward = 1;
}

/**
 * Print the the Information of Service. These informations are ID, name, state 
 * of Service and the remaining free head size. The Informationmation will be 
 * write to the Serial terminal
 */
void Service::Information()
{
  // Serial.printf_P("   ID: %d -- ",ID_Service);
  // Serial.printf_P(Name_Service);
  // Serial.printf_P(" : ");
  // Serial.printf_P(State_Service_String());
  // Serial.printf_P(" -- Free heap size %d.\n",ESP.getFreeHeap());
}
#endif