#include "../src/apps/atApp_CP.h"
#include "../src/apps/atApp_BT.h"
#include "../src/apps/atApp_AI.h"
// #include "../src/apps/atApp_WD.h"
#include "../src/apps/atApp_AP.h"
#include "../src/apps/atApp_DI.h"
#include "../src/apps/atApp_DO.h"
#include "../src/apps/atApp_LED.h"
#include "../src/apps/atApp_LCD.h"
#include "../src/apps/atApp_MB1.h"
#include "../src/apps/atApp_MB2.h"
#include "../src/apps/atApp_Sensor.h"
#include "../src/apps/atApp_RTC_v2.h"
#include "../src/apps/atApp_FTP.h"
#include "../src/apps/atApp_Logger.h"
#include "../src/apps/atApp_ParamSync.h"
// #include "../src/apps/atApp_TestSD.h"
// #include "../src/apps/atApp_TestLCD.h"
// #include "../src/apps/atApp_MQTT_PUB.h"
// #include "../src/apps/atApp_MQTT_SUB.h"
// #include "../src/apps/atApp_RTC.h"
// #include "../src/apps/atApp_TestFTP.h"
// #include "../src/apps/atApp_TestI2CScan.h"
// #include "../src/obj/atObj_Param.h"
// #include "../src/obj/atObj_Data.h"

void setup()
{
  // Reset MCP23008
  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, LOW);
  vTaskDelay(10 / portTICK_PERIOD_MS);
  digitalWrite(RESET_PIN, HIGH);
  vTaskDelay(10 / portTICK_PERIOD_MS);

  Serial.begin(115200);
  vTaskDelay(3000 / portTICK_PERIOD_MS);
  // deinit the task watchdog to avoid reset at the beginning
  // esp_task_wdt_delete(NULL);

  // Uncomment to put the app in debug mode
  // atApp_MQTT_PUB.Debug();
  // atApp_MQTT_SUB.Debug();
  // atApp_FTP.Debug();
  // atApp_AI.Debug();
  // atApp_AP.Debug();
  // atApp_BT.Debug();
  // atApp_CP.Debug();
  // atApp_LCD.Debug();
  // atApp_WD.Debug();
  // atApp_LED.Debug();
  // atApp_MB1.Debug();
  // atApp_MB2.Debug();
  // atApp_RTC.Debug();
  // atApp_RTC_v2.Debug();
  // atApp_Sensor.Debug();
  // atApp_Logger.Debug();
  // atApp_ParamSync.Debug();
  // atApp_DI.Debug();
  // atApp_DO.Debug();
  // atApp_GPS.Debug();
  // atApp_TestSD.Debug();
  // atApp_TestFTP.Debug();
  // atApp_TestLCD.Debug();
  // atApp_TestI2CScan.Debug();

  // Uncomment to put the service in debug mode
  

  // --------------------------------------------Tasks table--------------------------------------------//
  // ---------------------------------------------------------------------------------------------------//
  // || Creation  ||  Task function ||  PC name ||  Heap size ||  Parameter ||  Priority  ||  Task handle ||  CoreID  ||
  
  Serial.println("=== Creating FreeRTOS Tasks ===");
  Serial.printf("[MEMORY] Initial Free Heap: %u bytes\n", esp_get_free_heap_size());
  Serial.printf("[MEMORY] Initial Free PSRAM: %u bytes\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
  Serial.printf("[MEMORY] Largest free block: %u bytes\n", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
  
  // xTaskCreatePinnedToCore(atApp_WD_Task_Func, "atApp_WD_Application", 2048, NULL, 2, &Task_atApp_WD, 1);

  BaseType_t result = xTaskCreatePinnedToCore(atApp_ParamSync_Task_Func, "atApp_ParamSync_Application", 10000, NULL, 1, &Task_atApp_ParamSync, 1);
  Serial.printf("[TASK] ParamSync Application: %s (Handle: %p)\n", result == pdPASS ? "✅ SUCCESS" : "❌ FAILED", Task_atApp_ParamSync);
  // Wait for ParamSync to complete SD->Param loading
  Serial.println("[MAIN] Waiting for ParamSync to complete...");
  while (atObject_Data.sd_to_param_request)
  {
    vTaskDelay(pdMS_TO_TICKS(100)); // Check every 100ms
  }
  Serial.println("[MAIN] ✅ ParamSync completed, continuing initialization...");
  
  result = xTaskCreatePinnedToCore(atApp_CP_Task_Func, "atApp_CP_Application", 2048, NULL, 1, &Task_atApp_CP, 1);
  Serial.printf("[TASK] CP Application: %s (Handle: %p)\n", result == pdPASS ? "✅ SUCCESS" : "❌ FAILED", Task_atApp_CP);
  
  result = xTaskCreatePinnedToCore(atApp_RTC_v2_Task_Func, "atApp_RTC_v2_Application", 4096, NULL, 1, &Task_atApp_RTC_v2, 1);
  Serial.printf("[TASK] RTC_v2 Application: %s (Handle: %p)\n", result == pdPASS ? "✅ SUCCESS" : "❌ FAILED", Task_atApp_RTC_v2);
  
  result = xTaskCreatePinnedToCore(atApp_AI_Task_Func, "atApp_AI_Application", 2048, NULL, 1, &Task_atApp_AI, 1);
  Serial.printf("[TASK] AI Application: %s (Handle: %p)\n", result == pdPASS ? "✅ SUCCESS" : "❌ FAILED", Task_atApp_AI);
  
  result = xTaskCreatePinnedToCore(atApp_LCD_Task_Func, "atApp_LCD_Application", 4096, NULL, 1, &Task_atApp_LCD, 1);
  Serial.printf("[TASK] LCD Application: %s (Handle: %p)\n", result == pdPASS ? "✅ SUCCESS" : "❌ FAILED", Task_atApp_LCD);
  
  result = xTaskCreatePinnedToCore(atApp_MB1_Task_Func, "atApp_MB1_Application", 4096, NULL, 1, &Task_atApp_MB1, 1);
  Serial.printf("[TASK] MB1 Application: %s (Handle: %p)\n", result == pdPASS ? "✅ SUCCESS" : "❌ FAILED", Task_atApp_MB1);
  
  result = xTaskCreatePinnedToCore(atApp_MB2_Task_Func, "atApp_MB2_Application", 4096, NULL, 1, &Task_atApp_MB2, 1);
  Serial.printf("[TASK] MB2 Application: %s (Handle: %p)\n", result == pdPASS ? "✅ SUCCESS" : "❌ FAILED", Task_atApp_MB2);
  
  result = xTaskCreatePinnedToCore(atApp_Sensor_Task_Func, "atApp_Sensor_Application", 5000, NULL, 1, &Task_atApp_Sensor, 1);
  Serial.printf("[TASK] Sensor Application: %s (Handle: %p)\n", result == pdPASS ? "✅ SUCCESS" : "❌ FAILED", Task_atApp_Sensor);
  
  result = xTaskCreatePinnedToCore(atApp_DI_Task_Func, "atApp_DI_Application", 4096, NULL, 1, &Task_atApp_DI, 1);
  Serial.printf("[TASK] DI Application: %s (Handle: %p)\n", result == pdPASS ? "✅ SUCCESS" : "❌ FAILED", Task_atApp_DI);
  
  result = xTaskCreatePinnedToCore(atApp_DO_Task_Func, "atApp_DO_Application", 4096, NULL, 1, &Task_atApp_DO, 1);
  Serial.printf("[TASK] DO Application: %s (Handle: %p)\n", result == pdPASS ? "✅ SUCCESS" : "❌ FAILED", Task_atApp_DO);
  
  result = xTaskCreatePinnedToCore(atApp_LED_Task_Func, "atApp_LED_Application", 2048, NULL, 1, &Task_atApp_LED, 1);
  Serial.printf("[TASK] LED Application: %s (Handle: %p)\n", result == pdPASS ? "✅ SUCCESS" : "❌ FAILED", Task_atApp_LED);
  
  result = xTaskCreatePinnedToCore(atApp_BT_Task_Func, "atApp_BT_Application", 4096, NULL, 1, &Task_atApp_BT, 1);
  Serial.printf("[TASK] BT Application: %s (Handle: %p)\n", result == pdPASS ? "✅ SUCCESS" : "❌ FAILED", Task_atApp_BT);

  result = xTaskCreatePinnedToCore(atApp_Logger_Task_Func, "atApp_Logger_Application", 30000, NULL, 3, &Task_atApp_Logger, 0);
  Serial.printf("[TASK] Logger Application: %s (Handle: %p)\n", result == pdPASS ? "✅ SUCCESS" : "❌ FAILED", Task_atApp_Logger);
  
  result = xTaskCreatePinnedToCore(atApp_FTP_Task_Func, "atApp_FTP_Application", 40960, NULL, 1, &Task_atApp_FTP, 0);
  Serial.printf("[TASK] FTP Application: %s (Handle: %p)\n", result == pdPASS ? "✅ SUCCESS" : "❌ FAILED", Task_atApp_FTP);
  
  // Check memory before creating AP task
  Serial.printf("[MEMORY] Free Heap before AP task: %u bytes\n", esp_get_free_heap_size());
  Serial.printf("[MEMORY] Free PSRAM before AP task: %u bytes\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
  Serial.printf("[MEMORY] Largest free block: %u bytes\n", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
  
  result = xTaskCreatePinnedToCore(atApp_AP_Task_Func, "atApp_AP_Application", 81920, NULL, 4, &Task_atApp_AP, 0);
  Serial.printf("[TASK] AP Application: %s (Handle: %p)\n", result == pdPASS ? "✅ SUCCESS" : "❌ FAILED", Task_atApp_AP);
  
  // Detailed error analysis
  Serial.printf("[DEBUG] AP Application Creation Result: %d\n", result);
  if (result != pdPASS) {
    Serial.println("[ERROR] AP Task Creation Failed!");
    Serial.printf("[ERROR] Required stack: 81920 bytes\n");
    Serial.printf("[ERROR] Available heap: %u bytes\n", esp_get_free_heap_size());
    Serial.printf("[ERROR] Largest block: %u bytes\n", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
    
    // Try creating with smaller stack
    Serial.println("[RETRY] Attempting to create AP task with smaller stack...");
    result = xTaskCreatePinnedToCore(atApp_AP_Task_Func, "atApp_AP_Application", 40960, NULL, 4, &Task_atApp_AP, 0);
    Serial.printf("[RETRY] AP Application (40KB): %s (Handle: %p)\n", result == pdPASS ? "✅ SUCCESS" : "❌ FAILED", Task_atApp_AP);
    
    if (result != pdPASS) {
      Serial.println("[RETRY] Attempting to create AP task with even smaller stack...");
      result = xTaskCreatePinnedToCore(atApp_AP_Task_Func, "atApp_AP_Application", 20480, NULL, 4, &Task_atApp_AP, 0);
      Serial.printf("[RETRY] AP Application (20KB): %s (Handle: %p)\n", result == pdPASS ? "✅ SUCCESS" : "❌ FAILED", Task_atApp_AP);
    }
  }

  // xTaskCreatePinnedToCore(atApp_MQTT_PUB_Task_Func, "atApp_MQTT_PUB_Application", 2048, NULL, 1, &Task_atApp_MQTT_PUB, 1);
  // xTaskCreatePinnedToCore(atApp_MQTT_SUB_Task_Func, "atApp_MQTT_SUB_Application", 2048, NULL, 1, &Task_atApp_MQTT_SUB, 1);
  // xTaskCreatePinnedToCore(atApp_GPS_Task_Func, "atApp_GPS_Application", 2048, NULL, 1, &Task_atApp_GPS, 1);
  // xTaskCreatePinnedToCore(atApp_RTC_Task_Func, "atApp_RTC_Application", 10000, NULL, 1, &Task_atApp_RTC, 1);
  // xTaskCreatePinnedToCore(atApp_TestSD_Task_Func, "atApp_TestSD_Application", 10000, NULL, 1, &Task_atApp_TestSD, 1);
  // xTaskCreatePinnedToCore(atApp_TestFTP_Task_Func, "atApp_TestFTP_Application", 10000, NULL, 1, &Task_atApp_TestFTP, 1);
  // xTaskCreatePinnedToCore(atApp_TestLCD_Task_Func, "atApp_TestLCD_Application", 10000, NULL, 1, &Task_atApp_TestLCD, 1);
  // xTaskCreatePinnedToCore(atApp_TestI2CScan_Task_Func, "atApp_TestI2CScan_Application", 2048, NULL, 1, &Task_atApp_TestI2CScan, 1);

  Serial.println("=== Task Creation Summary ===");
  Serial.printf("Total Active Tasks: %d\n", uxTaskGetNumberOfTasks());
  Serial.printf("Free Heap after task creation: %u bytes\n", esp_get_free_heap_size());
  Serial.printf("Free PSRAM after task creation: %u bytes\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
  Serial.printf("Minimum Free Heap: %u bytes\n", esp_get_minimum_free_heap_size());
  Serial.printf("Largest free block: %u bytes\n", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
  
  // Memory usage analysis
  size_t total_stack_size = 2048 + 10000 + 2048 + 10000 + 10000 + 10000 + 5000 + 10000 + 10000 + 2048 + 10000 + 30000 + 100000 + 81920;
  Serial.printf("Total allocated stack size: %u bytes (%.1f KB)\n", total_stack_size, total_stack_size / 1024.0);
  
  if (Task_atApp_AP == NULL) {
    Serial.println("⚠️  RECOMMENDATION: Consider reducing AP task stack size or other tasks");
    Serial.println("    - Try AP stack: 40960 (40KB) instead of 81920 (80KB)");
    Serial.println("    - Try FTP stack: 50000 (50KB) instead of 100000 (100KB)");
    Serial.println("    - Or disable some non-critical tasks temporarily");
  }

  //------------------------------------------------------------------------------------------------------//
  // Some tasks will be suspended after initiating
  // vTaskSuspend(Task_atApp_AP);
  // vTaskSuspend(Task_atApp_ABC);
  
  // Suspend AP task by default - it will be resumed when button is held for 5 seconds
  if (Task_atApp_AP != NULL)
  {
    vTaskSuspend(Task_atApp_AP);
    Serial.println("[SUSPEND] ⏸️ AP task suspended by default");
    Serial.printf("[INFO] Active Tasks after suspension: %d\n", uxTaskGetNumberOfTasks());
  }
  else
  {
    Serial.println("[ERROR] ❌ AP task handle is NULL, cannot suspend");
  }
  
  Serial.println("=== System Initialization Complete ===");
  Serial.printf("Final Free Heap: %u bytes\n", esp_get_free_heap_size());
  Serial.printf("Final Free PSRAM: %u bytes\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
  Serial.println("=== Starting Main Application ===");
}
void loop()
{
  vTaskDelete(NULL); // Delete the main loop
}
