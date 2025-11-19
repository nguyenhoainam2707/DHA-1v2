#ifndef _Application_atApp_RTC_v2_
#define _Application_atApp_RTC_v2_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "App.h"
#include "../src/obj/atObj_Param.h"
#include "../src/obj/atObj_Data.h"
#include "../services/atService_DS3231.h"
/* _____DEFINITIONS__________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
TaskHandle_t Task_atApp_RTC_v2;
void atApp_RTC_v2_Task_Func(void *parameter);
///////////////////////////////////////////////Testing part//
/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */

/**
 * This Application class is the application to manage the
 */
class App_RTC_v2 : public Application
{
public:
	App_RTC_v2();
	~App_RTC_v2();

protected:
private:
	static void App_RTC_v2_Pend();
	static void App_RTC_v2_Start();
	static void App_RTC_v2_Restart();
	static void App_RTC_v2_Execute();
	static void App_RTC_v2_Suspend();
	static void App_RTC_v2_Resume();
	static void App_RTC_v2_End();
} atApp_RTC_v2;
/**
 * This function will be automaticaly called when a object is created by this class
 */
App_RTC_v2::App_RTC_v2(/* args */)
{
	_Pend_User = *App_RTC_v2_Pend;
	_Start_User = *App_RTC_v2_Start;
	_Restart_User = *App_RTC_v2_Restart;
	_Execute_User = *App_RTC_v2_Execute;
	_Suspend_User = *App_RTC_v2_Suspend;
	_Resume_User = *App_RTC_v2_Resume;
	_End_User = *App_RTC_v2_End;

	// change the ID of application
	ID_Application = 1;
	// change the application name
	Name_Application = (char *)"RTC_v2 Application	";
	// change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
App_RTC_v2::~App_RTC_v2()
{
}
/**
 * Pend to start is the first task of this application it will do prerequisite condition. In the debit mode, task will send information of application to terminal to start the application.
 */
void App_RTC_v2::App_RTC_v2_Pend()
{
	// atService_DS3231.Debug();
}
/**
 * This start function will init some critical function
 */
void App_RTC_v2::App_RTC_v2_Start()
{
	// init atDS3231 Service in the fist running time
	atService_DS3231.Run_Service();
	// Automatically set time from compile timestamp
	// atService_DS3231.autoSetTimeFromCompile(atObject_Data);
	// if (atService_DS3231.setDS3231Time(atObject_Data))
	// {
	// 	if (atService_DS3231.User_Mode == SER_USER_MODE_DEBUG)
	// 	{
	// 		Serial.println("✓ DS3231 time set from compile timestamp successfully!");
	// 	}
	// }
	// else
	// {
	if(atApp_RTC_v2.User_Mode == APP_USER_MODE_DEBUG)
	{
		// atService_DS3231.Debug();
		// Serial.println("✗ Failed to set DS3231 time from compile timestamp.");
	}
	// }
}
/**
 * Restart function of SNM  app
 */
void App_RTC_v2::App_RTC_v2_Restart()
{
}
/**
 * Execute fuction of SNM app
 */
void App_RTC_v2::App_RTC_v2_Execute()
{
	atService_DS3231.readDS3231Time(atObject_Data);
	atService_DS3231.readDS3231Temperature(atObject_Data);
	if(atApp_RTC_v2.User_Mode == APP_USER_MODE_DEBUG)
    {
		// print rtc time
		Serial.printf("\t RTC date time: %d-%02d-%02d %02d:%02d:%02d\n",
					  atObject_Data.rtc_year,
					  atObject_Data.rtc_month,
					  atObject_Data.rtc_day,
					  atObject_Data.rtc_hour,
					  atObject_Data.rtc_minute,
					  atObject_Data.rtc_second);
    }
}
void App_RTC_v2::App_RTC_v2_Suspend() {}
void App_RTC_v2::App_RTC_v2_Resume() {}
void App_RTC_v2::App_RTC_v2_End() {}
void atApp_RTC_v2_Task_Func(void *parameter)
{
	while (1)
	{
		atApp_RTC_v2.Run_Application(APP_RUN_MODE_AUTO);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
#endif