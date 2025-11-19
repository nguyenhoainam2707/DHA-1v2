#ifndef _Application_atApp_BT_
#define _Application_atApp_BT_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "App.h"
#include "../src/obj/atObj_Param.h"
#include "../src/obj/atObj_Data.h"
// #include "../src/apps/atApp_Database.h"
/* _____DEFINITIONS__________________________________________________________ */
#define BUTTON_PIN 41

/* _____GLOBAL VARIABLES_____________________________________________________ */
TaskHandle_t Task_atApp_BT;
void atApp_BT_Task_Func(void *parameter);
// FreeRTOS objects
QueueHandle_t buttonQueue;
///////////////////////////////////////////////Testing part//
/* _____GLOBAL FUNCTION______________________________________________________ */
// ISR for button press
void IRAM_ATTR buttonISR()
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	//   bool state = true; // just a flag
	if (digitalRead(BUTTON_PIN) == LOW)
	{
		if (xTaskGetTickCount() - atObject_Data.lastButtonPressedTime < 1000)
		{
			// Increment button click count
			atObject_Data.buttonClickCount++;
			Serial.printf("Button clicked - Count: %d\n", atObject_Data.buttonClickCount);
		}
		else
		{
			atObject_Data.buttonClickCount = 1; // reset to 1 if within debounce time
		}
		atObject_Data.lastButtonPressedTime = xTaskGetTickCount();
		atObject_Data.lastButtonPressedTime_seconds = (xTaskGetTickCount() / 1000) % 60;
		atObject_Data.lastButtonPressedTime_minutes = (xTaskGetTickCount() / 60000) % 60;
		// atObject_Data.flagButtonHeldSecond[0] = true;
		atObject_Data.buttonPressedTimeNumber++;
		Serial.printf("Button ISR triggered - Count: %d\n", atObject_Data.buttonPressedTimeNumber);
		// time pressed atObject_Data.lastButtonPressedTime
		Serial.printf("atObject_Data.lastButtonPressedTime: %u ticks\n", atObject_Data.lastButtonPressedTime);
		// Update last pressed time
	}
	//   xQueueSendFromISR(buttonQueue, &state, &xHigherPriorityTaskWoken);
	if (xHigherPriorityTaskWoken)
	{
		portYIELD_FROM_ISR();
	}
}
/* _____CLASS DEFINITION_____________________________________________________ */

/**
 * This Application class is the application to manage the
 */
class App_BT : public Application
{
public:
	App_BT();
	~App_BT();
	const TickType_t debounceDelay = pdMS_TO_TICKS(50);	  // debounce time
	const TickType_t holdThreshold = pdMS_TO_TICKS(5000); // 5 seconds
	TickType_t pressStartTime = 0;
	bool buttonPressed = false;

protected:
private:
	static void App_BT_Pend();
	static void App_BT_Start();
	static void App_BT_Restart();
	static void App_BT_Execute();
	static void App_BT_Suspend();
	static void App_BT_Resume();
	static void App_BT_End();
} atApp_BT;
/**
 * This function will be automaticaly called when a object is created by this class
 */
App_BT::App_BT(/* args */)
{
	_Pend_User = *App_BT_Pend;
	_Start_User = *App_BT_Start;
	_Restart_User = *App_BT_Restart;
	_Execute_User = *App_BT_Execute;
	_Suspend_User = *App_BT_Suspend;
	_Resume_User = *App_BT_Resume;
	_End_User = *App_BT_End;

	// change the ID of application
	ID_Application = 1;
	// change the application name
	Name_Application = (char *)"BT Application	";
	// change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
App_BT::~App_BT()
{
}
/**
 * Pend to start is the first task of this application it will do prerequisite condition. In the debit mode, task will send information of application to terminal to start the application.
 */
void App_BT::App_BT_Pend()
{
	// atService_XYZ.Debug();
}
/**
 * This start function will init some critical function
 */
void App_BT::App_BT_Start()
{
	pinMode(BUTTON_PIN, INPUT_PULLUP);

	// Create the queue (length 10, element size of bool)
	buttonQueue = xQueueCreate(10, sizeof(bool));

	// Attach the interrupt
	attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);
}
/**
 * Restart function of SNM  app
 */
void App_BT::App_BT_Restart()
{
	pinMode(BUTTON_PIN, INPUT_PULLUP);
}
/**
 * Execute fuction of SNM app
 */
void App_BT::App_BT_Execute()
{
	// check if Button is being released and have been pressed
	if (digitalRead(BUTTON_PIN) == LOW)
	{
		// check if Button is being released and have been pressed
		if ((!atApp_BT.buttonPressed))
		{
			// when the button is pressed delay 100ms to check again if the button
			// is still pressed
			vTaskDelay(atApp_BT.debounceDelay);
			if ((digitalRead(BUTTON_PIN) == LOW) && (!atApp_BT.buttonPressed))
			{
				atApp_BT.buttonPressed = true;
				atApp_BT.pressStartTime = xTaskGetTickCount();
			}
		}
	}
	else
	{
		atApp_BT.buttonPressed = false;
	}

	if (atApp_BT.buttonPressed)
	{
		for (uint8_t second = 0; second < MAX_BUTTON_HOLD_TIME_SECONDS; second++)
		{
			if (xTaskGetTickCount() - atApp_BT.pressStartTime > pdMS_TO_TICKS(second * 1000))
			{
				atObject_Data.flagButtonHeldSecond[second] = true;
			}
			else
			{
				atObject_Data.flagButtonHeldSecond[second] = false;
			}
		}
	}
	else
	{
		// reset all flags when button is released
		for (uint8_t second = 0; second < MAX_BUTTON_HOLD_TIME_SECONDS; second++)
		{
			atObject_Data.flagButtonHeldSecond[second] = false;
		}
	}

	if (atApp_BT.User_Mode == APP_USER_MODE_DEBUG)
	{
		// print button status
		Serial.printf("\t Button pressed: %d", atApp_BT.buttonPressed);
		for (uint8_t second = 0; second < MAX_BUTTON_HOLD_TIME_SECONDS; second++)
		{
			Serial.printf(" %d", atObject_Data.flagButtonHeldSecond[second]);
		}
		Serial.println();
	}
}
void App_BT::App_BT_Suspend() {}
void App_BT::App_BT_Resume() {}
void App_BT::App_BT_End() {}
void atApp_BT_Task_Func(void *parameter)
{
	while (1)
	{
		atApp_BT.Run_Application(APP_RUN_MODE_AUTO);
		vTaskDelay(900 / portTICK_PERIOD_MS);
	}
}
#endif