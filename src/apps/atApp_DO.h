#ifndef _Application_atApp_DO_
#define _Application_atApp_DO_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "App.h"
#include "../services/atService_MCP23008.h"
/* _____DEFINITIONS__________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
TaskHandle_t Task_atApp_DO;
void atApp_DO_Task_Func(void *parameter);
///////////////////////////////////////////////Testing part//
/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */

/**
 * This Application class is the application to manage the
 */
class App_DO : public Application
{
public:
	App_DO();
	~App_DO();
	Service_MCP23008 atService_MCP23008;

protected:
private:
	static void App_DO_Pend();
	static void App_DO_Start();
	static void App_DO_Restart();
	static void App_DO_Execute();
	static void App_DO_Suspend();
	static void App_DO_Resume();
	static void App_DO_End();
	static bool control_DO_from_sensor_schmitt_trigger(uint8_t do_index);
	static bool control_DO_from_sensor_state(uint8_t do_index);
	// static bool control_DO_from_mqtt(uint8_t do_index);
	static bool control_DO_from_digital_input(uint8_t do_index);
} atApp_DO;
/**
 * This function will be automaticaly called when a object is created by this class
 */
App_DO::App_DO(/* args */)
{
	_Pend_User = *App_DO_Pend;
	_Start_User = *App_DO_Start;
	_Restart_User = *App_DO_Restart;
	_Execute_User = *App_DO_Execute;
	_Suspend_User = *App_DO_Suspend;
	_Resume_User = *App_DO_Resume;
	_End_User = *App_DO_End;

	// change the ID of application
	ID_Application = 1;
	// change the application name
	Name_Application = (char *)"DO Application	";
	// change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
App_DO::~App_DO()
{
}
/**
 * Pend to start is the first task of this application it will do prerequisite condition. In the debit mode, task will send information of application to terminal to start the application.
 */
void App_DO::App_DO_Pend()
{
	// atService_MCP23008.Debug();
}
/**
 * This start function will init some critical function
 */
void App_DO::App_DO_Start()
{
	// init atMCP23008 Service in the fist running time
	// atService_MCP23008.Run_Service();
	atApp_DO.atService_MCP23008.Service_MCP23008_Start();

	// Config GP4–GP7 as output and set LOW
	atService_I2C.checkIn();
	for (uint8_t i = 4; i < 8; i++)
	{
		atApp_DO.atService_MCP23008.pinMode(i, OUTPUT);
		atApp_DO.atService_MCP23008.digitalWrite(i, LOW);
	}
	atService_I2C.checkOut();
}
/**
 * Restart function of SNM  app
 */
void App_DO::App_DO_Restart()
{
}
/**
 * Execute fuction of SNM app
 */
void App_DO::App_DO_Execute()
{
	// atService_MCP23008.Run_Service();
	// read output status from atObject_Data.do_output_list and set outputs
	atService_I2C.checkIn();
	for (uint8_t i = 0; i < 4; i++)
	{
		atApp_DO.atService_MCP23008.digitalWrite(i + 4, !atObject_Data.do_state[i]);
	}
	atService_I2C.checkOut();

	// check and control DO
	for (uint8_t i = 0; i < MAX_DO_CHANNEL; i++)
	{
		if (atObject_Param.do_enable_list[i])
		{

			switch (atObject_Param.do_protocol_list[i])
			{
			case PROTOCOL_DO_SENSOR_SCHMITT_TRIGGER:
				atObject_Data.do_state[i] = atApp_DO.control_DO_from_sensor_schmitt_trigger(i);
				break;
			case PROTOCOL_DO_SENSOR_STATE:
				atObject_Data.do_state[i] = atApp_DO.control_DO_from_sensor_state(i);
				break;
			case PROTOCOL_DO_INPUT_DIGITAL:
				atObject_Data.do_state[i] = atApp_DO.control_DO_from_digital_input(i);
				break;
			// case PROTOCOL_DO_MQTT:

			// 	// to be implemented
			// 	break;

			default:
				break;
			}
		}
		else
		{
			atObject_Data.do_state[i] = false; // disable DO channel, set to OFF
		}
	}
	if (atApp_DO.User_Mode == APP_USER_MODE_DEBUG)
	{
		Serial.println("\tDO Outputs updated:");
		for (uint8_t i = 0; i < 4; i++)
		{
			Serial.print("  DO");
			Serial.print(i);
			Serial.print(": ");
			Serial.println(atObject_Data.do_state[i] ? "ON" : "OFF");
		}
	}
}
void App_DO::App_DO_Suspend() {}
void App_DO::App_DO_Resume() {}
void App_DO::App_DO_End() {}
bool App_DO::control_DO_from_sensor_schmitt_trigger(uint8_t do_index)
{
	// Serial.printf("Control DO %d from schmitt trigger method\r\n", do_index);
	// check if do_index is valid
	if (do_index >= MAX_DO_CHANNEL)
	{
		return false;
	}

	// get the corresponding sensor index from do_index
	uint8_t sensor_index = atObject_Param.do_control_schmitt_trigger_sensor_list[do_index]; // assuming 1-to-1 mapping for simplicity
	// check if sensor_index is valid
	if (sensor_index >= MAX_SENSORS)
	{
		return false;
	}
	// get the schmitt trigger thresholds from atObject_Param
	float upper_threshold = atObject_Param.do_schmitt_trigger_upper_threshold_list[do_index];
	float lower_threshold = atObject_Param.do_schmitt_trigger_lower_threshold_list[do_index];
	// get the current sensor value from atObject_Data
	float sensor_value = 0;
	switch (atObject_Param.do_control_schmitt_trigger_sensor_valuetype_list[do_index])
	{
	case SENSOR_VALUE_TYPE_RAW:
		sensor_value = atObject_Data.sensor_raw_value_list[sensor_index];
		break;
	case SENSOR_VALUE_TYPE_CALIB:
		sensor_value = atObject_Data.sensor_calib_value_list[sensor_index];
		break;
	case SENSOR_VALUE_TYPE_SUM:
		sensor_value = atObject_Data.sensor_sum_value_list[sensor_index];
		break;
	default:
		break;
	}
	// print sensor value and thresholds for debugging
	// Serial.printf("  Sensor %d value: %.2f, Upper threshold: %.2f, Lower threshold: %.2f Sensor Index: %d\r\n",
	// 			  sensor_index, sensor_value, upper_threshold, lower_threshold, sensor_index);
	// control DO based on schmitt trigger logic
	if (sensor_value >= upper_threshold)
	{
		return true; // turn ON DO
	}
	else if (sensor_value <= lower_threshold)
	{
		return false; // turn OFF DO
	}
	// maintain current state if within thresholds
	return atObject_Data.do_state[do_index];
}
bool App_DO::control_DO_from_sensor_state(uint8_t do_index)
{
	// check if do_index is valid
	if (do_index >= MAX_DO_CHANNEL)
	{
		return false;
	}

	// get the corresponding sensor index from do_index
	uint8_t sensor_index = atObject_Param.do_control_sensor_list[do_index]; // assuming 1-to-1 mapping for simplicity
	// check if sensor_index is valid
	if (sensor_index >= MAX_SENSORS)
	{
		return false;
	}
	// get the desired sensor state to turn ON DO
	uint8_t desired_sensor_state = atObject_Param.do_control_sensor_state_list[do_index];
	// get the current sensor state from atObject_Data
	uint8_t current_sensor_state = atObject_Data.sensor_status_list[sensor_index];
	// control DO based on sensor state
	if (current_sensor_state == desired_sensor_state)
	{
		return true; // turn ON DO
	}
	else
	{
		return false; // turn OFF DO
	}
}
bool App_DO::control_DO_from_digital_input(uint8_t do_index)
{
	// check if do_index is valid
	if (do_index >= MAX_DO_CHANNEL)
	{
		return false;
	}

	// get the corresponding DI channel from atObject_Param
	uint8_t di_channel = atObject_Param.do_control_input_digital_list[do_index];
	// check if di_channel is valid
	if (di_channel >= MAX_DI_CHANNEL)
	{
		return false;
	}
	// get the current DI state from atObject_Data
	bool di_state = atObject_Data.di_state[di_channel];
	// control DO based on DI state
	return di_state; // turn ON DO if DI is HIGH, else turn OFF DO
}
void atApp_DO_Task_Func(void *parameter)
{
	while (1)
	{
		atApp_DO.Run_Application(APP_RUN_MODE_AUTO);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

#endif