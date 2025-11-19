#ifndef _Application_atApp_DI_
#define _Application_atApp_DI_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "App.h"
#include "../src/obj/atObj_Param.h"
#include "../src/obj/atObj_Data.h"
#include "../src/services/atService_MCP23008.h"
/* _____DEFINITIONS__________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
TaskHandle_t Task_atApp_DI;
void atApp_DI_Task_Func(void *parameter);
///////////////////////////////////////////////Testing part//
/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */

/**
 * This Application class is the application to manage the
 */
class App_DI : public Application
{
public:
	App_DI();
	~App_DI();
	Service_MCP23008 atService_MCP23008;
protected:
private:
	static void App_DI_Pend();
	static void App_DI_Start();
	static void App_DI_Restart();
	static void App_DI_Execute();
	static void App_DI_Suspend();
	static void App_DI_Resume();
	static void App_DI_End();
} atApp_DI;
/**
 * This function will be automaticaly called when a object is created by this class
 */
App_DI::App_DI(/* args */)
{
	_Pend_User = *App_DI_Pend;
	_Start_User = *App_DI_Start;
	_Restart_User = *App_DI_Restart;
	_Execute_User = *App_DI_Execute;
	_Suspend_User = *App_DI_Suspend;
	_Resume_User = *App_DI_Resume;
	_End_User = *App_DI_End;

	// change the ID of application
	ID_Application = 1;
	// change the application name
	Name_Application = (char *)"DI Application	";
	// change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
App_DI::~App_DI()
{
}
/**
 * Pend to start is the first task of this application it will do prerequisite condition. In the debit mode, task will send information of application to terminal to start the application.
 */
void App_DI::App_DI_Pend()
{
	// atService_XYZ.Debug();
}
/**
 * This start function will init some critical function
 */
void App_DI::App_DI_Start()
{
	// init atXYZ Service in the fist running time
	// atService_XYZ.Run_Service();
	atApp_DI.atService_MCP23008.Service_MCP23008_Start();

	// config GP0–GP3 as input (có pull-up)
	atService_I2C.checkIn();
	for (uint8_t i = 0; i < 4; i++)
	{
		atApp_DI.atService_MCP23008.pinMode(i, INPUT);
	}
	atService_I2C.checkOut();
}
/**
 * Restart function of SNM  app
 */
void App_DI::App_DI_Restart()
{
}
/**
 * Execute fuction of SNM app
 */
void App_DI::App_DI_Execute()
{
	// read input status from MCP23008 and store to atObject_Data.di_input_list
	atService_I2C.checkIn();
	for (uint8_t i = 0; i < 4; i++)
	{
		atObject_Data.di_state[3-i] = atApp_DI.atService_MCP23008.digitalRead(i); // Active LOW
	}
	atService_I2C.checkOut();

	// handle read state for sensor
	for (uint8_t di_index = 0; di_index < MAX_DI_CHANNEL; di_index++)
	{
		if (atObject_Param.di_enable_list[di_index] == true &&
			atObject_Param.di_control_sensor_state_enable_list[di_index] == true)
		{
			uint8_t sensor_index = atObject_Param.di_control_sensor_list[di_index];
			uint8_t desired_sensor_state = atObject_Param.di_control_sensor_state_list[di_index];
			if (atObject_Param.sensor_state_protocol_list[sensor_index] == PROTOCOL_STATE_READING_DIGITAL_INPUT)
				if (atObject_Data.di_state[di_index]) // DI is On
					atObject_Data.sensor_status_list[sensor_index] = desired_sensor_state;				
				else // DI is Off
					atObject_Data.sensor_status_list[sensor_index] = SENSOR_STATE_NORMAL;
			

				
		}
	}

	if (atApp_DI.User_Mode == APP_USER_MODE_DEBUG)
	{
		Serial.println("\tDI Inputs status:");
		for (uint8_t i = 0; i < 4; i++)
		{
			Serial.print("  DI");
			Serial.print(i+1);
			Serial.print(": ");
			Serial.println(atObject_Data.di_state[i] ? "ON" : "OFF");
		}
	}
}
void App_DI::App_DI_Suspend() {}
void App_DI::App_DI_Resume() {}
void App_DI::App_DI_End() {}
void atApp_DI_Task_Func(void *parameter)
{
	while (1)
	{
		atApp_DI.Run_Application(APP_RUN_MODE_AUTO);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
#endif