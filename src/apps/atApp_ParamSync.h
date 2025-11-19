#ifndef _Application_atApp_ParamSync_
#define _Application_atApp_ParamSync_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "App.h"
#include "../src/obj/atObj_Param.h"
#include "../src/obj/atObj_Data.h"
#include "../services/memory/atService_SD.h"
/* _____DEFINITIONS__________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
TaskHandle_t Task_atApp_ParamSync;
void atApp_ParamSync_Task_Func(void *parameter);
///////////////////////////////////////////////Testing part//
/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */

/**
 * This Application class is the application to manage the
 */
class App_ParamSync : public Application
{
public:
	App_ParamSync();
	~App_ParamSync();

protected:
private:
	static void App_ParamSync_Pend();
	static void App_ParamSync_Start();
	static void App_ParamSync_Restart();
	static void App_ParamSync_Execute();
	static void App_ParamSync_Suspend();
	static void App_ParamSync_Resume();
	static void App_ParamSync_End();
} atApp_ParamSync;
/**
 * This function will be automaticaly called when a object is created by this class
 */
App_ParamSync::App_ParamSync(/* args */)
{
	_Pend_User = *App_ParamSync_Pend;
	_Start_User = *App_ParamSync_Start;
	_Restart_User = *App_ParamSync_Restart;
	_Execute_User = *App_ParamSync_Execute;
	_Suspend_User = *App_ParamSync_Suspend;
	_Resume_User = *App_ParamSync_Resume;
	_End_User = *App_ParamSync_End;

	// change the ID of application
	ID_Application = 1;
	// change the application name
	Name_Application = (char *)"ParamSync Application	";
	// change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
App_ParamSync::~App_ParamSync()
{
}
/**
 * Pend to start is the first task of this application it will do prerequisite condition. In the debit mode, task will send information of application to terminal to start the application.
 */
void App_ParamSync::App_ParamSync_Pend()
{
	// atService_SD.Debug();
}
/**
 * This start function will init some critical function
 */
void App_ParamSync::App_ParamSync_Start()
{
	if (atApp_ParamSync.User_Mode == APP_USER_MODE_DEBUG)
	{
		Serial.println("[ParamSync] Initializing Parameter Synchronization Application...");
	}
	atService_SD.Run_Service();
	if (!atService_SD.isInitialized())
	{
		if (!atService_SD.begin(SD_CLK, SD_CMD, SD_D0, SD_D1, SD_D2, SD_D3, false))
		{
			if (atApp_ParamSync.User_Mode == SER_USER_MODE_DEBUG)
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
				if (atApp_ParamSync.User_Mode == SER_USER_MODE_DEBUG)
				{
					Serial.println("✗ 1-bit mode also failed!");
				}
				return;
			}
		}
		if (atApp_ParamSync.User_Mode == APP_USER_MODE_DEBUG)
		{
			Serial.println("✓ SD Card initialized successfully.");
			atService_SD.Debug();
			atService_SD.printCardInfo();
		}
	}
}
/**
 * Restart function of SNM  app
 */
void App_ParamSync::App_ParamSync_Restart()
{
}
/**
 * Execute fuction of SNM app
 */
void App_ParamSync::App_ParamSync_Execute()
{
	if (atObject_Data.sd_to_param_request == true)
	{
		if (atService_SD.loadConfigFromJSON("/CONFIG/Config.json"))
		{
			atObject_Data.sd_to_param_request = false; // Clear the request flag
			if (atApp_ParamSync.User_Mode == APP_USER_MODE_DEBUG)
			{
				Serial.println("[ParamSync] ✅\tSynchronized parameters from SD to RAM.");
			}
		}
		else
		{
			if (atApp_ParamSync.User_Mode == APP_USER_MODE_DEBUG)
			{
				Serial.println("[ParamSync] ❌\tFailed to synchronize parameters from SD to RAM.");
			}
			if (!atService_SD.fileExists("/CONFIG/Config.json"))
			{
				if (atApp_ParamSync.User_Mode == APP_USER_MODE_DEBUG)
				{
					Serial.println("[ParamSync] ❌\tFile JSON không tồn tại: /CONFIG/Config.json");
					Serial.println("[ParamSync] Tạo file cấu hình mặc định trên SD card...");
				}
				if (atService_SD.saveConfigToJSON("/CONFIG/Config.json"))
				{
					if (atApp_ParamSync.User_Mode == APP_USER_MODE_DEBUG)
					{
						Serial.println("[ParamSync] ✅\tTạo file cấu hình mặc định thành công.");
					}
				}
				else
				{
					if (atApp_ParamSync.User_Mode == APP_USER_MODE_DEBUG)
					{
						Serial.println("[ParamSync] ❌\tTạo file cấu hình mặc định thất bại.");
					}
				}
			}
		}
	}
	if (atObject_Data.param_to_sd_request == true)
	{
		if (atService_SD.saveConfigToJSON("/CONFIG/Config.json"))
		{
			atObject_Data.param_to_sd_request = false; // Clear the request flag
			if (atApp_ParamSync.User_Mode == APP_USER_MODE_DEBUG)
			{
				Serial.println("[ParamSync] ✅\tSynchronized parameters from RAM to SD.");
			}
		}
		else
		{
			if (atApp_ParamSync.User_Mode == APP_USER_MODE_DEBUG)
			{
				Serial.println("[ParamSync] ❌\tFailed to synchronize parameters from RAM to SD.");
			}
		}
	}
}
void App_ParamSync::App_ParamSync_Suspend() {}
void App_ParamSync::App_ParamSync_Resume() {}
void App_ParamSync::App_ParamSync_End() {}
void atApp_ParamSync_Task_Func(void *parameter)
{
	while (1)
	{
		atApp_ParamSync.Run_Application(APP_RUN_MODE_AUTO);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
#endif