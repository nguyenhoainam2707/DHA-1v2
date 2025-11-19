#ifndef _Application_atApp_MQTT_SUB_
#define _Application_atApp_MQTT_SUB_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "App.h"
#include "../services/atService_EG800K.h"
#include "../src/obj/atObj_Param.h"
#include "../src/obj/atObj_Data.h"
// #include "../src/apps/atApp_Database.h"
/* _____DEFINITIONS__________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
TaskHandle_t Task_atApp_MQTT_SUB;
void atApp_MQTT_SUB_Task_Func(void *parameter);
///////////////////////////////////////////////Testing part//
/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */

/**
 * This Application class is the application to manage the
 */
class App_MQTT_SUB : public Application
{
public:
	App_MQTT_SUB();
	~App_MQTT_SUB();

protected:
private:
	static void App_MQTT_SUB_Pend();
	static void App_MQTT_SUB_Start();
	static void App_MQTT_SUB_Restart();
	static void App_MQTT_SUB_Execute();
	static void App_MQTT_SUB_Suspend();
	static void App_MQTT_SUB_Resume();
	static void App_MQTT_SUB_End();
} atApp_MQTT_SUB;
/**
 * This function will be automaticaly called when a object is created by this class
 */
App_MQTT_SUB::App_MQTT_SUB(/* args */)
{
	_Pend_User = *App_MQTT_SUB_Pend;
	_Start_User = *App_MQTT_SUB_Start;
	_Restart_User = *App_MQTT_SUB_Restart;
	_Execute_User = *App_MQTT_SUB_Execute;
	_Suspend_User = *App_MQTT_SUB_Suspend;
	_Resume_User = *App_MQTT_SUB_Resume;
	_End_User = *App_MQTT_SUB_End;

	// change the ID of application
	ID_Application = 3;
	// change the application name
	Name_Application = (char *)"MQTT_SUB Application	";
	// change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
App_MQTT_SUB::~App_MQTT_SUB()
{
}
/**
 * Pend to start is the first task of this application it will do prerequisite condition. In the debit mode, task will send information of application to terminal to start the application.
 */
void App_MQTT_SUB::App_MQTT_SUB_Pend()
{
	// atService_EG800K.Debug();
}
/**
 * This start function will init some critical function
 */
void App_MQTT_SUB::App_MQTT_SUB_Start()
{
	// Init atEG800K Service in the fist running time
	atService_EG800K.Run_Service();
	Service_EG800K::initEG800K();
	Service_EG800K::setupMQTT();
	// Subscribe to MQTT topic
	Service_EG800K::subscribeMQTTTopic("esp32s3/content");
}
/**
 * Restart function of SNM  app
 */
void App_MQTT_SUB::App_MQTT_SUB_Restart()
{
}
/**
 * Execute fuction of SNM app
 */
void App_MQTT_SUB::App_MQTT_SUB_Execute()
{
	Service_EG800K::handleMQTTMessage();
}
void App_MQTT_SUB::App_MQTT_SUB_Suspend() {}
void App_MQTT_SUB::App_MQTT_SUB_Resume() {}
void App_MQTT_SUB::App_MQTT_SUB_End() {}
void atApp_MQTT_SUB_Task_Func(void *parameter)
{
	while (1)
	{
		atApp_MQTT_SUB.Run_Application(APP_RUN_MODE_AUTO);
		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
}
#endif