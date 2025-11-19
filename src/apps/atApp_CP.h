#ifndef _Application_atApp_CP_
#define _Application_atApp_CP_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "App.h"
#include "../src/apps/atApp_BT.h"
#include "../src/apps/atApp_AI.h"
#include "../src/apps/atApp_AP.h"
#include "../src/apps/atApp_DI.h"
#include "../src/apps/atApp_DO.h"
#include "../src/apps/atApp_LED.h"
#include "../src/apps/atApp_LCD.h"
#include "../src/apps/atApp_Sensor.h"
#include "../src/apps/atApp_MB1.h"
#include "../src/apps/atApp_MB2.h"
#include "../src/apps/atApp_Sensor.h"
#include "../src/apps/atApp_RTC_v2.h"
#include "../src/apps/atApp_FTP.h"
#include "../src/apps/atApp_Logger.h"
#include "../src/apps/atApp_ParamSync.h"
// #include "../src/apps/atApp_WD.h"
// #include "../src/apps/atApp_TestSD.h"
// #include "../src/apps/atApp_TestLCD.h"
// #include "../src/apps/atApp_MQTT_PUB.h"
// #include "../src/apps/atApp_MQTT_SUB.h"
// #include "../src/apps/atApp_RTC.h"
// #include "../src/apps/atApp_TestFTP.h"
// #include "../src/apps/atApp_TestI2CScan.h"
#include "../src/obj/atObj_Param.h"
#include "../src/obj/atObj_Data.h"
/* _____DEFINITIONS__________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
TaskHandle_t Task_atApp_CP;
void atApp_CP_Task_Func(void *parameter);
/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */

/**
 * This Application class is the application to manage the
 */
class App_CP : public Application
{
public:
	App_CP();
	~App_CP();

protected:
private:
	static void App_CP_Pend();
	static void App_CP_Start();
	static void App_CP_Restart();
	static void App_CP_Execute();
	static void App_CP_Suspend();
	static void App_CP_Resume();
	static void App_CP_End();
} atApp_CP;
/**
 * This function will be automaticaly called when a object is created by this class
 */
App_CP::App_CP(/* args */)
{
	_Pend_User = *App_CP_Pend;
	_Start_User = *App_CP_Start;
	_Restart_User = *App_CP_Restart;
	_Execute_User = *App_CP_Execute;
	_Suspend_User = *App_CP_Suspend;
	_Resume_User = *App_CP_Resume;
	_End_User = *App_CP_End;

	// change the ID of application
	ID_Application = 1;
	// change the application name
	Name_Application = (char *)"CP Application	";
	// change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
App_CP::~App_CP()
{
}
/**
 * Pend to start is the first task of this application it will do prerequisite condition. In the debit mode, task will send information of application to terminal to start the application.
 */
void App_CP::App_CP_Pend()
{
	// atService_XYZ.Debug();
}
/**
 * This start function will init some critical function
 */
void App_CP::App_CP_Start()
{
}
/**
 * Restart function of SNM  app
 */
void App_CP::App_CP_Restart()
{
}
/**
 * Execute fuction of SNM app
 */
void App_CP::App_CP_Execute()
{
	// check button press
	if (atObject_Data.flagButtonHeldSecond[12])
	{
		if (atApp_CP.User_Mode == APP_USER_MODE_DEBUG)
        {
            Serial.println("\t[CP] Button held for 5 seconds. Starting AP mode...");
            Serial.printf("\t[CP] Free heap before suspend: %u bytes\n", esp_get_free_heap_size());
			
			// Check initial state of AP task
			if (Task_atApp_AP != NULL)
			{
				eTaskState apTaskState = eTaskGetState(Task_atApp_AP);
				Serial.printf("\t[CP] AP task initial state: %d\n", apTaskState);
			}
			else
			{
				Serial.println("\t[CP] ⚠️ AP task handle is NULL!");
			}
        }

        // Suspend all tasks except Watchdog and CP
        // if (Task_atApp_RTC_v2) 	vTaskDelete(Task_atApp_RTC_v2);
        // if (Task_atApp_AI) 		vTaskDelete(Task_atApp_AI);
        // if (Task_atApp_LCD) 	vTaskDelete(Task_atApp_LCD);
        if (Task_atApp_MB1) 	vTaskDelete(Task_atApp_MB1);
        if (Task_atApp_MB2) 	vTaskDelete(Task_atApp_MB2);
        // if (Task_atApp_Sensor) 	vTaskDelete(Task_atApp_Sensor);
        // if (Task_atApp_DI) 		vTaskDelete(Task_atApp_DI);
        // if (Task_atApp_DO) 		vTaskDelete(Task_atApp_DO);
        // if (Task_atApp_LED) 	vTaskDelete(Task_atApp_LED);
        // if (Task_atApp_BT) 		vTaskDelete(Task_atApp_BT);
        if (Task_atApp_Logger) 	vTaskDelete(Task_atApp_Logger);
        if (Task_atApp_FTP) 	vTaskDelete(Task_atApp_FTP);

		// Note: AP task is already suspended by default in main.cpp
		// No need to suspend it again here

		
        // Reset flag
        atObject_Data.flagButtonHeldSecond[12] = false;

        if (atApp_CP.User_Mode == APP_USER_MODE_DEBUG)
        {
            Serial.println("\t[CP] All tasks suspended successfully");
            Serial.printf("\t[CP] Free heap after suspend: %u bytes\n", esp_get_free_heap_size());
            Serial.printf("\t[CP] Free PSRAM: %u bytes\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
			Serial.println("\t[CP] Resuming AP task...");
        }
		// delay for a while to let system stabilize
		vTaskDelay(2000 / portTICK_PERIOD_MS);
		
		// Check if AP task exists before resuming
		if (Task_atApp_AP != NULL)
		{
			// Check task state before resuming
			eTaskState taskState = eTaskGetState(Task_atApp_AP);
			if (taskState == eSuspended)
			{
				vTaskResume(Task_atApp_AP);
				if (atApp_CP.User_Mode == APP_USER_MODE_DEBUG)
				{
					Serial.println("\t[CP] ✅ AP task resumed successfully");
					Serial.printf("\t[CP] Available heap after resuming AP: %u bytes\n", esp_get_free_heap_size());
				}
			}
			else
			{
				if (atApp_CP.User_Mode == APP_USER_MODE_DEBUG)
				{
					Serial.printf("\t[CP] ⚠️ AP task is not suspended (state: %d), cannot resume\n", taskState);
				}
			}
		}
		else
		{
			if (atApp_CP.User_Mode == APP_USER_MODE_DEBUG)
			{
				Serial.println("\t[CP] ❌ AP task handle is NULL, cannot resume");
			}
			BaseType_t  result = xTaskCreatePinnedToCore(atApp_AP_Task_Func, "atApp_AP_Application", 81920, NULL, 4, &Task_atApp_AP, 0);
  			Serial.printf("[TASK] AP Application: %s (Handle: %p)\n", result == pdPASS ? "✅ SUCCESS" : "❌ FAILED", Task_atApp_AP);

		}

	}
	
	// for working mode
	switch (atObject_Data.working_mode)
	{
	case WORKING_MODE_CONFIG:
		// Turn on
		break;
	case WORKING_MODE_DEBUG_ALL:
		// turn on all apps to debug mode
		atApp_AI.Debug();
		atApp_AP.Debug();
		atApp_BT.Debug();
		atApp_CP.Debug();
		atApp_DI.Debug();
		atApp_DO.Debug();
		atApp_FTP.Debug();
		atApp_LCD.Debug();
		atApp_LED.Debug();
		atApp_Logger.Debug();
		atApp_MB1.Debug();
		atApp_MB2.Debug();
		atApp_RTC_v2.Debug();
		atApp_Sensor.Debug();
		// atApp_WD.Debug();

		break;
	case WORKING_MODE_DEBUG_AI:
		atApp_AI.Debug();
		break;
	case WORKING_MODE_DEBUG_AP:
		atApp_AP.Debug();
		break;
	case WORKING_MODE_DEBUG_BT:
		atApp_BT.Debug();
		break;
	case WORKING_MODE_DEBUG_CP:
		atApp_CP.Debug();
		break;
	case WORKING_MODE_DEBUG_DI:
		atApp_DI.Debug();
		break;
	case WORKING_MODE_DEBUG_DO:
		atApp_DO.Debug();
		break;
	case WORKING_MODE_DEBUG_FTP:
		atApp_FTP.Debug();
		break;
	case WORKING_MODE_DEBUG_LCD:
	
	default:
		break;
	}

	if (atApp_CP.User_Mode == APP_USER_MODE_DEBUG)
	{
		Serial.printf("\t[CP] Free heap : %u bytes\n", esp_get_free_heap_size());
		Serial.printf("\t[CP] Free PSRAM: %u bytes\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
	}
}
void App_CP::App_CP_Suspend() {}
void App_CP::App_CP_Resume() {}
void App_CP::App_CP_End() {}
void atApp_CP_Task_Func(void *parameter)
{
	while (1)
	{
		atApp_CP.Run_Application(APP_RUN_MODE_AUTO);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
#endif