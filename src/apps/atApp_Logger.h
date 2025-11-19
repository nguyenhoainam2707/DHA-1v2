#ifndef _Application_atApp_Logger_
#define _Application_atApp_Logger_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "App.h"
#include "../src/obj/atObj_Param.h"
#include "../src/obj/atObj_Data.h"
#include "../services/memory/atService_SD.h"
/* _____DEFINITIONS__________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
TaskHandle_t Task_atApp_Logger;
void atApp_Logger_Task_Func(void *parameter);
///////////////////////////////////////////////Testing part//
/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */

/**
 * This Application class is the application to manage the
 */
class App_Logger : public Application
{
public:
	App_Logger();
	~App_Logger();
	ActionTime last_logged_time;
protected:
private:
	static void App_Logger_Pend();
	static void App_Logger_Start();
	static void App_Logger_Restart();
	static void App_Logger_Execute();
	static void App_Logger_Suspend();
	static void App_Logger_Resume();
	static void App_Logger_End();
	String year_stamp ;
	String month_stamp;
	String day_stamp;
	String hour_stamp;
	String minute_stamp;
	String second_stamp;
	String date_time_stamp;
} atApp_Logger;
/**
 * This function will be automaticaly calLogger when a object is created by this class
 */
App_Logger::App_Logger(/* args */)
{
	_Pend_User = *App_Logger_Pend;
	_Start_User = *App_Logger_Start;
	_Restart_User = *App_Logger_Restart;
	_Execute_User = *App_Logger_Execute;
	_Suspend_User = *App_Logger_Suspend;
	_Resume_User = *App_Logger_Resume;
	_End_User = *App_Logger_End;

	// change the ID of application
	ID_Application = 1;
	// change the application name
	Name_Application = (char *)"Logger Application	";
	// change the ID of SNM
}
/**
 * This function will be automaticaly calLogger when the object of class is delete
 */
App_Logger::~App_Logger()
{
}
/**
 * Pend to start is the first task of this application it will do prerequisite condition. In the debit mode, task will send information of application to terminal to start the application.
 */
void App_Logger::App_Logger_Pend()
{
	// atService_PCF8574.Debug();
}
/**
 * This start function will init some critical function
 */
void App_Logger::App_Logger_Start()
{
	atApp_Logger.last_logged_time.request= false;
	atApp_Logger.last_logged_time.second = 0;
	atApp_Logger.last_logged_time.minute = 0;
	atApp_Logger.last_logged_time.hour = 0;

	atService_SD.Run_Service();
	if (!atService_SD.isInitialized())
	{
		if (!atService_SD.begin(SD_CLK, SD_CMD, SD_D0, SD_D1, SD_D2, SD_D3, false))
		{
			if (atApp_Logger.User_Mode == SER_USER_MODE_DEBUG)
			{
				Serial.println("\n✗ CANNOT INITIALIZE SD CARD!");
				Serial.println("Please check:");
				Serial.println("  - SD card is properly inserted");
				Serial.println("  - Correct SDIO pin connections:");
				Serial.printf("    CLK: GPIO%d\n", SD_CLK);
				Serial.printf("    CMD: GPIO%d\n", SD_CMD);
				Serial.printf("    D0:  GPIO%d\n", SD_D0);
				Serial.printf("    D1:  GPIO%d\n", SD_D1);
				Serial.printf("    D2:  GPIO%d\n", SD_D2);
				Serial.printf("    D3:  GPIO%d\n", SD_D3);
				Serial.println("  - SD card working properly");
				Serial.println("  - Stable 3.3V power supply");

				// Try 1-bit mode if 4-bit mode fails
				Serial.println("\n→ Retrying with 1-bit mode...");
			}
			if (!atService_SD.begin(SD_CLK, SD_CMD, SD_D0, 255, 255, 255, false))
			{
				if (atApp_Logger.User_Mode == SER_USER_MODE_DEBUG)
				{
					Serial.println("✗ 1-bit mode also failed!");
				}
				return;
			}
		}
		if (atApp_Logger.User_Mode == APP_USER_MODE_DEBUG)
		{
			Serial.println("✓ SD Card initialized successfully.");
			atService_SD.Debug();
			atService_SD.printCardInfo();
		}
	}
}
/**;
 * Restart function of SNM  app
 */
void App_Logger::App_Logger_Restart()
{
}
/**
 * Execute fuction of SNM app
 */
void App_Logger::App_Logger_Execute()
{
	// run at every minutes
	if (atObject_Data.rtc_minute != atApp_Logger.last_logged_time.minute)
	{
		atApp_Logger.last_logged_time.minute = atObject_Data.rtc_minute;
		// atApp_Logger.last_logged_time.second = atObject_Data.rtc_second;
		// atApp_Logger.last_logged_time.hour = atObject_Data.rtc_hour;
		atApp_Logger.last_logged_time.request = true;
	}
	if (atApp_Logger.last_logged_time.request)
	{
		atApp_Logger.last_logged_time.request = false;
		Serial.println("Logger executing...");
		atApp_Logger.year_stamp = "20" + String(atObject_Data.rtc_year);
		atApp_Logger.month_stamp = (atObject_Data.rtc_month < 10 ? "0" : "") + String(atObject_Data.rtc_month);
		atApp_Logger.day_stamp = (atObject_Data.rtc_day < 10 ? "0" : "") + String(atObject_Data.rtc_day);
		atApp_Logger.hour_stamp = (atObject_Data.rtc_hour < 10 ? "0" : "") + String(atObject_Data.rtc_hour);
		atApp_Logger.minute_stamp = (atObject_Data.rtc_minute < 10 ? "0" : "") + String(atObject_Data.rtc_minute);
		atApp_Logger.second_stamp = "00";
		atApp_Logger.date_time_stamp = 	atApp_Logger.year_stamp + \
										atApp_Logger.month_stamp + \
										atApp_Logger.day_stamp + \
										atApp_Logger.hour_stamp + \
										atApp_Logger.minute_stamp +\
										atApp_Logger.second_stamp;
		
		// print date time stamp
		Serial.printf("Date Time Stamp: %s\n", atApp_Logger.date_time_stamp.c_str());
		for (uint8_t ftp_index = 0; ftp_index < MAX_FTP_SERVER; ftp_index++)
		{
			if (atObject_Data.rtc_minute % atObject_Param.ftp_send_time_list[ftp_index] == 0)
			{
				atObject_Data.logger_content_list[ftp_index] = "";
				for (uint8_t sensor_index = 0; sensor_index < MAX_SENSORS; sensor_index++)
				{
					if (atObject_Param.ftp_sensor_config[ftp_index][sensor_index])
					{
						float round_calib_value = round(100 * atObject_Data.sensor_calib_value_list[sensor_index]) / 100;
						// Convert to a string
						String round_calib_value_string = String(round_calib_value);
						String sensor_state_num_string = atObject_Data.sensor_status_code_list[atObject_Data.sensor_status_list[sensor_index]];
						// Add to logger content
						atObject_Data.logger_content_list[ftp_index] += atObject_Param.sensor_name_list[sensor_index] + "\t" + round_calib_value_string + "\t" + atObject_Param.sensor_unit_list[sensor_index] + "\t" + atApp_Logger.date_time_stamp + "\t" + sensor_state_num_string + "\n";
					}
				}
				// write log into content of ftp server
				atObject_Data.ftp_file_content_list[ftp_index] = atObject_Data.logger_content_list[ftp_index];
				atObject_Data.logger_new_log_list[ftp_index] = true;
				// save file to the sd card into folder: FTP1,FTP2,....


			}
		}

		// save the new log to sdcad
		for (uint8_t ftp_index = 0; ftp_index < MAX_FTP_SERVER; ftp_index++)
		{
			// chech if there is a new content log
			if (atObject_Data.logger_new_log_list[ftp_index])
			{
				String file_path = atObject_Param.logger_save_path[ftp_index]; //+ atObject_Param.ftp_path_list[ftp_index];
				file_path.replace(LOGGER_CODE_YEAR, atApp_Logger.year_stamp);
				file_path.replace(LOGGER_CODE_MONTH, atApp_Logger.month_stamp);
				file_path.replace(LOGGER_CODE_DAY, atApp_Logger.day_stamp);
				// file_path.replace(LOGGER_CODE_HOUR, atApp_Logger.hour_stamp);
				// file_path.replace(LOGGER_CODE_MINUTE, atApp_Logger.minute_stamp);
				// file_path.replace(LOGGER_CODE_SECOND, atApp_Logger.second_stamp);
				
				//replace string YYYY, MM,DD with year_stamp, month_stamp, day_stamp
				String file_name = atObject_Param.ftp_file_name_list[ftp_index];
				// Serial.printf("Original file name: %s\n", file_name.c_str());
				file_name.replace(LOGGER_CODE_YEAR, atApp_Logger.year_stamp);
				// Serial.printf("After YEAR file name: %s\n", file_name.c_str());
				file_name.replace(LOGGER_CODE_MONTH, atApp_Logger.month_stamp);
				// Serial.printf("After MONTH file name: %s\n", file_name.c_str());
				file_name.replace(LOGGER_CODE_DAY, atApp_Logger.day_stamp);
				// Serial.printf("After DAY file name: %s\n", file_name.c_str());
				file_name.replace(LOGGER_CODE_HOUR, atApp_Logger.hour_stamp);
				// Serial.printf("After HOUR file name: %s\n", file_name.c_str());
				file_name.replace(LOGGER_CODE_MINUTE, atApp_Logger.minute_stamp);
				// Serial.printf("After MINUTE file name: %s\n", file_name.c_str());
				file_name.replace(LOGGER_CODE_SECOND, atApp_Logger.second_stamp);
				// Serial.printf("Final file name: %s\n", file_name.c_str());
				// save the new log to the sd card
				// clear the new log flag
				atObject_Data.logger_new_log_list[ftp_index] = false;
				
				if (!atService_SD.dirExists(file_path.c_str()))
					atService_SD.createDir(file_path.c_str());

				// write to sdcard
				bool response = atService_SD.writeFile(
					(file_path +"/"+ file_name).c_str(),
					atObject_Data.logger_content_list[ftp_index],
					false
				);
				// print response 
				if (atApp_Logger.User_Mode == APP_USER_MODE_DEBUG)
				{
					if (response)
					{
						Serial.printf("✓ Successfully saved new log to SD Card: %s/%s\n", file_path.c_str(), file_name.c_str());
					}
					else
					{
						Serial.printf("✗ ERROR! Cannot save log to SD Card: %s/%s\n", file_path.c_str(), file_name.c_str());
					}
				}
				// clear the flog new log
				atObject_Data.logger_new_log_list[ftp_index] = false;
			}
		}
	}

	if (atApp_Logger.User_Mode == APP_USER_MODE_DEBUG)
	{
		// print into serial rtc time
		Serial.printf("Logger executing at RTC time: %02d:%02d:%02d\n",
					  atObject_Data.rtc_hour,
					  atObject_Data.rtc_minute,
					  atObject_Data.rtc_second);
	}
}
void App_Logger::App_Logger_Suspend() {}
void App_Logger::App_Logger_Resume() {}
void App_Logger::App_Logger_End() {}
void atApp_Logger_Task_Func(void *parameter)
{
	while (1)
	{
		atApp_Logger.Run_Application(APP_RUN_MODE_AUTO);
		vTaskDelay(2000 / portTICK_PERIOD_MS);
	}
}
#endif