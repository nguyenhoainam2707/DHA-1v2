#ifndef _Application_atApp_Sensor_
#define _Application_atApp_Sensor_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "App.h"
#include "../src/obj/atObj_Param.h"
#include "../src/obj/atObj_Data.h"
#include "../services/atService_Tool.h"
/* _____DEFINITIONS__________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
TaskHandle_t Task_atApp_Sensor;
void atApp_Sensor_Task_Func(void *parameter);
///////////////////////////////////////////////Testing part//
/* _____GLOBAL FUNCTION______________________________________________________ */
union
{
	uint32_t u;
	float f;
} converter;
/* _____CLASS DEFINITION_____________________________________________________ */
/**
 * This Application class is the application to manage the
 */
class App_Sensor : public Application
{
public:
	App_Sensor();
	~App_Sensor();
	static float read_raw_rs485(uint8_t slave_rs485_index, uint8_t rs485_channel, uint8_t data_type);
	static float calculate_calib(uint8_t sensor_index);
	static bool calculate_calib_linear_regression(CalibTablePoint *table, bool *enable_list, float raw_value, float *calib_value);
	static void  check_error(uint8_t sensor_index);

protected:
private:
	static void App_Sensor_Pend();
	static void App_Sensor_Start();
	static void App_Sensor_Restart();
	static void App_Sensor_Execute();
	static void App_Sensor_Suspend();
	static void App_Sensor_Resume();
	static void App_Sensor_End();
} atApp_Sensor;
/**
 * This function will be automaticaly calSensor when a object is created by this class
 */
App_Sensor::App_Sensor(/* args */)
{
	_Pend_User = *App_Sensor_Pend;
	_Start_User = *App_Sensor_Start;
	_Restart_User = *App_Sensor_Restart;
	_Execute_User = *App_Sensor_Execute;
	_Suspend_User = *App_Sensor_Suspend;
	_Resume_User = *App_Sensor_Resume;
	_End_User = *App_Sensor_End;

	// change the ID of application
	ID_Application = 1;
	// change the application name
	Name_Application = (char *)"Sensor Application	";
	// change the ID of SNM
}
/**
 * This function will be automaticaly calSensor when the object of class is delete
 */
App_Sensor::~App_Sensor()
{
}
/**
 * Pend to start is the first task of this application it will do prerequisite condition. In the debit mode, task will send information of application to terminal to start the application.
 */
void App_Sensor::App_Sensor_Pend()
{
	// atService_PCF8574.Debug();
}
/**
 * This start function will init some critical function
 */
void App_Sensor::App_Sensor_Start()
{
}
/**
 * Restart function of SNM  app
 */
void App_Sensor::App_Sensor_Restart()
{
}
/**
 * Execute fuction of SNM app
 */
void App_Sensor::App_Sensor_Execute()
{
	// Read all sensor raw value
	for (uint8_t sensor_index = 0; sensor_index < MAX_SENSORS; sensor_index++)
	{
		// check if this sensor is enable
		if (atObject_Param.sensor_enable_list[sensor_index] == true)
		{
			switch (atObject_Param.sensor_protocol_list[sensor_index])
			{
			case PROTOCOL_RS485_CH1:
				// if this sensor is enable and protocol is PROTOCOL_RS485_CH1
				atObject_Data.sensor_raw_value_list[sensor_index] = atApp_Sensor.read_raw_rs485(
					atObject_Param.sensor_rs485_index[RS485_CHANNEL_1][sensor_index],
					RS485_CHANNEL_1,
					atObject_Param.sensor_data_type_list[sensor_index]);
				// Serial.printf("\tRead RS485 CH1 Sensor %d: Raw Value = %.2f\n", sensor_index, atObject_Data.sensor_raw_value_list[sensor_index]);
				break;

			case PROTOCOL_RS485_CH2:
				// if this sensor is enable and protocol is PROTOCOL_RS485_CH2
				// print atObject_Param.sensor_rs485_index[PROTOCOL_RS485_CH2][sensor_index],
				// Serial.printf("\t---> Reading Sensor %d with RS485 Index %d on RS485 Channel 2\n",
				// 			  sensor_index,
				// 			  atObject_Param.sensor_rs485_index[RS485_CHANNEL_2][sensor_index]);
				atObject_Data.sensor_raw_value_list[sensor_index] = atApp_Sensor.read_raw_rs485(
					atObject_Param.sensor_rs485_index[RS485_CHANNEL_2][sensor_index],
					RS485_CHANNEL_2,
					atObject_Param.sensor_data_type_list[sensor_index]);
				// Serial.printf("\tRead RS485 CH2 Sensor %d: Raw Value = %.2f\n", sensor_index, atObject_Data.sensor_raw_value_list[sensor_index]);
				break;

			case PROTOCOL_ADC_4_20mA_CH1:
				// if this sensor is enable and protocol is PROTOCOL_ADC_4_20mA_CH1
				atObject_Data.sensor_raw_value_list[sensor_index] = (float)atObject_Data.ai_current[AI_CHANNEL_1];
				// Serial.printf("\t---> Reading Sensor %d from ADC CH1 with Raw Value = %.2f\n",
				// 			  sensor_index,
				// 			  atObject_Data.ai_current[AI_CHANNEL_1]);
				// Serial.printf("\tRead ADC CH1 Sensor %d: Raw Value = %.2f\n", sensor_index, atObject_Data.sensor_raw_value_list[sensor_index]);
				break;
			case PROTOCOL_ADC_4_20mA_CH2:
				// if this sensor is enable and protocol is PROTOCOL_ADC_4_20mA_CH2
				atObject_Data.sensor_raw_value_list[sensor_index] = (float)atObject_Data.ai_current[AI_CHANNEL_2];
				// Serial.printf("\t---> Reading Sensor %d from ADC CH2 with Raw Value = %.2f\n",
				// 			  sensor_index,
				// 			  atObject_Data.ai_current[AI_CHANNEL_2]);
				// Serial.printf("\tRead ADC CH2 Sensor %d: Raw Value = %.2f\n", sensor_index, atObject_Data.sensor_raw_value_list[sensor_index]);
				break;
			case PROTOCOL_ADC_4_20mA_CH3:
				// if this sensor is enable and protocol is PROTOCOL_ADC_4_20mA_CH3
				atObject_Data.sensor_raw_value_list[sensor_index] = (float)atObject_Data.ai_current[AI_CHANNEL_3];
				// Serial.printf("\t---> Reading Sensor %d from ADC CH3 with Raw Value = %.2f\n",
				// 			  sensor_index,
				// 			  atObject_Data.ai_current[AI_CHANNEL_3]);
				// Serial.printf("\tRead ADC CH3 Sensor %d: Raw Value = %.2f\n", sensor_index, atObject_Data.sensor_raw_value_list[sensor_index]);
				break;
			case PROTOCOL_ADC_4_20mA_CH4:
				// if this sensor is enable and protocol is PROTOCOL_ADC_4_20mA_CH4
				atObject_Data.sensor_raw_value_list[sensor_index] = (float)atObject_Data.ai_current[AI_CHANNEL_4];
				// Serial.printf("\t---> Reading Sensor %d from ADC CH4 with Raw Value = %.2f\n",
				// 			  sensor_index,
				// 			  atObject_Data.ai_current[AI_CHANNEL_4]);
				// Serial.printf("\tRead ADC CH4 Sensor %d: Raw Value = %.2f\n", sensor_index, atObject_Data.sensor_raw_value_list[sensor_index]);
				break;

			default:
				// Handle unknown protocol if needed
				break;
			}
			// ...existing code...
			// For calib value
			atObject_Data.sensor_calib_value_list[sensor_index] = atApp_Sensor.calculate_calib(sensor_index);
			
			atApp_Sensor.check_error(sensor_index);
			
		}
		else
		{
			atObject_Data.sensor_status_list[sensor_index] = SENSOR_STATE_DISABLE;
		}
		
	}
	if (atApp_Sensor.User_Mode == APP_USER_MODE_DEBUG)
	{
		for (uint8_t sensor_index = 0; sensor_index < MAX_SENSORS; sensor_index++)
		{
			if (atObject_Param.sensor_enable_list[sensor_index] == true)
			{
				Serial.printf("\tSensor %d: Raw = %.2f, Calib = %.2f, Status = %2d\n",
							  sensor_index + 1,
							  atObject_Data.sensor_raw_value_list[sensor_index],
							  atObject_Data.sensor_calib_value_list[sensor_index],
							  atObject_Data.sensor_status_list[sensor_index]);
			}
		}
	}
}
void App_Sensor::App_Sensor_Suspend() {}
void App_Sensor::App_Sensor_Resume() {}
void App_Sensor::App_Sensor_End() {}
void atApp_Sensor_Task_Func(void *parameter)
{
	while (1)
	{
		atApp_Sensor.Run_Application(APP_RUN_MODE_AUTO);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
/**
 * @brief 	Read sensor value from sensor
 * @param 	sensor_index: sensor index
 * @param 	rs485_channel: rs485 channel
 * @param 	data_type: data type
 * @return 	raw value
 * @note	This function is used to read sensor value from RS485 protocols
 */
float App_Sensor::read_raw_rs485(uint8_t slave_rs485_index, uint8_t rs485_channel, uint8_t data_type)
{
	float raw_value = 0;
	uint16_t high16bitByte, low16bitByte = 0;
	high16bitByte = atObject_Data.rs485_data_list[rs485_channel][slave_rs485_index].high16bitByte;
	low16bitByte = atObject_Data.rs485_data_list[rs485_channel][slave_rs485_index].low16bitByte;
	// Serial.printf("\t---> Reading RS485 Channel %u, Slave Index %u: High 16bit = %u, Low 16bit = %u, data type = %u \n",
	// 			  rs485_channel,
	// 			  slave_rs485_index,
	// 			  high16bitByte,
	// 			  low16bitByte,
	// 			  atObject_Param.sensor_data_type_list[slave_rs485_index]);
	switch (atObject_Param.sensor_data_type_list[slave_rs485_index])
	{
	case SENSOR_DATA_TYPE_INT16:
		// Serial.printf("\tSENSOR_DATA_TYPE_INT16 read\n");
		raw_value = (float)((int16_t)(high16bitByte));
		break;

	case SENSOR_DATA_TYPE_UINT16:
		// Serial.printf("\tSENSOR_DATA_TYPE_UINT16 read\n");
		raw_value = (float)((uint16_t)(high16bitByte));
		break;

	case SENSOR_DATA_TYPE_INT32:
		// Serial.printf("\tSENSOR_DATA_TYPE_INT32 read\n");
		raw_value = (float)((int32_t)((uint32_t)(high16bitByte) << 16 + (uint32_t)(low16bitByte)));
		break;

	case SENSOR_DATA_TYPE_UINT32:
		// Serial.printf("\tSENSOR_DATA_TYPE_UINT32 read\n");
		raw_value = (float)((uint32_t)(high16bitByte) << 16 + (uint32_t)(low16bitByte));
		break;

	case SENSOR_DATA_TYPE_FLOAT_BIG_ENDIAN:
		// Serial.printf("\tSENSOR_DATA_TYPE_FLOAT Convert\n");
		converter.u = ((uint32_t)high16bitByte << 16) | (uint32_t)low16bitByte;
		raw_value = converter.f;
		// Serial.printf("\tHigh 16bit : %u\n", high16bitByte);
		// Serial.printf("\tLow 16bit : %u\n", low16bitByte);
		// Serial.printf("\tunsgined int value : %lu\n", converter.u);
		// Serial.printf("\tfloat value %f at slave_rs485_index [%d]\n", raw_value, slave_rs485_index);

		break;

	case SENSOR_DATA_TYPE_FLOAT_BIG_ENDIAN_SWAPPED:
	{
		// Serial.printf("\tSENSOR_DATA_TYPE_FLOAT_BIG_ENDIAN_SWAPPED Convert\n");
		uint32_t swapped_buffer = high16bitByte;
		high16bitByte = ((swapped_buffer & 0x00FF) << 8) | ((swapped_buffer & 0xFF00) >> 8);
		swapped_buffer = low16bitByte;
		low16bitByte = ((swapped_buffer & 0x00FF) << 8) | ((swapped_buffer & 0xFF00) >> 8);
		converter.u = ((uint32_t)high16bitByte << 16) | (uint32_t)low16bitByte;
		raw_value = converter.f;
		// Serial.printf("\tHigh 16bit : %u\n", high16bitByte);
		// Serial.printf("\tLow 16bit : %u\n", low16bitByte);
		// Serial.printf("\tunsgined int value : %lu\n", converter.u);
		// Serial.printf("\tfloat value %f at slave_rs485_index [%d]\n", raw_value, slave_rs485_index);
		break;
	}

	case SENSOR_DATA_TYPE_FLOAT_LITTLE_ENDIAN:
	{
		// Serial.printf("\tSENSOR_DATA_TYPE_FLOAT_LITTLE_ENDIAN Convert\n");
		uint32_t swapped_buffer = high16bitByte;
		high16bitByte = ((swapped_buffer & 0x00FF) << 8) | ((swapped_buffer & 0xFF00) >> 8);
		swapped_buffer = low16bitByte;
		low16bitByte = ((swapped_buffer & 0x00FF) << 8) | ((swapped_buffer & 0xFF00) >> 8);
		converter.u = (uint32_t)(low16bitByte) << 16 | (uint32_t)(high16bitByte);
		raw_value = converter.f;
		// Serial.printf("\tHigh 16bit : %u\n", high16bitByte);
		// Serial.printf("\tLow 16bit : %u\n", low16bitByte);
		// Serial.printf("\t unsgined int value : %lu\n", converter.u);
		// Serial.printf("\tfloat value %f at slave_rs485_index [%d]\n", raw_value, slave_rs485_index);
		break;
	}
	case SENSOR_DATA_TYPE_FLOAT_LITTLE_ENDIAN_SWAPPED:
		// Serial.printf("\tSENSOR_DATA_TYPE_FLOAT_LITTLE_ENDIAN_SWAPPED read\n");
		converter.u = (uint32_t)(low16bitByte) << 16 | (uint32_t)(high16bitByte);
		raw_value = converter.f;
		// Serial.printf("\tunsigned int value [%u]\n", converter.u);
		// Serial.printf("\tfloat value [%f] at slave_rs485_index [%d]\n", raw_value, slave_rs485_index);
		break;
	default:
		break;
	}
	return raw_value;
}
/**
 * @brief Calculate the calib value of the index sensor
 * @param sensor_index
 * @param calib_value
 * @return the calib value
 */
float App_Sensor::calculate_calib(uint8_t sensor_index)
{
	float calib_value = 0;
	float index_A = atObject_Param.sensor_calib_function_index_list[sensor_index].index_A;
	float index_B = atObject_Param.sensor_calib_function_index_list[sensor_index].index_B;
	float index_C = atObject_Param.sensor_calib_function_index_list[sensor_index].index_C;
	float index_D = atObject_Param.sensor_calib_function_index_list[sensor_index].index_D;
	float raw_value = atObject_Data.sensor_raw_value_list[sensor_index];

	switch (atObject_Param.sensor_calib_function_order_list[sensor_index])
	{
	case EQUATION_LINEAR:
		calib_value = index_A + index_B * raw_value;
		break;
	case EQUATION_QUADRATIC:
		calib_value = index_A + index_B * raw_value + index_C * raw_value * raw_value;
		break;
	case EQUATION_CUBIC:
		calib_value = index_A + index_B * raw_value + index_C * raw_value * raw_value + index_D * raw_value * raw_value * raw_value;
		break;
	case EQUATION_TABLE:
	{
		// Use calibration table

		
		bool success = calculate_calib_linear_regression(
			atObject_Param.sensor_calib_table_list[sensor_index], 
			atObject_Param.sensor_calib_table_point_active_list[sensor_index], 
			raw_value, 
			&calib_value);
		if (!success)
		{
			// If calculation failed, fallback to raw value
			calib_value = raw_value;
		}
		break;
	}
	default:
		break;
	}
	// Check if user enable the limit
	if (atObject_Param.sensor_limit_enable_list[sensor_index])
	{
		// in the case lower limit
		if (calib_value < atObject_Param.sensor_calib_lower_limit_threshold_list[sensor_index])
		{
			calib_value = atObject_Param.sensor_calib_lower_limit_threshold_list[sensor_index] + atService_Tool.randomFloat(
																									 -atObject_Param.sensor_lower_limit_noise_list[sensor_index],
																									 atObject_Param.sensor_lower_limit_noise_list[sensor_index]);
		}
		// in the case upper limit
		if (calib_value > atObject_Param.sensor_calib_upper_limit_threshold_list[sensor_index])
		{

			calib_value = atObject_Param.sensor_calib_upper_limit_threshold_list[sensor_index] + atService_Tool.randomFloat(
																									 -atObject_Param.sensor_upper_limit_noise_list[sensor_index],
																									 atObject_Param.sensor_upper_limit_noise_list[sensor_index]);
		}
	}
	return calib_value;
}
/**
 * @brief 	Check if the sensor is error, if raw value and calib value is out of range
 * @param 	sensor_index: the index of the sensor
 */
void App_Sensor::check_error(uint8_t sensor_index)
{
	if (atObject_Param.sensor_error_enable_list[sensor_index])
	{
		switch (atObject_Param.sensor_state_protocol_list[sensor_index])
		{
		case PROTOCOL_STATE_READING_AUTO: // ------------------------------------------- PROTOCOL_STATE_READING_AUTO
		{
			/* code */
			// Read values from Data
			float raw_value = atObject_Data.sensor_raw_value_list[sensor_index];
			float calib_value = atObject_Data.sensor_calib_value_list[sensor_index];
			// Check condition
			bool error_existed = false;
			if (raw_value < atObject_Param.sensor_raw_error_lower_threshold_list[sensor_index])
				error_existed = true;
			else if (raw_value > atObject_Param.sensor_raw_error_upper_threshold_list[sensor_index])
				error_existed = true;
			else if (calib_value < atObject_Param.sensor_calib_error_lower_threshold_list[sensor_index])
				error_existed = true;
			else if (calib_value > atObject_Param.sensor_calib_error_upper_threshold_list[sensor_index])
				error_existed = true;
			else
				error_existed = false;
			// Write the state to the Data object
			if (atObject_Data.sensor_status_list[sensor_index] != SENSOR_STATE_CALIB)
			{
				if (error_existed)
					atObject_Data.sensor_status_list[sensor_index] = SENSOR_STATE_ERROR;
				else
					atObject_Data.sensor_status_list[sensor_index] = SENSOR_STATE_NORMAL;
			}
			// check status from rs485 response if sensor use rs485 protocol
			if (atObject_Param.sensor_protocol_list[sensor_index] == PROTOCOL_RS485_CH1)
			{
				uint8_t rs485_response = atObject_Data.rs485_response_list[RS485_CHANNEL_1][atObject_Param.sensor_rs485_index[RS485_CHANNEL_1][sensor_index]];
				if (rs485_response != 0)
				{
					atObject_Data.sensor_status_list[sensor_index] = SENSOR_STATE_ERROR;
				}
			}
			else if (atObject_Param.sensor_protocol_list[sensor_index] == PROTOCOL_RS485_CH2)
			{
				uint8_t rs485_response = atObject_Data.rs485_response_list[RS485_CHANNEL_2][atObject_Param.sensor_rs485_index[RS485_CHANNEL_2][sensor_index]];
				if (rs485_response != 0)
				{
				}
			}
			break;
		}
		case PROTOCOL_STATE_READING_DIGITAL_INPUT: // ------------------------------------------- PROTOCOL_STATE_READING_DIGITAL_INPUT
			// do nothing for now, the DI will control the sensor state directly
			
			break;
		case PROTOCOL_STATE_READING_RS485: // ------------------------------------------- PROTOCOL_STATE_READING_RS
			// do nothing for now, the RS485 response will control the sensor state directly
			
			break;

		default:
			break;
		}
	}
	else
	{
		// if error checking is not enable, just set the status to normal if not in calib mode
		if (atObject_Data.sensor_status_list[sensor_index] != SENSOR_STATE_CALIB)
		{
			atObject_Data.sensor_status_list[sensor_index] = SENSOR_STATE_NORMAL;
		}
	}
}

/**
 * @brief 	Calculate the calib value using linear regression method
 * @param  	table: array of calibration points {CalibTablePoint}
 * @param 	enable_list: list of bools indicating enabled points
 * @param 	raw_value: the raw value to be calculated
 * @param 	calib_value: pointer to store the calculated calib value
 * @return 	false if calculation failed (e.g., not enough points), true otherwise
 */
bool App_Sensor::calculate_calib_linear_regression(CalibTablePoint *table, bool *enable_list, float raw_value, float *calib_value)
{
    uint8_t point_count = 0;
    double sum_x = 0, sum_y = 0, sum_xy = 0, sum_x2 = 0;

    for (uint8_t i = 0; i < MAX_CALIB_TABLE_POINTS; i++)
    {
        if (!enable_list[i])
            continue; // skip disabled points

        double x = table[i].raw_value;
        double y = table[i].calib_value;

        sum_x += x;
        sum_y += y;
        sum_xy += x * y;
        sum_x2 += x * x;
        point_count++;
		// print debug info for each point
		// Serial.printf("Point %d: raw_value = %f, calib_value = %f\n", i, x, y);
    }

    if (point_count < 2)
        return false; // Not enough points

    double denominator = (point_count * sum_x2 - sum_x * sum_x);
    if (fabs(denominator) < 1e-6)
        return false; // Avoid division by zero

    double slope = (point_count * sum_xy - sum_x * sum_y) / denominator;
    double intercept = (sum_y - slope * sum_x) / point_count;

    *calib_value =  static_cast<float>(slope * raw_value + intercept);

	// print all debug info
	// Serial.printf("slope: %f, intercept: %f, raw_value: %f, calib_value: %f\n", slope, intercept, raw_value, *calib_value);
	// print all sum
	// Serial.printf("sum_x: %f, sum_y: %f, sum_xy: %f, sum_x2: %f, point_count: %d\n", sum_x, sum_y, sum_xy, sum_x2, point_count);

    return true;
}

#endif