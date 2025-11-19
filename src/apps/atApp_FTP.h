#ifndef _Application_atApp_FTP_
#define _Application_atApp_FTP_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "App.h"
#include "../services/atService_EG800K.h"
#include "../src/obj/atObj_Param.h"
#include "../src/obj/atObj_Data.h"
#include "../services/atService_Tool.h"
#include "../services/memory/atService_SD.h"
/* _____DEFINITIONS__________________________________________________________ */
#define MAX_PATH_NUMBER 20
/* _____GLOBAL VARIABLES_____________________________________________________ */
TaskHandle_t Task_atApp_FTP;
void atApp_FTP_Task_Func(void *parameter);
///////////////////////////////////////////////Testing part//
/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */

/**
 * This Application class is the application to manage the
 */
class App_FTP : public Application
{
public:
	App_FTP();
	~App_FTP();
	static void getTimeStampFromLogFileName(String log_file_name, String &year, String &month, String &day);
	ActionTime last_ftp_time;
protected:
private:
	static void App_FTP_Pend();
	static void App_FTP_Start();
	static void App_FTP_Restart();
	static void App_FTP_Execute();
	static void App_FTP_Suspend();
	static void App_FTP_Resume();
	static void App_FTP_End();
} atApp_FTP;
/**
 * This function will be automaticaly called when a object is created by this class
 */
App_FTP::App_FTP(/* args */)
{
	_Pend_User = *App_FTP_Pend;
	_Start_User = *App_FTP_Start;
	_Restart_User = *App_FTP_Restart;
	_Execute_User = *App_FTP_Execute;
	_Suspend_User = *App_FTP_Suspend;
	_Resume_User = *App_FTP_Resume;
	_End_User = *App_FTP_End;

	// change the ID of application
	ID_Application = 1;
	// change the application name
	Name_Application = (char *)"FTP Application	";
	// change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
App_FTP::~App_FTP()
{
}
/**
 * Pend to start is the first task of this application it will do prerequisite condition. In the debit mode, task will send information of application to terminal to start the application.
 */
void App_FTP::App_FTP_Pend()
{
	// atService_EG800K.Debug();
}
/**
 * This start function will init some critical function
 */
void App_FTP::App_FTP_Start()
{
	// init atEG800K Service in the fist running time
	// atService_EG800K.Run_Service();
	Service_EG800K::initEG800K();
	// Service_EG800K::configFTP();
	if (atApp_FTP.User_Mode == APP_USER_MODE_DEBUG)
	{
		atService_EG800K.Debug();
		atService_SD.Debug();
		Serial.println("\n\nFTP Application Started debgugging...\n");
	}

	// init last ftp time
	atApp_FTP.last_ftp_time.request = false;
	atApp_FTP.last_ftp_time.hour = atObject_Data.rtc_hour;
	atApp_FTP.last_ftp_time.minute = atObject_Data.rtc_minute;
	atApp_FTP.last_ftp_time.second = atObject_Data.rtc_second;
}
/**
 * Restart function of SNM  app
 */
void App_FTP::App_FTP_Restart()
{
}
/**
 * Execute fuction of SNM app
 */
void App_FTP::App_FTP_Execute()
{
	if ((atObject_Data.rtc_minute!= atApp_FTP.last_ftp_time.minute) && (atObject_Data.rtc_second >= 15))
	{

		atApp_FTP.last_ftp_time.minute = atObject_Data.rtc_minute;
		atApp_FTP.last_ftp_time.request = true;
	}
	// ---------------------- Testing
	atApp_FTP.last_ftp_time.request = true;
	// run every minutes
	if (atApp_FTP.last_ftp_time.request)
	{
		// reset request
		atApp_FTP.last_ftp_time.request = false;
		// Serial.println("FTP Application Executing...");

		// check if the case
		for (uint8_t ftp_index = 0; ftp_index < MAX_FTP_SERVER; ftp_index++)
		{
			if (atObject_Param.ftp_active_list[ftp_index])
			{
				if (atObject_Data.rtc_minute % atObject_Param.ftp_send_time_list[ftp_index] == 0)
				{
					atObject_Data.ftp_send_log_flag_list[ftp_index] = true;
					// Serial.printf("Set FTP Server %d to send log file\n", ftp_index);
				}
			}
		}

		for (uint8_t ftp_index = 0; ftp_index < MAX_FTP_SERVER; ftp_index++)
		{
			if (atObject_Data.ftp_send_log_flag_list[ftp_index])
			{
				// Read unsend file in the log folder of this FTP Server
				String unsend_file_name_list[atObject_Param.ftp_max_file_number_per_one_send_time[ftp_index]] = {""};
				// write "" into every element of unsend_file_name_list
				for (uint8_t i = 0; i < atObject_Param.ftp_max_file_number_per_one_send_time[ftp_index]; i++)
				{
					unsend_file_name_list[i] = "";
				}
				//  List all file in the folder into unsend_file_name_list
				atService_SD.listFiles(
					atObject_Param.logger_save_path[ftp_index].c_str(),
					unsend_file_name_list,
					atObject_Param.ftp_max_file_number_per_one_send_time[ftp_index]);
				
				if (atApp_FTP.User_Mode == APP_USER_MODE_DEBUG)
				{
					// print unsend file name list
					// Serial.printf("Unsend file list for FTP Server %d:\n", ftp_index);
					for (uint8_t file_index = 0; file_index < atObject_Param.ftp_max_file_number_per_one_send_time[ftp_index]; file_index++)
					{
						if (unsend_file_name_list[file_index] != "")
						{
							Serial.printf(" - %s\n", unsend_file_name_list[file_index].c_str());
						}
						else
						{
							break;
						}
					}
				}

				// Send all file that is not send and store in unsend_file_name_list
				for (uint8_t file_index = 0; file_index < atObject_Param.ftp_max_file_number_per_one_send_time[ftp_index]; file_index++)
				{
					// break in the case the unsend_file_name_list[file_index] is empty ""
					if (unsend_file_name_list[file_index] == "")
					{
						break;
					}

					// Process if unsend_file_name_list[file_index] is not empty
					String log_file_path = atObject_Param.logger_save_path[ftp_index] + "/" + unsend_file_name_list[file_index];
					// get time stemp from unsend file name
					String time_stamp_YYYY = "";
					String time_stamp_MM = "";
					String time_stamp_DD = "";
					// get the time stamp from unsend file name
					atApp_FTP.getTimeStampFromLogFileName(
						unsend_file_name_list[file_index],
						time_stamp_YYYY,
						time_stamp_MM,
						time_stamp_DD);

					// get FTP Server path from rapam
					String ftp_server_path = atObject_Param.ftp_path_list[file_index];
					ftp_server_path.replace(LOGGER_CODE_YEAR, time_stamp_YYYY);
					ftp_server_path.replace(LOGGER_CODE_MONTH, time_stamp_MM);
					ftp_server_path.replace(LOGGER_CODE_DAY, time_stamp_DD);
					// now we get the ftp server path to send the file unsend_file_name_list from log path in log_file_path
					String ftp_server_path_element_list[MAX_PATH_NUMBER] = {""};
					//split the ftp_server_path to get the element list_list[i] = "";
					atService_Tool.splitPath(
						ftp_server_path, 
						ftp_server_path_element_list, 
						MAX_PATH_NUMBER
					);
					// print ftp server path element list
					if (atApp_FTP.User_Mode == APP_USER_MODE_DEBUG)
					{
						// Serial.printf("FTP Server %d path elements for file %s:\n", ftp_index, unsend_file_name_list[file_index].c_str());
						for (uint8_t path_element_index = 0; path_element_index < MAX_PATH_NUMBER; path_element_index++)
						{
							if (ftp_server_path_element_list[path_element_index] != "")
							{
								Serial.printf(" - %s\n", ftp_server_path_element_list[path_element_index].c_str());
							}
							else
							{
								break;
							}
						}
					}
					
					// read content of file log
					// Serial.printf("Reading file %s from SD Card...\n", log_file_path.c_str());
					String file_content = atService_SD.readFile(log_file_path.c_str());
					if (file_content == "")
					{
						// if file is empty, delete it
						atService_SD.deleteFile(log_file_path.c_str());
						continue;
					}
					// Serial.printf("File %s content read successfully, size: %d bytes\n", log_file_path.c_str(), file_content.length());
					// // print file_content
					// Serial.println(file_content);

					// try 3 time to send file
					bool send_file_success = false;
					for (uint8_t the_time = 0; the_time < atObject_Param.ftp_retry_times_list[ftp_index]; the_time++)
					{
						// this bool will be stored the status
						//  Connect to FTP Server
						// Serial.printf("Connecting to FTP Server %d to send file %s (Attempt %d)...\n", ftp_index, unsend_file_name_list[file_index].c_str(), the_time + 1);
						atService_EG800K.openFTP(
							atObject_Param.ftp_host_list[ftp_index],
							atObject_Param.ftp_user_list[ftp_index],
							atObject_Param.ftp_pass_list[ftp_index],
							atObject_Param.ftp_port_list[ftp_index]);
						// Change the director to the path
						for (uint8_t path_element_index = 0; path_element_index < MAX_PATH_NUMBER; path_element_index++)
						{
							if (ftp_server_path_element_list[path_element_index] != "")
							{
								// check if ftp_server_path_element_list[path_element_index].c_str() folder is exist
								if (!atService_EG800K.changeDirectoryFTP(ftp_server_path_element_list[path_element_index].c_str()))
								{
									// if not exist, create it
									atService_EG800K.mkdirFTP(ftp_server_path_element_list[path_element_index].c_str());
									atService_EG800K.changeDirectoryFTP(ftp_server_path_element_list[path_element_index].c_str());
								}
							}
							else
								break;
						}
						// send file to the FTP Server
						// Serial.printf("Sending file %s to FTP Server %d...\n", unsend_file_name_list[file_index].c_str(), ftp_index);
						send_file_success = atService_EG800K.uploadFTP(
							unsend_file_name_list[file_index].c_str(), 
							file_content.c_str(),
							file_content.length()
						);
						// Serial.printf("Upload function returned: %s\n", send_file_success ? "Success" : "Failure");
						// wait for 3 seconds
						// Serial.printf("Waiting 3 seconds before closing FTP connection...\n");
						vTaskDelay(3000 / portTICK_PERIOD_MS);
						atService_EG800K.closeFTP();
						// Serial.printf("Sent file %s to FTP Server %d successfully\n", unsend_file_name_list[file_index].c_str(), ftp_index);
						
						// if send successfully, close FTP and break.
						if (send_file_success){
							// Move file the  SENT folder of this log.
							String new_log_path = atObject_Param.logger_save_sent_file_path[ftp_index] + "/" + unsend_file_name_list[file_index];
							// Serial.printf("Moving sent file %s to SENT folder...\n", unsend_file_name_list[file_index].c_str());
							// Serial.printf("From path: %s\n", log_file_path.c_str());
							// Serial.printf("To path: %s/%s\n", new_log_path.c_str());
							if (!atService_SD.dirExists(atObject_Param.logger_save_sent_file_path[ftp_index].c_str()))
							{
								atService_SD.createDir(atObject_Param.logger_save_sent_file_path[ftp_index].c_str());
								vTaskDelay(100 / portTICK_PERIOD_MS);
							}
							atService_SD.moveFile(
								log_file_path.c_str(),
								new_log_path.c_str()
							);
							// break the for loop
							break;
						}
					}
				}
				// Close FTP
			}
		}
		
	}
	if (atApp_FTP.User_Mode == APP_USER_MODE_DEBUG)
	{
		// print into serial rtc time
		Serial.printf("FTP executing at RTC time: %02d:%02d:%02d\n",
			atObject_Data.rtc_hour,
			atObject_Data.rtc_minute,
			atObject_Data.rtc_second);
		}
	}
	
	void App_FTP::getTimeStampFromLogFileName(String log_file_name, String &year, String &month, String &day)
	{
		// Find the last underscore before the timestamp
		int lastUnderscoreIndex = log_file_name.lastIndexOf('_');
		int dotIndex = log_file_name.lastIndexOf('.');
	
		// Extract the timestamp part (e.g. "20250903094100")
		String timestamp = log_file_name.substring(lastUnderscoreIndex + 1, dotIndex);
	
		// Ensure timestamp has at least YYYYMMDD
		if (timestamp.length() >= 8)
		{
			year = timestamp.substring(0, 4);
			month = timestamp.substring(4, 6);
			day = timestamp.substring(6, 8);
		}
		else
		{
			// fallback if invalid format
			year = "0000";
			month = "00";
			day = "00";
		}
	}

	void App_FTP::App_FTP_Suspend() {}
	void App_FTP::App_FTP_Resume() {}
	void App_FTP::App_FTP_End() {}
	void atApp_FTP_Task_Func(void *parameter)
	{
		while (1)
		{
			atApp_FTP.Run_Application(APP_RUN_MODE_AUTO);
			vTaskDelay(3000 / portTICK_PERIOD_MS);
		}
	}

#endif