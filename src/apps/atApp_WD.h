#ifndef _Application_atApp_WD_
#define _Application_atApp_WD_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "App.h"
#include "../src/obj/atObj_Param.h"
#include "../src/obj/atObj_Data.h"
#include "esp_task_wdt.h"
// #include "../src/apps/atApp_Database.h"
/* _____DEFINITIONS__________________________________________________________ */
#define WATCHDOG_TIMEOUT_SECONDS 10
/* _____GLOBAL VARIABLES_____________________________________________________ */
TaskHandle_t Task_atApp_WD;  
void atApp_WD_Task_Func(void *parameter);
///////////////////////////////////////////////Testing part//
/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */

/**
 * This Application class is the application to manage the 
 */
class App_WD : public Application
{
public:
  	App_WD();
 	~App_WD();
protected:
private:
  	static void  App_WD_Pend();
	static void  App_WD_Start();
	static void  App_WD_Restart();
	static void  App_WD_Execute();
	static void  App_WD_Suspend();
	static void  App_WD_Resume();	  
	static void  App_WD_End();
} atApp_WD ;
/**
 * This function will be automaticaly called when a object is created by this class
 */
App_WD::App_WD(/* args */)
{
  	_Pend_User 	     = *App_WD_Pend;
	_Start_User 	 = *App_WD_Start;
	_Restart_User 	 = *App_WD_Restart;
	_Execute_User 	 = *App_WD_Execute;
	_Suspend_User	 = *App_WD_Suspend;
	_Resume_User	 = *App_WD_Resume;
	_End_User	     = *App_WD_End;

	// change the ID of application
	ID_Application = 1;
	// change the application name
	Name_Application = (char*)"WD Application	";
	// change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
App_WD::~App_WD()
{
	
}
/**
 * Pend to start is the first task of this application it will do prerequisite condition. In the debit mode, task will send information of application to terminal to start the application.
 */
void  App_WD::App_WD_Pend()
{
    // atService_XYZ.Debug();
	
	// For initial setup, let's disable the watchdog temporarily
	// This prevents conflicts during system initialization
	
	if(atApp_WD.User_Mode == APP_USER_MODE_DEBUG)
    {
		Serial.println("\tWatchdog App_WD_Pend: Initializing...");
    }
	
	// Try to deregister the current task from WDT monitoring if it exists
	esp_err_t delete_result = esp_task_wdt_delete(NULL);
	if (delete_result != ESP_OK && delete_result != ESP_ERR_NOT_FOUND) {
		if(atApp_WD.User_Mode == APP_USER_MODE_DEBUG)
		{
			Serial.printf("\tWatchdog delete failed: 0x%x\n", delete_result);
		}
	}

	// Deinitialize existing WDT if any
	esp_err_t deinit_result = esp_task_wdt_deinit();
	if (deinit_result != ESP_OK && deinit_result != ESP_ERR_INVALID_STATE) {
		if(atApp_WD.User_Mode == APP_USER_MODE_DEBUG)
		{
			Serial.printf("\tWatchdog deinit failed: 0x%x\n", deinit_result);
		}
	}

	//  create esp_task_wdt_config_t
	esp_task_wdt_config_t wdt_config = {
		.timeout_ms = WATCHDOG_TIMEOUT_SECONDS * 1000,
		.idle_core_mask = 0, // Don't monitor idle tasks initially
		.trigger_panic = false, // Don't panic on timeout, just reset
	};

	// Initialize the WDT with a timeout of 10 seconds 
	esp_err_t init_result = esp_task_wdt_init(&wdt_config);
	if (init_result != ESP_OK) {
		if(atApp_WD.User_Mode == APP_USER_MODE_DEBUG)
		{
			Serial.printf("\tWatchdog init failed: 0x%x\n", init_result);
		}
		return; // Skip registration if init failed
	}

	// Register the current task to be monitored by WDT
	esp_err_t add_result = esp_task_wdt_add(NULL);
	if (add_result != ESP_OK) {
		if(atApp_WD.User_Mode == APP_USER_MODE_DEBUG)
		{
			Serial.printf("\tWatchdog add failed: 0x%x\n", add_result);
		}
	}
	
	if(atApp_WD.User_Mode == APP_USER_MODE_DEBUG)
    {
		Serial.println("\tWatchdog App_WD_Pend: Completed");
    }
}
/**
 * This start function will init some critical function 
 */
void  App_WD::App_WD_Start()
{
	// init atXYZ Service in the fist running time
	// atService_XYZ.Run_Service();
	esp_task_wdt_reset();
}  
/**
 * Restart function of SNM  app
 */
void  App_WD::App_WD_Restart()
{
	esp_task_wdt_reset();
}
/**
 * Execute fuction of SNM app
 */
void  App_WD::App_WD_Execute()
{	
	// atService_XYZ.Run_Service();
	esp_task_wdt_reset();
    if(atApp_WD.User_Mode == APP_USER_MODE_DEBUG)
    {
		Serial.println("\tWatchdog reset");
    }   
}
void  App_WD::App_WD_Suspend(){}
void  App_WD::App_WD_Resume(){}	  
void  App_WD::App_WD_End(){}
void atApp_WD_Task_Func(void *parameter)
{
  while (1)
  {
    atApp_WD.Run_Application(APP_RUN_MODE_AUTO);
    vTaskDelay(WATCHDOG_TIMEOUT_SECONDS*333/ portTICK_PERIOD_MS);
  }
}
#endif