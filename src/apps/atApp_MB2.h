#ifndef _Application_atApp_MB2_
#define _Application_atApp_MB2_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "App.h"
#include "../services/atService_Modbus.h"
#include "../src/obj/atObj_Param.h"
#include "../src/obj/atObj_Data.h"
// #include "../src/apps/atApp_Database.h"
/* _____DEFINITIONS__________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
TaskHandle_t Task_atApp_MB2;
void atApp_MB2_Task_Func(void *parameter);
///////////////////////////////////////////////Testing part//
/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */

/**
 * This Application class is the application to manage the
 */
class App_MB2 : public Application
{
public:
	App_MB2();
	~App_MB2();

protected:
private:
	static void App_MB2_Pend();
	static void App_MB2_Start();
	static void App_MB2_Restart();
	static void App_MB2_Execute();
	static void App_MB2_Suspend();
	static void App_MB2_Resume();
	static void App_MB2_End();
	static void App_MB2_Read_Calib_Status_Register(uint8_t sensor_index);
	static void App_MB2_Read_Error_Status_Register(uint8_t sensor_index);
} atApp_MB2;
/**
 * This function will be automaticaly called when a object is created by this class
 */
App_MB2::App_MB2(/* args */)
{
	_Pend_User = *App_MB2_Pend;
	_Start_User = *App_MB2_Start;
	_Restart_User = *App_MB2_Restart;
	_Execute_User = *App_MB2_Execute;
	_Suspend_User = *App_MB2_Suspend;
	_Resume_User = *App_MB2_Resume;
	_End_User = *App_MB2_End;

	// change the ID of application
	ID_Application = 1;
	// change the application name
	Name_Application = (char *)"MB2 Application	";
	// change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
App_MB2::~App_MB2()
{
}
/**
 * Pend to start is the first task of this application it will do prerequisite condition. In the debit mode, task will send information of application to terminal to start the application.
 */
void App_MB2::App_MB2_Pend()
{
	// atService_Modbus.Debug();
}
/**
 * This start function will init some critical function
 */
void App_MB2::App_MB2_Start()
{
	// init atModbus Service in the fist running time
	atService_Modbus2.init(
		atObject_Param.rs485_slave_baudrate_list[RS485_CHANNEL_2][0],
		atObject_Param.rs485_slave_serial_config_list[RS485_CHANNEL_2][0],
		MB2_RX_PIN,
		MB2_TX_PIN);
	if (atApp_MB2.User_Mode == APP_USER_MODE_DEBUG)
	{
		Serial.println("\tMB2 Application Started debug mode");
	}
	// atService_Modbus2.User_Mode = SER_USER_MODE_DEBUG;
}
/**
 * Restart function of SNM  app
 */
void App_MB2::App_MB2_Restart()
{
}
/**
 * Execute fuction of SNM app
 */
void App_MB2::App_MB2_Execute()
{
	uint16_t data[2] = {0,0};
	uint8_t response;
	// chec
	if (atObject_Param.rs485_enable[RS485_CHANNEL_2] == false)
	{
		return;
	}
	for (uint8_t i = 0; i < MAX_SENSORS; i++)
	{
		// check if the slave is enabled
		if (!atObject_Param.rs485_slave_enable_list[RS485_CHANNEL_2][i])
		{
			continue;
		}
		switch (atObject_Param.rs485_slave_function_code_list[RS485_CHANNEL_2][i])
		{
		case 3: // Read Holding Registers
			// set baudrate and config before reading
			atService_Modbus2.setBaudRate(atObject_Param.rs485_slave_baudrate_list[RS485_CHANNEL_2][i]);
			atService_Modbus2.setConfig(atObject_Param.rs485_slave_serial_config_list[RS485_CHANNEL_2][i]);
			response = atService_Modbus2.atReadHoldingRegisters(						  // function to read holding registers
				atObject_Param.rs485_slave_id_list[RS485_CHANNEL_2][i],				  // slave ID
				atObject_Param.rs485_slave_timeout_list[RS485_CHANNEL_2][i],		  // timeout in milliseconds
				atObject_Param.rs485_slave_starting_address_list[RS485_CHANNEL_2][i], // start address
				atObject_Param.rs485_slave_quantity_list[RS485_CHANNEL_2][i],		  // number of registers to read
				data																  // array to store the read values
			);
			atObject_Data.rs485_response_list[RS485_CHANNEL_2][i] = response;
			// in the case read successful
			if (response == 0)
			{
				atObject_Data.rs485_data_list[RS485_CHANNEL_2][i].low16bitByte = data[1];
				atObject_Data.rs485_data_list[RS485_CHANNEL_2][i].high16bitByte = data[0];
			}	
				
			// read Calibration status register
			if (atObject_Param.rs485_slave_calib_state_reading_enable_list[RS485_CHANNEL_2][i])
			{
				// read calib status register
				App_MB2_Read_Calib_Status_Register(i);
			}
			// read Error status register
			if (atObject_Param.rs485_slave_error_state_reading_enable_list[RS485_CHANNEL_2][i])
			{
				// read error status register
				App_MB2_Read_Error_Status_Register(i);
			}

			break;

		case 4: // Read Input Registers
			atService_Modbus2.setBaudRate(atObject_Param.rs485_slave_baudrate_list[RS485_CHANNEL_2][i]);
			atService_Modbus2.setConfig(atObject_Param.rs485_slave_serial_config_list[RS485_CHANNEL_2][i]);
			response = atService_Modbus2.atReadInputRegisters(						  // function to read holding registers
				atObject_Param.rs485_slave_id_list[RS485_CHANNEL_2][i],				  // slave ID
				atObject_Param.rs485_slave_timeout_list[RS485_CHANNEL_2][i],		  // timeout in milliseconds
				atObject_Param.rs485_slave_starting_address_list[RS485_CHANNEL_2][i], // start address
				atObject_Param.rs485_slave_quantity_list[RS485_CHANNEL_2][i],		  // number of registers to read
				data																  // array to store the read values
			);
			atObject_Data.rs485_response_list[RS485_CHANNEL_2][i] = response;
			// in the case read successful
			if (response == 0)
			{
				atObject_Data.rs485_data_list[RS485_CHANNEL_2][i].low16bitByte = data[1];
				atObject_Data.rs485_data_list[RS485_CHANNEL_2][i].high16bitByte = data[0];
			}
			// read Calibration status register
			if (atObject_Param.rs485_slave_calib_state_reading_enable_list[RS485_CHANNEL_2][i])
			{
				// read calib status register
				App_MB2_Read_Calib_Status_Register(i);
			}
			// read Error status register
			if (atObject_Param.rs485_slave_error_state_reading_enable_list[RS485_CHANNEL_2][i])
			{
				// read error status register
				App_MB2_Read_Error_Status_Register(i);
			}
			break;	
		default:
			break;
		}
		// Delay between each slave read
		vTaskDelay(10 / portTICK_PERIOD_MS); // delay between each slave read
	}
	
	if (atApp_MB2.User_Mode == APP_USER_MODE_DEBUG)
	{
		// print all rs485_data_list of channel 2
		Serial.println("\tMB2: RS485 Channel 2 Data:");
		for (uint8_t i = 0; i < MAX_SENSORS; i++)
		{
			Serial.printf("\t\tSensor %d: High 16bit: %d, Low 16bit: %d, Response: %d\n", \
				i + 1, \
				atObject_Data.rs485_data_list[RS485_CHANNEL_2][i].high16bitByte, \
				atObject_Data.rs485_data_list[RS485_CHANNEL_2][i].low16bitByte, \
				atObject_Data.rs485_response_list[RS485_CHANNEL_2][i]);
		}
	}
}
void App_MB2::App_MB2_Suspend() {}
void App_MB2::App_MB2_Resume() {}
void App_MB2::App_MB2_End() {}
void App_MB2::App_MB2_Read_Calib_Status_Register(uint8_t sensor_index)
{
	uint16_t data[1] = {0};
	uint8_t response = 0;
	// Serial.printf("\n\t---> Reading Calibration Status Register for Sensor %u ",
	// 			  sensor_index + 1);
	response = atService_Modbus2.atReadHoldingRegisters(					  // function to read holding registers
		atObject_Param.rs485_slave_id_list[RS485_CHANNEL_2][sensor_index],				  // slave ID
		atObject_Param.rs485_slave_timeout_list[RS485_CHANNEL_2][sensor_index],		  // timeout in milliseconds
		atObject_Param.rs485_slave_state_calib_address_list[RS485_CHANNEL_2][sensor_index], // start address
		1,		  // number of registers to read
		data// array to store the read values
	);
	// print the response code and data
	// Serial.printf("... Response: %d, Data: %d",
	// 			  response,
	// 			  data[0]);
	if (response == 0) 
	{
		if  (atObject_Param.rs485_slave_state_calib_value_list[RS485_CHANNEL_2][sensor_index] == data[0])
		{
			atObject_Data.sensor_status_list[sensor_index] = SENSOR_STATE_CALIB;
			// Serial.printf("++++++++++++> Sensor %u is in CALIBRATION state!\n", sensor_index + 1);
		}
		else
		{
			atObject_Data.sensor_status_list[sensor_index] = SENSOR_STATE_NORMAL;
		}
	}
	
}
void App_MB2::App_MB2_Read_Error_Status_Register(uint8_t sensor_index)
{
	uint16_t data[1] = {0};
	uint8_t response = 0;
	// Serial.printf("\n\t---> Reading Error Status Register for Sensor %u ",
				//   sensor_index + 1);
	response = atService_Modbus2.atReadHoldingRegisters(					  // function to read holding registers
		atObject_Param.rs485_slave_id_list[RS485_CHANNEL_2][sensor_index],				  // slave ID
		atObject_Param.rs485_slave_timeout_list[RS485_CHANNEL_2][sensor_index],		  // timeout in milliseconds
		atObject_Param.rs485_slave_state_error_address_list[RS485_CHANNEL_2][sensor_index], // start address
		1,		  // number of registers to read
		data// array to store the read values
	);
	// print the response code and data
	// Serial.printf("... Response: %d, Data: %d",
	// 			  response,
	// 			  data[0]);
	if (response == 0)
	{ 
		if (atObject_Param.rs485_slave_state_error_value_list[RS485_CHANNEL_2][sensor_index] == data[0])
		{
			atObject_Data.sensor_status_list[sensor_index] = SENSOR_STATE_ERROR;
			// Serial.printf("++++++++++++> Sensor %u is in ERROR state!\n", sensor_index + 1);
		}		
	}
	// else
	// {
	// 	if (atObject_Param.sensor_state_protocol_list[sensor_index] == PROTOCOL_STATE_READING_AUTO)
	// 		atObject_Data.sensor_status_list[sensor_index] = SENSOR_STATE_ERROR;
	// }
}
void atApp_MB2_Task_Func(void *parameter)
{
	while (1)
	{
		atApp_MB2.Run_Application(APP_RUN_MODE_AUTO);
		vTaskDelay(2000 / portTICK_PERIOD_MS);
	}
}
#endif