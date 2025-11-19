#ifndef _Application_atApp_ABC_
#define _Application_atApp_ABC_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "App.h"
#include "../src/obj/atObj_Param.h"
#include "../src/obj/atObj_Data.h"
// #include "../src/apps/atApp_Database.h"
/* _____DEFINITIONS__________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
TaskHandle_t Task_atApp_ABC;  
void atApp_ABC_Task_Func(void *parameter);
///////////////////////////////////////////////Testing part//
/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */

/**
 * This Application class is the application to manage the 
 */
class App_ABC : public Application
{
public:
  	App_ABC();
 	~App_ABC();
protected:
private:
  	static void  App_ABC_Pend();
	static void  App_ABC_Start();
	static void  App_ABC_Restart();
	static void  App_ABC_Execute();
	static void  App_ABC_Suspend();
	static void  App_ABC_Resume();	  
	static void  App_ABC_End();
} atApp_ABC ;
/**
 * This function will be automaticaly called when a object is created by this class
 */
App_ABC::App_ABC(/* args */)
{
  	_Pend_User 	     = *App_ABC_Pend;
	_Start_User 	 = *App_ABC_Start;
	_Restart_User 	 = *App_ABC_Restart;
	_Execute_User 	 = *App_ABC_Execute;
	_Suspend_User	 = *App_ABC_Suspend;
	_Resume_User	 = *App_ABC_Resume;
	_End_User	     = *App_ABC_End;

	// change the ID of application
	ID_Application = 1;
	// change the application name
	Name_Application = (char*)"ABC Application	";
	// change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
App_ABC::~App_ABC()
{
	
}
/**
 * Pend to start is the first task of this application it will do prerequisite condition. In the debit mode, task will send information of application to terminal to start the application.
 */
void  App_ABC::App_ABC_Pend()
{
    // atService_XYZ.Debug();
}
/**
 * This start function will init some critical function 
 */
void  App_ABC::App_ABC_Start()
{
	// init atXYZ Service in the fist running time
	// atService_XYZ.Run_Service();
}  
/**
 * Restart function of SNM  app
 */
void  App_ABC::App_ABC_Restart()
{

}
/**
 * Execute fuction of SNM app
 */
void  App_ABC::App_ABC_Execute()
{	
	// atService_XYZ.Run_Service();
    if(atApp_ABC.User_Mode == APP_USER_MODE_DEBUG)
    {
		
    }   
}
void  App_ABC::App_ABC_Suspend(){}
void  App_ABC::App_ABC_Resume(){}	  
void  App_ABC::App_ABC_End(){}
void atApp_ABC_Task_Func(void *parameter)
{
  while (1)
  {
    atApp_ABC.Run_Application(APP_RUN_MODE_AUTO);
    vTaskDelay(1000/ portTICK_PERIOD_MS);
  }
}
#endif