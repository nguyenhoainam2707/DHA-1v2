#ifndef _Application_atApp_TestI2CScan_
#define _Application_atApp_TestI2CScan_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "App.h"
#include <Wire.h>
#include "../src/obj/atObj_Param.h"
#include "../src/obj/atObj_Data.h"
// #include "../src/apps/atApp_Database.h"
/* _____DEFINITIONS__________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
TaskHandle_t Task_atApp_TestI2CScan;
void atApp_TestI2CScan_Task_Func(void *parameter);
///////////////////////////////////////////////Testing part//
/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */

/**
 * This Application class is the application to manage the
 */
class App_TestI2CScan : public Application
{
public:
	App_TestI2CScan();
	~App_TestI2CScan();

protected:
private:
	static void App_TestI2CScan_Pend();
	static void App_TestI2CScan_Start();
	static void App_TestI2CScan_Restart();
	static void App_TestI2CScan_Execute();
	static void App_TestI2CScan_Suspend();
	static void App_TestI2CScan_Resume();
	static void App_TestI2CScan_End();
} atApp_TestI2CScan;
/**
 * This function will be automaticaly called when a object is created by this class
 */
App_TestI2CScan::App_TestI2CScan(/* args */)
{
	_Pend_User = *App_TestI2CScan_Pend;
	_Start_User = *App_TestI2CScan_Start;
	_Restart_User = *App_TestI2CScan_Restart;
	_Execute_User = *App_TestI2CScan_Execute;
	_Suspend_User = *App_TestI2CScan_Suspend;
	_Resume_User = *App_TestI2CScan_Resume;
	_End_User = *App_TestI2CScan_End;

	// change the ID of application
	ID_Application = 1;
	// change the application name
	Name_Application = (char *)"TestI2CScan Application	";
	// change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
App_TestI2CScan::~App_TestI2CScan()
{
}
/**
 * Pend to start is the first task of this application it will do prerequisite condition. In the debit mode, task will send information of application to terminal to start the application.
 */
void App_TestI2CScan::App_TestI2CScan_Pend()
{
	// atService_XYZ.Debug();
}
/**
 * This start function will init some critical function
 */
void App_TestI2CScan::App_TestI2CScan_Start()
{
	// init atXYZ Service in the fist running time
	// atService_XYZ.Run_Service();
	Wire.begin(48, 45);
}
/**
 * Restart function of SNM  app
 */
void App_TestI2CScan::App_TestI2CScan_Restart()
{
}
/**
 * Execute fuction of SNM app
 */
void App_TestI2CScan::App_TestI2CScan_Execute()
{
	// atService_XYZ.Run_Service();
	if (atApp_TestI2CScan.User_Mode == APP_USER_MODE_DEBUG)
	{
		byte error, address;
		int nDevices = 0;

		delay(5000);

		Serial.println("Scanning for I2C devices ...");
		for (address = 0x01; address < 0x7f; address++)
		{
			Wire.beginTransmission(address);
			error = Wire.endTransmission();
			if (error == 0)
			{
				Serial.printf("I2C device found at address 0x%02X\n", address);
				nDevices++;
			}
			else if (error != 2)
			{
				Serial.printf("Error %d at address 0x%02X\n", error, address);
			}
		}
		if (nDevices == 0)
		{
			Serial.println("No I2C devices found");
		}
	}
}
void App_TestI2CScan::App_TestI2CScan_Suspend() {}
void App_TestI2CScan::App_TestI2CScan_Resume() {}
void App_TestI2CScan::App_TestI2CScan_End() {}
void atApp_TestI2CScan_Task_Func(void *parameter)
{
	while (1)
	{
		atApp_TestI2CScan.Run_Application(APP_RUN_MODE_AUTO);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
#endif