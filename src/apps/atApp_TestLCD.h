#ifndef _Application_atApp_TestLCD_
#define _Application_atApp_TestLCD_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "App.h"
#include "../services/atService_LCD1602.h"
#include "../src/obj/atObj_Param.h"
#include "../src/obj/atObj_Data.h"
// #include "../src/apps/atApp_Database.h"
/* _____DEFINITIONS__________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
TaskHandle_t Task_atApp_TestLCD;  
void atApp_TestLCD_Task_Func(void *parameter);
///////////////////////////////////////////////Testing part//
/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */

/**
 * This Application class is the application to manage the 
 */
class App_TestLCD : public Application
{
public:
  	App_TestLCD();
 	~App_TestLCD();
protected:
private:
  	static void  App_TestLCD_Pend();
	static void  App_TestLCD_Start();
	static void  App_TestLCD_Restart();
	static void  App_TestLCD_Execute();
	static void  App_TestLCD_Suspend();
	static void  App_TestLCD_Resume();	  
	static void  App_TestLCD_End();
} atApp_TestLCD ;
/**
 * This function will be automaticaly called when a object is created by this class
 */
App_TestLCD::App_TestLCD(/* args */)
{
  	_Pend_User 	     = *App_TestLCD_Pend;
	_Start_User 	 = *App_TestLCD_Start;
	_Restart_User 	 = *App_TestLCD_Restart;
	_Execute_User 	 = *App_TestLCD_Execute;
	_Suspend_User	 = *App_TestLCD_Suspend;
	_Resume_User	 = *App_TestLCD_Resume;
	_End_User	     = *App_TestLCD_End;

	// change the ID of application
	ID_Application = 1;
	// change the application name
	Name_Application = (char*)"TestLCD Application	";
	// change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
App_TestLCD::~App_TestLCD()
{
	
}
/**
 * Pend to start is the first task of this application it will do prerequisite condition. In the debit mode, task will send information of application to terminal to start the application.
 */
void  App_TestLCD::App_TestLCD_Pend()
{
    // atService_LCD1602.Debug();
}
/**
 * This start function will init some critical function 
 */
void  App_TestLCD::App_TestLCD_Start()
{
	// init atLCD1602 Service in the fist running time
	if(atApp_TestLCD.User_Mode == APP_USER_MODE_DEBUG)
	{
		atService_LCD1602.Debug();
	}
	atService_LCD1602.Run_Service();
}  
/**
 * Restart function of SNM  app
 */
void  App_TestLCD::App_TestLCD_Restart()
{

}
/**
 * Execute fuction of SNM app
 */
void  App_TestLCD::App_TestLCD_Execute()
{	
	// ==================== Test 1: Basic displayText with alignment ====================
	atService_LCD1602.clearLine(LCD_LINE_BOTH);
	atService_LCD1602.displayText("Left Align", LCD_LINE_1, LCD_ALIGN_LEFT, false);
	vTaskDelay(2000 / portTICK_PERIOD_MS);

	atService_LCD1602.clearLine(LCD_LINE_BOTH);
	atService_LCD1602.displayText("Center Align", LCD_LINE_1, LCD_ALIGN_CENTER, false);
	vTaskDelay(2000 / portTICK_PERIOD_MS);

	// atService_LCD1602.clearLine(LCD_LINE_BOTH);
	// atService_LCD1602.displayText("Right Align", LCD_LINE_1, LCD_ALIGN_RIGHT, false);
	// vTaskDelay(2000 / portTICK_PERIOD_MS);

	// // ==================== Test 2: displayText with auto-scroll (long text) ====================
	// atService_LCD1602.clearLine(LCD_LINE_BOTH);
	// atService_LCD1602.displayText("This is a very long text that will auto-scroll", LCD_LINE_1, LCD_ALIGN_LEFT, true, 300);
	// vTaskDelay(1000 / portTICK_PERIOD_MS);

	// // ==================== Test 3: displayTwoLines with alignment ====================
	// atService_LCD1602.clearLine(LCD_LINE_BOTH);
	// atService_LCD1602.displayTwoLines("Line 1 Left", "Line 2 Right", LCD_ALIGN_LEFT, LCD_ALIGN_RIGHT, false);
	// vTaskDelay(2000 / portTICK_PERIOD_MS);

	// atService_LCD1602.clearLine(LCD_LINE_BOTH);
	// atService_LCD1602.displayTwoLines("Centered 1", "Centered 2", LCD_ALIGN_CENTER, LCD_ALIGN_CENTER, false);
	// vTaskDelay(2000 / portTICK_PERIOD_MS);

	// // ==================== Test 4: displayTwoLines with auto-scroll ====================
	// atService_LCD1602.clearLine(LCD_LINE_BOTH);
	// atService_LCD1602.displayTwoLines("Short line", "This is a very long second line that will scroll", 
	//                                   LCD_ALIGN_LEFT, LCD_ALIGN_LEFT, true, 300);
	// vTaskDelay(1000 / portTICK_PERIOD_MS);

	// // ==================== Test 5: scrollText function ====================
	// atService_LCD1602.clearLine(LCD_LINE_BOTH);
	// atService_LCD1602.scrollText("Manual scroll test - This text is scrolling manually", LCD_LINE_1, 250);
	// vTaskDelay(1000 / portTICK_PERIOD_MS);

	// // ==================== Test 6: blinkText function ====================
	// atService_LCD1602.clearLine(LCD_LINE_BOTH);
	// atService_LCD1602.blinkText("Blinking Text!", LCD_LINE_1, LCD_ALIGN_CENTER, 3, 500);
	// vTaskDelay(1000 / portTICK_PERIOD_MS);

	// // ==================== Test 7: showLoading animation ====================
	// atService_LCD1602.clearLine(LCD_LINE_BOTH);
	// atService_LCD1602.showLoading("Loading", 3);
	// vTaskDelay(1000 / portTICK_PERIOD_MS);

	// atService_LCD1602.clearLine(LCD_LINE_BOTH);
	// atService_LCD1602.showLoading("Please Wait", 3);
	// vTaskDelay(1000 / portTICK_PERIOD_MS);

	// // ==================== Test 8: showProgressBar ====================
	// atService_LCD1602.clearLine(LCD_LINE_BOTH);
	// atService_LCD1602.displayText("Progress Test", LCD_LINE_1, LCD_ALIGN_CENTER, false);
	// for(uint8_t i = 0; i <= 100; i += 10)
	// {
	// 	atService_LCD1602.showProgressBar(i, LCD_LINE_2, true);
	// 	vTaskDelay(500 / portTICK_PERIOD_MS);
	// }
	// vTaskDelay(1000 / portTICK_PERIOD_MS);

	// // ==================== Test 9: Progress bar without percentage ====================
	// atService_LCD1602.clearLine(LCD_LINE_BOTH);
	// atService_LCD1602.displayText("Full Width Bar", LCD_LINE_1, LCD_ALIGN_CENTER, false);
	// for(uint8_t i = 0; i <= 100; i += 20)
	// {
	// 	atService_LCD1602.showProgressBar(i, LCD_LINE_2, false);
	// 	vTaskDelay(500 / portTICK_PERIOD_MS);
	// }
	// vTaskDelay(1000 / portTICK_PERIOD_MS);

	// // ==================== Test 10: setBacklight function ====================
	// atService_LCD1602.clearLine(LCD_LINE_BOTH);
	// atService_LCD1602.displayText("Backlight Test", LCD_LINE_1, LCD_ALIGN_CENTER, false);
	// vTaskDelay(1000 / portTICK_PERIOD_MS);
	
	// atService_LCD1602.setBacklight(false);
	// vTaskDelay(1000 / portTICK_PERIOD_MS);
	
	// atService_LCD1602.setBacklight(true);
	// vTaskDelay(1000 / portTICK_PERIOD_MS);

	// // ==================== Test 11: clearLine function ====================
	// atService_LCD1602.clearLine(LCD_LINE_BOTH);
	// atService_LCD1602.displayTwoLines("Line 1 Text", "Line 2 Text", LCD_ALIGN_LEFT, LCD_ALIGN_LEFT, false);
	// vTaskDelay(2000 / portTICK_PERIOD_MS);

	// atService_LCD1602.clearLine(LCD_LINE_1);
	// vTaskDelay(1000 / portTICK_PERIOD_MS);

	// atService_LCD1602.clearLine(LCD_LINE_2);
	// vTaskDelay(1000 / portTICK_PERIOD_MS);

	// ==================== Test 12: displayText with both lines using '|' separator ====================
	atService_LCD1602.clearLine(LCD_LINE_BOTH);
	atService_LCD1602.displayText("Device Ready|System OK", LCD_LINE_BOTH, LCD_ALIGN_CENTER, false);
	vTaskDelay(2000 / portTICK_PERIOD_MS);

	// ==================== Test 13: displayTextAt function ====================
	// atService_LCD1602.clearLine(LCD_LINE_BOTH);
	atService_LCD1602.displayTextAt("Device None", 5, 0);
	atService_LCD1602.displayTextAt("System OK", 5, 1);
	vTaskDelay(2000 / portTICK_PERIOD_MS);

	// ==================== Test Complete ====================
	atService_LCD1602.clearLine(LCD_LINE_BOTH);
	atService_LCD1602.displayTwoLines("Test Complete!", "All Functions OK", LCD_ALIGN_CENTER, LCD_ALIGN_CENTER, false);
	vTaskDelay(3000 / portTICK_PERIOD_MS);
}
void  App_TestLCD::App_TestLCD_Suspend(){}
void  App_TestLCD::App_TestLCD_Resume(){}	  
void  App_TestLCD::App_TestLCD_End(){}
void atApp_TestLCD_Task_Func(void *parameter)
{
  while (1)
  {
    atApp_TestLCD.Run_Application(APP_RUN_MODE_AUTO);
    vTaskDelay(1000/ portTICK_PERIOD_MS);
  }
}
#endif