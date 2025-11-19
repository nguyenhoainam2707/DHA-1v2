#ifndef _Application_atApp_TestFTP_
#define _Application_atApp_TestFTP_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "App.h"
#include "../services/atService_EG800K.h"
#include "../src/obj/atObj_Param.h"
#include "../src/obj/atObj_Data.h"
/* _____DEFINITIONS__________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
TaskHandle_t Task_atApp_TestFTP;
void atApp_TestFTP_Task_Func(void *parameter);
///////////////////////////////////////////////Testing part//
/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */

/**
 * This Application class is the application to manage the
 */
class App_TestFTP : public Application
{
public:
	App_TestFTP();
	~App_TestFTP();

protected:
private:
	static void App_TestFTP_Pend();
	static void App_TestFTP_Start();
	static void App_TestFTP_Restart();
	static void App_TestFTP_Execute();
	static void App_TestFTP_Suspend();
	static void App_TestFTP_Resume();
	static void App_TestFTP_End();
} atApp_TestFTP;
/**
 * This function will be automaticaly called when a object is created by this class
 */
App_TestFTP::App_TestFTP(/* args */)
{
	_Pend_User = *App_TestFTP_Pend;
	_Start_User = *App_TestFTP_Start;
	_Restart_User = *App_TestFTP_Restart;
	_Execute_User = *App_TestFTP_Execute;
	_Suspend_User = *App_TestFTP_Suspend;
	_Resume_User = *App_TestFTP_Resume;
	_End_User = *App_TestFTP_End;

	// change the ID of application
	ID_Application = 1;
	// change the application name
	Name_Application = (char *)"TestFTP Application	";
	// change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
App_TestFTP::~App_TestFTP()
{
}
/**
 * Pend to start is the first task of this application it will do prerequisite condition. In the debit mode, task will send information of application to terminal to start the application.
 */
void App_TestFTP::App_TestFTP_Pend()
{
	// atService_EG800K.Debug();
}
/**
 * This start function will init some critical function
 */
void App_TestFTP::App_TestFTP_Start()
{
	atObject_Data.ftpConnected = false;
	// init atEG800K Service in the fist running time
	atService_EG800K.Run_Service();
	if (!atService_EG800K.initEG800K())
	{
		Serial.println("Failed to initialize EG800K module");
		vTaskSuspend(NULL);
		return;
	}
	
	Serial.println("\n");
	Serial.println("╔══════════════════════════════════════════════╗");
	Serial.println("║     ESP32S3 FTP COMPLETE FUNCTION TEST       ║");
	Serial.println("╚══════════════════════════════════════════════╝");
	Serial.println("\n");

	if (!atService_EG800K.openFTP("103.149.86.230", "ftpuser", "abc123123", 21))
	{
		Serial.println("Failed to open FTP");
		vTaskSuspend(NULL);
		return;
	}

}
/**
 * Restart function of SNM  app
 */
void App_TestFTP::App_TestFTP_Restart()
{
}
/**
 * Execute fuction of SNM app
 */
void App_TestFTP::App_TestFTP_Execute()
{
	if (atApp_TestFTP.User_Mode == APP_USER_MODE_DEBUG)
	{
	}
}
void App_TestFTP::App_TestFTP_Suspend() {}
void App_TestFTP::App_TestFTP_Resume() {}
void App_TestFTP::App_TestFTP_End() {}
void atApp_TestFTP_Task_Func(void *parameter)
{
	while (1)
	{
		atApp_TestFTP.Run_Application(APP_RUN_MODE_AUTO);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
#endif