#ifndef _Application_atApp_MQTT_PUB_
#define _Application_atApp_MQTT_PUB_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "App.h"
#include "../services/atService_EG800K.h"
#include "../src/obj/atObj_Param.h"
#include "../src/obj/atObj_Data.h"
/* _____DEFINITIONS__________________________________________________________ */
// #define MQTT_PUB_TOPIC "esp32s3/testTopicDHA" // MQTT topic for publishing water level data
/* _____GLOBAL VARIABLES_____________________________________________________ */
TaskHandle_t Task_atApp_MQTT_PUB;
void atApp_MQTT_PUB_Task_Func(void *parameter);
///////////////////////////////////////////////Testing part//
/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */

/**
 * This Application class is the application to manage the
 */
class App_MQTT_PUB : public Application
{
public:
	App_MQTT_PUB();
	~App_MQTT_PUB();

protected:
private:
	static void App_MQTT_PUB_Pend();
	static void App_MQTT_PUB_Start();
	static void App_MQTT_PUB_Restart();
	static void App_MQTT_PUB_Execute();
	static void App_MQTT_PUB_Suspend();
	static void App_MQTT_PUB_Resume();
	static void App_MQTT_PUB_End();
} atApp_MQTT_PUB;
/**
 * This function will be automaticaly called when a object is created by this class
 */
App_MQTT_PUB::App_MQTT_PUB(/* args */)
{
	_Pend_User = *App_MQTT_PUB_Pend;
	_Start_User = *App_MQTT_PUB_Start;
	_Restart_User = *App_MQTT_PUB_Restart;
	_Execute_User = *App_MQTT_PUB_Execute;
	_Suspend_User = *App_MQTT_PUB_Suspend;
	_Resume_User = *App_MQTT_PUB_Resume;
	_End_User = *App_MQTT_PUB_End;

	// change the ID of application
	ID_Application = 2;
	// change the application name
	Name_Application = (char *)"MQTT_PUB Application	";
	// change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
App_MQTT_PUB::~App_MQTT_PUB()
{
}
/**
 * Pend to start is the first task of this application it will do prerequisite condition. In the debit mode, task will send information of application to terminal to start the application.
 */
void App_MQTT_PUB::App_MQTT_PUB_Pend()
{
	// atService_EG800K.Debug();
}
/**
 * This start function will init some critical function
 */
void App_MQTT_PUB::App_MQTT_PUB_Start()
{

	atObject_Data.mqtt_payload = "{";
	atObject_Data.mqtt_payload += "\"temperature\":" + String(19.9) + ",";
	atObject_Data.mqtt_payload += "\"humidity\":" + String(50.5) + ",";
	atObject_Data.mqtt_payload += "\"light\":" + String(300) + ",";
	atObject_Data.mqtt_payload += "\"timestamp\":" + String(xTaskGetTickCount());
	atObject_Data.mqtt_payload += "}";
	atObject_Data.mqttConnected = false;
	atObject_Data.simReady = false;

	// init atEG800K Service in the fist running time
	atService_EG800K.Run_Service();
	if (!atService_EG800K.initEG800K())
	{
		Serial.println("Failed to initialize EG800K module");
		vTaskSuspend(NULL);
		return;
	}
	if (!atService_EG800K.setupMQTT())
	{
		Serial.println("Failed to setup MQTT");
		vTaskSuspend(NULL);
		return;
	}
}
/**
 * Restart function of SNM  app
 */
void App_MQTT_PUB::App_MQTT_PUB_Restart()
{
}
/**
 * Execute fuction of SNM app
 */
void App_MQTT_PUB::App_MQTT_PUB_Execute()
{
	if (atObject_Data.mqttConnected)
	{
		// Publish
		if (atService_EG800K.publishMQTT(atObject_Param.mqtt_pub_topic.c_str(), atObject_Data.mqtt_payload.c_str()))
		{
			Serial.println("Published: " + atObject_Data.mqtt_payload);
		}
		else
		{
			Serial.println("Publish failed");
		}
	}
}
void App_MQTT_PUB::App_MQTT_PUB_Suspend() {}
void App_MQTT_PUB::App_MQTT_PUB_Resume() {}
void App_MQTT_PUB::App_MQTT_PUB_End() {}
void atApp_MQTT_PUB_Task_Func(void *parameter)
{
	while (1)
	{
		TickType_t xLastWakeTime = xTaskGetTickCount();
		atApp_MQTT_PUB.Run_Application(APP_RUN_MODE_AUTO);
		// Đợi theo chu kỳ
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(atObject_Param.mqtt_sending_interval * 1000));
	}
}
#endif