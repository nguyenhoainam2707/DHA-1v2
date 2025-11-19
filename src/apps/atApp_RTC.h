/**
@file
Application for communicating with OP320A&S SNM over RS232/485 (via RTU protocol).
*/
/*
  Application_SNM.h - Arduino library for communicating with OP320A&S SNM
  over RS232/485 (via RTU protocol).

  Library:: chickyPig_OP320_V1

  This version is

  Copyright:: 2021 nguyentrinhtuan1996@gmail.com
*/

#ifndef _Application_atApp_RTC_
#define _Application_atApp_RTC_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "App.h"
#include "RTClib.h"
#include "services/i2c/atService_I2C.h"
#include "../src/obj/atObj_Data.h"
// inclide Wire1 from Wire.h
#include <Wire.h>
/* _____DEFINITIONS__________________________________________________________ */
enum Day_Of_Week
{
  MONDAY,
  TUESDAY,
  WEDNESDAY,
  THURSDAY,
  FRIDAY,
  SATURDAY,
  SUNDAY
};
/* _____GLOBAL VARIABLES_____________________________________________________ */
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
// RTC_DS3231 rtc;
///////////////////////////////////////////////Testing part//
/* _____GLOBAL FUNCTION______________________________________________________ */
TaskHandle_t Task_atApp_RTC;
void atApp_RTC_Task_Func(void *parameter);
/* _____CLASS DEFINITION_____________________________________________________ */
/**
 * This Application class is the application to manage the
 */
class App_RTC : public Application, public RTC_DS3231
{
public:
  App_RTC();
  ~App_RTC();

  void update_Timer(uint8_t hour, uint8_t minute, uint8_t second, uint8_t day, uint8_t month, uint16_t year);

protected:
  static void App_RTC_Pend();
  static void App_RTC_Start();
  static void App_RTC_Restart();
  static void App_RTC_Execute();
  static void App_RTC_Suspend();
  static void App_RTC_Resume();
  static void App_RTC_End();

private:
} atApp_RTC;
/**
 * This function will be automatically called when a object is created by this class
 */
App_RTC::App_RTC(/* args */)
{
  _Pend_User = *App_RTC_Pend;
  _Start_User = *App_RTC_Start;
  _Restart_User = *App_RTC_Restart;
  _Execute_User = *App_RTC_Execute;
  _Suspend_User = *App_RTC_Suspend;
  _Resume_User = *App_RTC_Resume;
  _End_User = *App_RTC_End;

  // change the ID of application
  ID_Application = 3;
  // change the application name
  Name_Application = (char *)"RTC_Application";
  // change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
App_RTC::~App_RTC()
{
}
/**
 * Pend to start is the first task of this application it will do prerequisite condition. In the debug mode, task will send information of application to terminal to start the application.
 */
void App_RTC::App_RTC_Pend()
{
}
/**
 * This start function will init some critical function
 */
void App_RTC::App_RTC_Start()
{
  atService_I2C.Run_Service();

  atService_I2C.checkIn();
  if (!atApp_RTC.begin(&Wire1))
  {
    if (atApp_RTC.User_Mode == APP_USER_MODE_DEBUG)
    {
      Serial.println("Couldn't find RTC");
      Serial.flush();
      abort();
    }
  }

  if (atApp_RTC.lostPower())
  {
    if (atApp_RTC.User_Mode == APP_USER_MODE_DEBUG)
    {
      Serial.println("RTC lost power, let's set the time!");
    }
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    atApp_RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  atService_I2C.checkOut();
}
/**
 * Restart function of SNM  app
 */
void App_RTC::App_RTC_Restart()
{
}
/**
 * Execute fuction of SNM app
 */
void App_RTC::App_RTC_Execute()
{
  atService_I2C.checkIn();
  DateTime now = atApp_RTC.now(); // get the current time
  atService_I2C.checkOut();

  if (now.hour() <= 24)
  {
    atObject_Data.rtc_hour = now.hour();
  }

  if (now.minute() <= 60)
  {
    atObject_Data.rtc_minute = now.minute();
  }

  if (now.second() <= 60)
  {
    atObject_Data.rtc_second = now.second();
  }

  if (now.day() <= 31)
  {
    atObject_Data.rtc_day = now.day();
  }

  if (now.month() <= 12)
  {
    atObject_Data.rtc_month = now.month();
    atObject_Data.rtc_year = (int)now.year();
    atObject_Data.rtc_day_of_week = now.dayOfTheWeek();

    atService_I2C.checkIn();
    atObject_Data.rtc_temperature = atApp_RTC.getTemperature();
    atService_I2C.checkOut();
  }

  if (atApp_RTC.User_Mode == APP_USER_MODE_DEBUG)
  {
    Serial.print("  Present time: ");
    Serial.print(daysOfTheWeek[atObject_Data.rtc_day_of_week]);
    Serial.printf(" - %d:%d:%d - %d/%d/%d \n", atObject_Data.rtc_hour, atObject_Data.rtc_minute, atObject_Data.rtc_second, atObject_Data.rtc_day, atObject_Data.rtc_month, atObject_Data.rtc_year);
    Serial.print("  Temperature: ");
    Serial.print(atObject_Data.rtc_temperature);
    Serial.println(" C");
    Serial.println();
  }
}
void App_RTC::App_RTC_Suspend() {}
void App_RTC::App_RTC_Resume() {}
void App_RTC::App_RTC_End() {}

void App_RTC::update_Timer(uint8_t hour, uint8_t minute, uint8_t second, uint8_t day, uint8_t month, uint16_t year)
{
  atApp_RTC.adjust(DateTime(year, month, day, hour, minute, second));
}
void atApp_RTC_Task_Func(void *parameter)
{
  while (1)
  {
    atApp_RTC.Run_Application(APP_RUN_MODE_AUTO);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
#endif