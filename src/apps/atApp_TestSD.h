#ifndef _Application_atApp_TestSD_
#define _Application_atApp_TestSD_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "App.h"
#include "../services/memory/atService_SD.h"
// #include "../src/obj/atObj_Param.h"
/* _____DEFINITIONS__________________________________________________________ */
// Configuration storage structure
struct ESPConfig
{
    String deviceName;
    String wifiSSID;
    String wifiPassword;
    int mqttPort;
    String mqttServer;
    bool enableOTA;
    float calibrationValue;
};
/* _____GLOBAL VARIABLES_____________________________________________________ */
TaskHandle_t Task_atApp_TestSD;
void atApp_TestSD_Task_Func(void *parameter);
///////////////////////////////////////////////Testing part//
/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */
// // Create SD Card management object
// Service_SD atService_SD(SD_CS);
/**
 * This Application class is the application to manage the
 */
class App_TestSD : public Application
{
public:
    App_TestSD();
    ~App_TestSD();
    void createSampleConfig();
    bool loadConfig();
    void testCopyAndMove();
    void testBasicFunctions();
    void testDirectoryListing();

    ESPConfig config; // Configuration storage variable
protected:
private:
    static void App_TestSD_Pend();
    static void App_TestSD_Start();
    static void App_TestSD_Restart();
    static void App_TestSD_Execute();
    static void App_TestSD_Suspend();
    static void App_TestSD_Resume();
    static void App_TestSD_End();
} atApp_TestSD;
/**
 * This function will be automaticaly called when a object is created by this class
 */
// Function to create sample JSON file with configuration
void App_TestSD::createSampleConfig()
{
    Serial.println("\n=== CREATE SAMPLE CONFIGURATION FILE ===");

    // Create JSON object
    JsonDocument doc;

    doc["deviceName"] = "ESP32-S3-Device-001";
    doc["wifiSSID"] = "MyWiFiNetwork";
    doc["wifiPassword"] = "MySecurePassword123";
    doc["mqttPort"] = 1883;
    doc["mqttServer"] = "mqtt.example.com";
    doc["enableOTA"] = true;
    doc["calibrationValue"] = 3.14159;

    // Add sensor array
    JsonArray sensors = doc["sensors"].to<JsonArray>();
    sensors.add("DHT22");
    sensors.add("BMP280");
    sensors.add("LDR");

    // Convert to String
    String jsonString;
    serializeJsonPretty(doc, jsonString);

    Serial.println("JSON content:");
    Serial.println(jsonString);

    // Write to SD card
    if (atService_SD.writeFile("/config.json", jsonString))
    {
        Serial.println("✓ Saved config.json successfully");
    }
    else
    {
        Serial.println("✗ Error saving config.json");
    }
}

// Function to read and parse JSON config file
bool App_TestSD::loadConfig()
{
    Serial.println("\n=== READ CONFIGURATION FILE ===");

    if (!atService_SD.fileExists("/config.json"))
    {
        Serial.println("✗ File config.json does not exist!");
        return false;
    }

    String jsonString = atService_SD.readFile("/config.json");
    if (jsonString.length() == 0)
    {
        Serial.println("✗ Cannot read file config.json");
        return false;
    }

    // Parse JSON
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonString);

    if (error)
    {
        Serial.printf("✗ JSON parse error: %s\n", error.c_str());
        return false;
    }

    // Get data from JSON
    config.deviceName = doc["deviceName"].as<String>();
    config.wifiSSID = doc["wifiSSID"].as<String>();
    config.wifiPassword = doc["wifiPassword"].as<String>();
    config.mqttPort = doc["mqttPort"];
    config.mqttServer = doc["mqttServer"].as<String>();
    config.enableOTA = doc["enableOTA"];
    config.calibrationValue = doc["calibrationValue"];

    Serial.println("✓ Successfully loaded config!");
    Serial.println("\n--- Loaded Configuration ---");
    Serial.printf("Device Name: %s\n", config.deviceName.c_str());
    Serial.printf("WiFi SSID: %s\n", config.wifiSSID.c_str());
    Serial.printf("WiFi Password: %s\n", config.wifiPassword.c_str());
    Serial.printf("MQTT Server: %s:%d\n", config.mqttServer.c_str(), config.mqttPort);
    Serial.printf("Enable OTA: %s\n", config.enableOTA ? "Yes" : "No");
    Serial.printf("Calibration: %.5f\n", config.calibrationValue);

    // In mảng sensors
    JsonArray sensors = doc["sensors"];
    Serial.print("Sensors: ");
    for (JsonVariant sensor : sensors)
    {
        Serial.printf("%s ", sensor.as<const char *>());
    }
    Serial.println();

    return true;
}

// Hàm test các chức năng cơ bản
void App_TestSD::testBasicFunctions()
{
    Serial.println("\n=== TEST CÁC CHỨC NĂNG CƠ BẢN ===");

    // Test tạo thư mục
    atService_SD.createDir("/logs/data/data/data1");
    atService_SD.createDir("/data");

    // Test ghi file text
    atService_SD.writeFile("/test.txt", "Hello ESP32-S3!\n");
    atService_SD.appendFile("/test.txt", "This is a test file.\n");
    atService_SD.appendFile("/test.txt", "Line 3 here!\n");

    // Test đọc file
    String content = atService_SD.readFile("/test.txt");
    Serial.println("\nNội dung file test.txt:");
    Serial.println(content);

    // Test ghi log
    String logEntry = String(millis()) + " - System started\n";
    atService_SD.appendFile("/logs/system.log", logEntry);

    // Test ghi dữ liệu nhị phân
    uint8_t binaryData[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    atService_SD.writeBinaryFile("/data/sensor.bin", binaryData, sizeof(binaryData));

    // Test đọc dữ liệu nhị phân
    uint8_t readBuffer[10];
    size_t readSize;
    if (atService_SD.readBinaryFile("/data/sensor.bin", readBuffer, &readSize))
    {
        Serial.print("Binary data: ");
        for (size_t i = 0; i < readSize; i++)
        {
            Serial.printf("0x%02X ", readBuffer[i]);
        }
        Serial.println();
    }
}

// Test hàm copy và move file
void App_TestSD::testCopyAndMove()
{
    Serial.println("\n=== TEST COPY VÀ MOVE FILE ===");

    // Tạo file test
    atService_SD.writeFile("/data/original.txt", "This is the original file content.\nLine 2\nLine 3\n");

    // Test copy file
    Serial.println("\n1. Test Copy File:");
    if (atService_SD.copyFile("/data/original.txt", "/backup/copied.txt"))
    {
        Serial.println("  ✓ Copy thành công!");

        // Kiểm tra nội dung file đã copy
        String copiedContent = atService_SD.readFile("/backup/copied.txt");
        Serial.println("  Nội dung file đã copy:");
        Serial.println(copiedContent);
    }
    else
    {
        Serial.println("  ✗ Copy thất bại!");
    }

    // Test copy file với tên khác
    Serial.println("\n2. Test Copy với tên khác:");
    if (atService_SD.copyFile("/data/original.txt", "/data/duplicate.txt"))
    {
        Serial.println("  ✓ Copy với tên khác thành công!");
    }

    // Test move file
    Serial.println("\n3. Test Move File:");
    atService_SD.writeFile("/data/temp.txt", "Temporary file to be moved\n");

    if (atService_SD.moveFile("/data/temp.txt", "/logs/moved.txt"))
    {
        Serial.println("  ✓ Move thành công!");
        Serial.printf("  File cũ tồn tại: %s\n", atService_SD.fileExists("/data/temp.txt") ? "Yes" : "No");
        Serial.printf("  File mới tồn tại: %s\n", atService_SD.fileExists("/logs/moved.txt") ? "Yes" : "No");
    }
    else
    {
        Serial.println("  ✗ Move thất bại!");
    }

    // Test copy file lớn (mô phỏng)
    Serial.println("\n4. Test Copy file lớn:");
    String largeContent = "";
    for (int i = 0; i < 100; i++)
    {
        largeContent += "Line " + String(i) + ": This is a test line with some data.\n";
    }
    atService_SD.writeFile("/data/large.txt", largeContent);

    unsigned long startTime = millis();
    if (atService_SD.copyFile("/data/large.txt", "/backup/large_copy.txt"))
    {
        unsigned long copyTime = millis() - startTime;
        Serial.printf("  ✓ Copy file lớn thành công! (%lu ms)\n", copyTime);
        Serial.printf("  Kích thước file: %d bytes\n", atService_SD.getFileSize("/backup/large_copy.txt"));
    }

    // Test error case - copy file không tồn tại
    Serial.println("\n5. Test Error Cases:");
    if (!atService_SD.copyFile("/nonexistent.txt", "/backup/error.txt"))
    {
        Serial.println("  ✓ Đã bắt lỗi file không tồn tại");
    }

    // Test move file đã tồn tại ở đích
    atService_SD.writeFile("/data/test1.txt", "Test 1\n");
    atService_SD.writeFile("/data/test2.txt", "Test 2\n");
    if (!atService_SD.moveFile("/data/test1.txt", "/data/test2.txt"))
    {
        Serial.println("  ✓ Đã bắt lỗi file đích đã tồn tại");
    }
}

// Hàm test liệt kê thư mục
void App_TestSD::testDirectoryListing()
{
    Serial.println("\n=== DANH SÁCH THƯ MỤC ===");
    atService_SD.listDir("/", 2);
}

App_TestSD::App_TestSD(/* args */)
{
    _Pend_User = *App_TestSD_Pend;
    _Start_User = *App_TestSD_Start;
    _Restart_User = *App_TestSD_Restart;
    _Execute_User = *App_TestSD_Execute;
    _Suspend_User = *App_TestSD_Suspend;
    _Resume_User = *App_TestSD_Resume;
    _End_User = *App_TestSD_End;

    // change the ID of application
    ID_Application = 1;
    // change the application name
    Name_Application = (char *)"TestSD Application	";
    // change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
App_TestSD::~App_TestSD()
{
}
/**
 * Pend to start is the first task of this application it will do prerequisite condition. In the debit mode, task will send information of application to terminal to start the application.
 */
void App_TestSD::App_TestSD_Pend()
{
    // atService_XYZ.Debug();
}
/**
 * This start function will init some critical function
 */
void App_TestSD::App_TestSD_Start()
{
    if (atApp_TestSD.User_Mode == SER_USER_MODE_DEBUG)
    {
        atService_SD.Debug();
    }
    atService_SD.Run_Service();
    if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("\n\n");
        Serial.println("╔════════════════════════════════════════╗");
        Serial.println("║   ESP32-S3 SD CARD MANAGER TEST        ║");
        Serial.println("╚════════════════════════════════════════╝");
    }

    // Khởi tạo SD Card với các chân SDIO tùy chỉnh
    // Chế độ 4-bit (nhanh): truyền đủ 6 tham số CLK, CMD, D0, D1, D2, D3
    // Chế độ 1-bit (chậm): chỉ truyền CLK, CMD, D0 (các chân còn lại để -1 hoặc 255)
    if (!atService_SD.begin(SD_CLK, SD_CMD, SD_D0, SD_D1, SD_D2, SD_D3, false))
    {
        if (atApp_TestSD.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("\n❌ KHÔNG THỂ KHỞI TẠO THẺ SD!");
            Serial.println("Vui lòng kiểm tra:");
            Serial.println("  - Thẻ SD đã được lắp đúng cách");
            Serial.println("  - Kết nối đúng chân SDIO:");
            Serial.printf("    CLK: GPIO%d\n", SD_CLK);
            Serial.printf("    CMD: GPIO%d\n", SD_CMD);
            Serial.printf("    D0:  GPIO%d\n", SD_D0);
            Serial.printf("    D1:  GPIO%d\n", SD_D1);
            Serial.printf("    D2:  GPIO%d\n", SD_D2);
            Serial.printf("    D3:  GPIO%d\n", SD_D3);
            Serial.println("  - Thẻ SD hoạt động tốt");
            Serial.println("  - Nguồn 3.3V ổn định");

            // Thử chế độ 1-bit nếu chế độ 4-bit thất bại
            Serial.println("\n→ Thử lại với chế độ 1-bit...");
        }
        if (!atService_SD.begin(SD_CLK, SD_CMD, SD_D0, 255, 255, 255, false))
        {
            if (atApp_TestSD.User_Mode == SER_USER_MODE_DEBUG)
            {
                Serial.println("❌ Chế độ 1-bit cũng thất bại!");
            }
            return;
        }
    }
    // In thông tin thẻ SD
    atService_SD.printCardInfo();

    // Chạy các test
    // atApp_TestSD.testBasicFunctions();
    // atApp_TestSD.createSampleConfig();
    // atApp_TestSD.loadConfig();
    // atApp_TestSD.testCopyAndMove();
    // atApp_TestSD.testDirectoryListing();
    // String listFileName = atService_SD.listFiles("/test", 2);
    // Serial.println("\n=== DANH SÁCH FILE TRONG /test ===");
    // Serial.println(listFileName);

    // atService_SD.moveFile("/data/duplicate.txt", "/logs/testhn.txt");
    // atService_SD.copyFile("/config.json", "/data/configcpy.json");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    Serial.println("\n=== QUÁ TRÌNH LƯU CẤU HÌNH VỀ JSON ===");
    //Tạo file JSON config lần đầu
    if (atService_SD.saveConfigToJSON("/CONFIG/Config.json"))
    {
        Serial.println("✅\tĐã lưu cấu hình ban đầu vào /CONFIG/Config.json");
    }
    else
    {
        Serial.println("❌\tLỗi khi lưu cấu hình ban đầu!");
    }

    // if (atService_SD.loadConfigFromJSON("/CONFIG/Config.json"))
    // {
    //     Serial.println("✅\tĐã tải cấu hình từ /CONFIG/Config.json");
    // }
    // else
    // {
    //     Serial.println("❌\tLỗi khi tải cấu hình từ /CONFIG/Config.json!");
    // }
    // Serial.print("Giá trị DO Schmitt Trigger Upper Threshold: ");
    // for (int i = 0; i < MAX_DO_CHANNEL; i++)
    // {
    //     Serial.print(atObject_Param.do_schmitt_trigger_upper_threshold_list[i]);
    //     Serial.print(" ");
    // }
    Serial.println("╔═══════════════════════╗");
    Serial.println("║   TEST HOÀN THÀNH!    ║");
    Serial.println("╚═══════════════════════╝");
}
/**
 * Restart function of SNM  app
 */
void App_TestSD::App_TestSD_Restart()
{
}
/**
 * Execute fuction of SNM app
 */
void App_TestSD::App_TestSD_Execute()
{
    if (!atService_SD.fileExists("/logs/system.log"))
    {
        if (atService_SD.createDir("/logs") && atService_SD.createFile("/logs/system.log"))
        {
            if (atApp_TestSD.User_Mode == SER_USER_MODE_DEBUG)
            {
                Serial.println("✅\t Đã tạo file log hệ thống mới");
            }
            if (atService_SD.writeFile("/logs/system.log", "System Log Initialized\n"))
            {
                if (atApp_TestSD.User_Mode == SER_USER_MODE_DEBUG)
                {
                    Serial.println("✅\t Đã ghi dòng khởi tạo vào file log hệ thống");
                }
            }
            else
            {
                if (atApp_TestSD.User_Mode == SER_USER_MODE_DEBUG)
                {
                    Serial.println("❌\t Lỗi khi ghi dòng khởi tạo vào file log hệ thống!");
                }
            }
        }
        else
        {
            if (atApp_TestSD.User_Mode == SER_USER_MODE_DEBUG)
            {
                Serial.println("❌\t Lỗi khi tạo file log hệ thống!");
            }
            return;
        }
    }
    // Ví dụ: ghi log định kỳ
    static unsigned long lastLog = 0;
    if (millis() - lastLog > 5000)
    { // Mỗi 5 giây
        lastLog = millis();

        String logEntry = String(millis()) + " - Heartbeat\n";
        atService_SD.appendFile("/logs/system.log", logEntry);

        Serial.println("✅\t Đã ghi log heartbeat");
    }
}
void App_TestSD::App_TestSD_Suspend() {}
void App_TestSD::App_TestSD_Resume() {}
void App_TestSD::App_TestSD_End() {}
void atApp_TestSD_Task_Func(void *parameter)
{
    while (1)
    {
        atApp_TestSD.Run_Application(APP_RUN_MODE_AUTO);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
#endif