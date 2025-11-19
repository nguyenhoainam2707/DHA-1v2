#ifndef _Object_
#define _Object_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "Arduino.h"
/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____DEFINITIONS__________________________________________________________ */
 enum OBJ_State
  {
    OBJ_STATE_STARTING,
    OBJ_STATE_EXECUTING,
    OBJ_STATE_ENDING,
    OBJ_STATE_ERROR
  };
enum OBJ_User_Mode
  {
    OBJ_USER_MODE_NORMAL,
    OBJ_USER_MODE_DEBUG
  };
/* _____CLASS _____DEFINITIONS__________________________________________________________ */
/**
 * This Object class is base class for implement another class by inheritance 
 */
class Object
{
public:
  Object();
  void      Run_Object();
  // user can existing state of app by change Object_State into below state
 
  uint8_t Object_State = OBJ_STATE_STARTING;
  bool Step_Forward = 0;
protected:
  void (*_Start_User)();
  void (*_Execute_User)();
  void (*_End_User)();
  
  uint8_t   State_Object();
  char*     State_Object_String();
private:
  static const bool Run_Mode_Auto = 1;
  //  Tasks default of Object
  void Start(void);
  void Execute(void);
  void End(void);
};

/**
 */
Object::Object(void)
{
  _Start_User   = 0;
  _Execute_User = 0;
  _End_User     = 0;
}


/**
 * The main task of Object, need to bee call regularly for attack like a 
 * task in freertos. There are 3 state of an Object.  Starting is initting the prerequisite condition to
 * execute. Executing state is main state to handle the task of Object. Object will finish all task and save all
 * parameter, stop and remove all parameter. Error is the indicator for user when the 
 * Object is executing wrongly.
 * @param autoRun  Object_RUN_MODE_AUTO for auto execute and Object_RUN_MODE_MANUAL for wait for execute manually by user
 */
void Object::Run_Object()
{
  switch (Object_State)
  {
    // starting the task
    case OBJ_STATE_STARTING:
      Start();
      break;

    //the task is executing 
    case OBJ_STATE_EXECUTING:
      Execute();
      break;

    // the task is been ending
    case OBJ_STATE_ENDING:
      End();
      break;

    // default
    default:

      break;
  }
}
/**
 * This function will return a unsigned integer of Object state at the present 
 * @return Object_State the
 */
uint8_t Object::State_Object()
{
  return Object_State;
}
char* Object::State_Object_String()
{
  switch (Object_State)
  {
  case OBJ_STATE_STARTING:
    return (char*)"Starting State";
    break;
  case OBJ_STATE_EXECUTING:
    return (char*)"Executing State";
    break;
  case OBJ_STATE_ENDING:
    return (char*)"Ending State";
    break;
  default:
    return (char*)"Error state";
    break;
  }
}
/**
 *  start is the First task of this Object it will do prerequisite
 *  condition. In the debug mode, task will send information of Object to 
 * terminal to start the Object.
 * @param (*_End_User)() this function pointer will be define by user
 * @param User_Mode will be specified by user
 */
void Object::Start()
{
  // the user function
  (*_Start_User)();
  Object_State = OBJ_STATE_EXECUTING;
  Step_Forward = 1;
}
/**
 *  Pend to start is the first task of this Object it will do prerequisite
 *  condition. In the debug mode, task will send information of Object to 
 * terminal to start the Object.
 * @param (*_End_User)() this function pointer will be define by user
 * @param User_Mode will be specified by user
 */
void Object::Execute()
{
  
  // the user function
  (*_Execute_User)();
  Step_Forward = 1;
}

/**
 *  Pend to start is the first task of this Object it will do prerequisite
 *  condition. In the debug mode, task will send information of Object to 
 * terminal to start the Object.
 * @param (*_End_User)() this function pointer will be define by user
 * @param User_Mode will be specified by user
 */
void Object::End()
{
  // the user function
  (*_End_User)();
  Step_Forward = 1;
}

#endif