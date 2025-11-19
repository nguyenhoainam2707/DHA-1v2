#ifndef _Application_atApp_LED_
#define _Application_atApp_LED_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "App.h"
#include "../services/atService_PCF8574.h"
#include "../src/obj/atObj_Param.h"
#include "../src/obj/atObj_Data.h"
// #include "../src/apps/atApp_Database.h"
/* _____DEFINITIONS__________________________________________________________ */
const int LED_dataPin = 3;	 // DS
const int LED_latchPin = 46;	 // ST_CP
const int LED_clockPin = 9; // SH_CP

#define LED_6 0
#define LED_9 1
#define LED_2 2
#define LED_5 3
#define LED_8 4
#define LED_1 5
#define LED_4 6
#define LED_7 7



/* _____GLOBAL VARIABLES_____________________________________________________ */
TaskHandle_t Task_atApp_LED;
void atApp_LED_Task_Func(void *parameter);
///////////////////////////////////////////////Testing part//
/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */

/**
 * This Application class is the application to manage the
 */
class App_LED : public Application
{
public:
	App_LED();
	~App_LED();

protected:
private:
	static void App_LED_Pend();
	static void App_LED_Start();
	static void App_LED_Restart();
	static void App_LED_Execute();
	static void App_LED_Suspend();
	static void App_LED_Resume();
	static void App_LED_End();
	static void updateShiftRegister();
	static void setLed(int8_t led, bool state);
	static void turn_led(int8_t led, bool state);
	static void turn_all_leds(bool state);
	// Store current LED states (1 byte = 8 LEDs)
	byte ledState = 0b00000000;
} atApp_LED;
/**
 * This function will be automaticaly called when a object is created by this class
 */
App_LED::App_LED(/* args */)
{
	_Pend_User = *App_LED_Pend;
	_Start_User = *App_LED_Start;
	_Restart_User = *App_LED_Restart;
	_Execute_User = *App_LED_Execute;
	_Suspend_User = *App_LED_Suspend;
	_Resume_User = *App_LED_Resume;
	_End_User = *App_LED_End;

	// change the ID of application
	ID_Application = 1;
	// change the application name
	Name_Application = (char *)"LED Application	";
	// change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
App_LED::~App_LED()
{
}
/**
 * Pend to start is the first task of this application it will do prerequisite condition. In the debit mode, task will send information of application to terminal to start the application.
 */
void App_LED::App_LED_Pend()
{
	// atService_PCF8574.Debug();
}
/**
 * This start function will init some critical function
 */
void App_LED::App_LED_Start()
{
	// init atPCF8574 Service in the fist running time
	// atService_PCF8574.Run_Service();
	pinMode(LED_dataPin, OUTPUT);
	pinMode(LED_clockPin, OUTPUT);
	pinMode(LED_latchPin, OUTPUT);
	// turn off all leds at start
	atApp_LED.ledState = 0b11111111;
	atApp_LED.updateShiftRegister();
	// Initial test sequence
	for (int8_t i = 0; i < 8; i++)
	{
		atApp_LED.turn_led(i, false);
		delay(200);
		atApp_LED.turn_led(i, true);
	}
	// Set all LEDs off at the end of test
	atApp_LED.ledState = 0b11111111;
	atApp_LED.updateShiftRegister();

}
/**
 * Restart function of SNM  app
 */
void App_LED::App_LED_Restart()
{
}
/**
 * Execute fuction of SNM app
 */
void App_LED::App_LED_Execute()
{
	// atService_PCF8574.Run_Service();
	if (atObject_Data.status_led[LED_SIM_STATUS]) 		atApp_LED.turn_led(LED_1, true);
	if (atObject_Data.status_led[LED_ACCESS_POINT]) 	atApp_LED.turn_led(LED_2, true);
	// if (atObject_Data.status_led[LED_POWER]) 			atApp_LED.turn_led(LED_3, true);
	if (atObject_Data.status_led[LED_RS485_CHANNEL_1]) 	atApp_LED.turn_led(LED_4, true);
	if (atObject_Data.status_led[LED_RS485_CHANNEL_2]) 	atApp_LED.turn_led(LED_5, true);
	if (atObject_Data.status_led[LED_FTP]) 				atApp_LED.turn_led(LED_6, true);
	if (atObject_Data.status_led[LED_TCPIP]) 			atApp_LED.turn_led(LED_7, true);
	if (atObject_Data.status_led[LED_LOGGER]) 			atApp_LED.turn_led(LED_8, true);
	if (atObject_Data.status_led[LED_SD_CARD]) 			atApp_LED.turn_led(LED_9, true);
	vTaskDelay(1000 / portTICK_PERIOD_MS);
	atApp_LED.turn_all_leds(false);

	

	if (atApp_LED.User_Mode == APP_USER_MODE_DEBUG)
	{
	}
}
void App_LED::App_LED_Suspend() {}
void App_LED::App_LED_Resume() {}
void App_LED::App_LED_End() {}
void App_LED::setLed(int8_t led, bool state)
{
	if (led < 0 || led > 7)
		return; // Safety check

	if (state)
		atApp_LED.ledState |= (1 << led); // Set bit (turn LED ON)
	else
		atApp_LED.ledState &= ~(1 << led); // Clear bit (turn LED OFF)

	atApp_LED.updateShiftRegister(); // Send data to 74HC595
}
void App_LED::updateShiftRegister()
{
	digitalWrite(LED_latchPin, LOW);
	shiftOut(LED_dataPin, LED_clockPin, MSBFIRST, atApp_LED.ledState);
	digitalWrite(LED_latchPin, HIGH);
}
void App_LED::turn_led(int8_t led, bool state)
{
	atApp_LED.setLed(led, !state);
}
void App_LED::turn_all_leds(bool state)
{
	for (int8_t i = 0; i < 8; i++)
	{
		atApp_LED.setLed(i, !state);
	}
}

void atApp_LED_Task_Func(void *parameter)
{
	while (1)
	{
		atApp_LED.Run_Application(APP_RUN_MODE_AUTO);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
#endif