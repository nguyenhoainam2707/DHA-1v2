#ifndef _Service_atService_EG800K_
#define _Service_atService_EG800K_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "Service.h"
#include "../src/obj/atObj_Param.h"
#include "../src/obj/atObj_Data.h"
/* _____DEFINATIONS__________________________________________________________ */
#define UART_NUM 0
#define EG800K_RX 44 // UART0
#define EG800K_TX 43 // UART0
#define SIM_PWRKEY_PIN 2
#define SIM_RESET_PIN 1
#define SIM_VBAT_PIN 42
#define EG800K_BAUD 115200
#define AT_TIMEOUT_MS 5000

#define MQTT_BROKER "broker.emqx.io"
#define MQTT_PORT "1883"
#define MQTT_CLIENT_ID "ESP32S3_001"

HardwareSerial eg800k(UART_NUM);
SemaphoreHandle_t uart0_mutex;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */
/**
 * This Service class is the Service to manage the
 */
class Service_EG800K : public Service
{
public:
    Service_EG800K();
    ~Service_EG800K();

    static bool sendATCommand(const char *command, const char *expectedResponse, int timeout = AT_TIMEOUT_MS);
    static bool initSIM();
    static bool initEG800K();

    // MQTT functions
    static bool setupMQTT();
    static bool publishMQTT(const char *topic, const char *payload);

    // static void subscribeMQTTTopic(const String &subTopic);
    // static void handleMQTTMessage();

    // // FTP functions
    static bool openFTP(
        String ftp_host = "ftphost",
        String ftp_user = "ftpuser",
        String ftp_pass = "ftppassword",
        uint16_t ftp_port = 21);
    static bool closeFTP();
    static bool mkdirFTP(const char *dirname);
    static bool printWorkingDirectoryFTP();
    static bool listFilesFTP(const char *path);
    static bool changeDirectoryFTP(const char *path);
    static bool uploadFTP(const char *filename, const char *data,size_t dataLen);
    static bool downloadFTP(const char *filename);
    static bool deleteFileFTP(const char *filename);
    static bool renameFTP(const char *oldName, const char *newName);
    static bool removeDirectoryFTP(const char *dirname);

    // // GPS functions
    // static void configGPS();
    // static void getPosition();

protected:
private:
    static void Service_EG800K_Start();
    static void Service_EG800K_Execute();
    static void Service_EG800K_End();
} atService_EG800K;

/**
 * This function will be automaticaly called when a object is created by this class
 */
Service_EG800K::Service_EG800K(/* args */)
{
    _Start_User = *Service_EG800K_Start;
    _Execute_User = *Service_EG800K_Execute;
    _End_User = *Service_EG800K_End;

    // change the ID of Service
    ID_Service = 2;
    // change the Service name
    Name_Service = (char *)"EG800K Service";
    // change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
Service_EG800K::~Service_EG800K()
{
}
/**
 * This function will send AT command to EG800K
 */
// Hàm gửi lệnh AT và đợi phản hồi
bool Service_EG800K::sendATCommand(const char *command, const char *expectedResponse, int timeout)
{
    xSemaphoreTake(uart0_mutex, portMAX_DELAY);

    eg800k.println(command);
    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.print(">> ");
        Serial.println(command);
    }

    String response = "";
    unsigned long startTime = millis();

    while (millis() - startTime < timeout)
    {
        if (eg800k.available())
        {
            response += eg800k.readString();
            if (response.indexOf(expectedResponse) != -1)
            {
                if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
                {
                    Serial.print("<< ");
                    Serial.println(response);
                }
                xSemaphoreGive(uart0_mutex);
                return true;
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.print("<< TIMEOUT: ");
        Serial.println(response);
    }
    xSemaphoreGive(uart0_mutex);
    return false;
}

bool Service_EG800K::initSIM()
{
    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("Initializing SIM module...");
    }

    // Kiểm tra kết nối
    if (!sendATCommand("AT", "OK", 2000))
    {
        if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("SIM module not responding");
        }
        return false;
    }

    // Tắt echo
    sendATCommand("ATE0", "OK");

    // Kiểm tra SIM card
    if (!sendATCommand("AT+CPIN?", "READY"))
    {
        Serial.println("SIM card not ready");
        return false;
    }

    // Đợi đăng ký mạng
    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("Waiting for network registration...");
    }
    for (int i = 0; i < 30; i++)
    {
        if (sendATCommand("AT+CREG?", "+CREG: 0,1") || sendATCommand("AT+CREG?", "+CREG: 0,5"))
        {
            if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
            {
                Serial.println("Network registered successfully");
            }
            break;
        }
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        if (i == 29)
        {
            Serial.println("Network registration failed");
            return false;
        }
    }

    // Kiểm tra tín hiệu
    sendATCommand("AT+CSQ", "OK");

    return true;
}

bool Service_EG800K::setupMQTT()
{
    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("Setting up MQTT...");
    }

    // Tạo MQTT client
    String cmd = "AT+QMTOPEN=0,\"" + atObject_Param.mqtt_broker + "\"," + String(atObject_Param.mqtt_port);
    if (!sendATCommand(cmd.c_str(), "+QMTOPEN: 0,0", 10000))
    {
        if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("Failed to connect to MQTT broker");
        }
        return false;
    }

    // Kết nối MQTT
    cmd = "AT+QMTCONN=0,\"" + atObject_Param.mqtt_client_id + "\"";
    if (!sendATCommand(cmd.c_str(), "+QMTCONN: 0,0,0", 10000))
    {
        Serial.println("Failed to connect MQTT client");
        return false;
    }

    // Subscribe topic
    cmd = "AT+QMTSUB=0,1,\"" + atObject_Param.mqtt_sub_topic + "\",0";
    if (!sendATCommand(cmd.c_str(), "+QMTSUB: 0,1,0", 5000))
    {
        Serial.println("Failed to subscribe topic");
        return false;
    }

    Serial.println("MQTT setup completed successfully");
    atObject_Data.mqttConnected = true;
    return true;
}
/**
 * This function will setup the EG800K
 */
bool Service_EG800K::initEG800K()
{
    pinMode(SIM_PWRKEY_PIN, OUTPUT);
    pinMode(SIM_RESET_PIN, OUTPUT);
    pinMode(SIM_VBAT_PIN, OUTPUT);
    digitalWrite(SIM_PWRKEY_PIN, HIGH);
    digitalWrite(SIM_RESET_PIN, LOW);
    digitalWrite(SIM_VBAT_PIN, LOW);

    vTaskDelay(2000 / portTICK_PERIOD_MS);

    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("ESP32S3 MQTT Client Starting...");
    }

    // Khởi tạo UART cho SIM module
    eg800k.begin(EG800K_BAUD, SERIAL_8N1, EG800K_RX, EG800K_TX);

    // Tạo mutex và queue
    uart0_mutex = xSemaphoreCreateMutex();
    // commandQueue = xQueueCreate(10, sizeof(String));

    // Khởi tạo SIM module
    if (initSIM())
    {
        atObject_Data.simReady = true;
        Serial.println("SIM module initialized successfully");
        return true;
    }
    return false;
}

/**
 * This function will send data to MQTT broker
 */
// Publish dữ liệu MQTT
bool Service_EG800K::publishMQTT(const char *topic, const char *payload)
{
    if (!atObject_Data.mqttConnected)
        return false;

    String cmd = "AT+QMTPUBEX=0,0,0,0,\"" + String(topic) + "\"," + String(strlen(payload));

    xSemaphoreTake(uart0_mutex, portMAX_DELAY);

    eg800k.println(cmd);
    vTaskDelay(100 / portTICK_PERIOD_MS); // Đợi một chút để module xử lý lệnh

    // Đợi prompt '>'
    String response = "";
    unsigned long startTime = millis();
    while (millis() - startTime < 2000)
    {
        if (eg800k.available())
        {
            char c = eg800k.read();
            if (c == '>')
            {
                break;
            }
        }
    }

    // Gửi payload
    eg800k.print(payload);

    // Đợi phản hồi
    response = "";
    startTime = millis();
    bool success = false;
    while (millis() - startTime < 5000)
    {
        if (eg800k.available())
        {
            response += eg800k.readString();
            if (response.indexOf("+QMTPUBEX: 0,0,0") != -1)
            {
                success = true;
                break;
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    xSemaphoreGive(uart0_mutex);
    return success;
}

// void Service_EG800K::subscribeMQTTTopic(const String &subTopic)
// {
//     // Subscribe to topic
//     String cmd = "AT+QMTSUB=0,1,\"" + subTopic + "\",2"; // 0: client_idx, 1: msg_id, QoS=2
//     sendATCommand(cmd, 1500);
// }
// // This function should be called regularly in the loop/task to check for incoming data
// void Service_EG800K::handleMQTTMessage()
// {
//     static bool receiving = false;
//     static String recvLine = "";

//     while (eg800k.available())
//     {
//         char c = eg800k.read();
//         if (!receiving)
//         {
//             // Tìm bắt đầu chuỗi +QMTRECV:
//             if (c == '+')
//             {
//                 recvLine = c;
//                 receiving = true;
//             }
//         }
//         else
//         {
//             recvLine += c;
//             if (c == '}')
//             {
//                 if (recvLine.startsWith("+QMTRECV:"))
//                 {
//                     uint16_t openingBrace = recvLine.indexOf('{');
//                     uint8_t firstQuote = recvLine.indexOf('"');
//                     uint8_t secondQuote = recvLine.indexOf('"', firstQuote + 1);
//                     String topic = recvLine.substring(firstQuote + 1, secondQuote);
//                     String payload = recvLine.substring(openingBrace);
//                     if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
//                     {
//                         Serial.print("[MQTT] Topic: ");
//                         Serial.println(topic);
//                         Serial.println("[MQTT] Payload: ");
//                         Serial.println(payload);
//                     }
//                     // Reset để nhận bản tin tiếp theo
//                     receiving = false;
//                     recvLine = "";
//                 }
//             }
//         }
//     }
// }

/**
 * This function will connect to FTP server
 * @param ftp_host The FTP server host
 * @param ftp_user The FTP server user
 * @param ftp_pass The FTP server password
 * @param ftp_port The FTP server port
 * @param ftp_path The FTP server path
 */
bool Service_EG800K::openFTP(
    String ftp_host,
    String ftp_user,
    String ftp_pass,
    uint16_t ftp_port)
{
    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("\n========== FTP OPEN ==========");
    }

    // Đóng kết nối cũ nếu có
    if (atObject_Data.ftpConnected)
    {
        sendATCommand("AT+QFTPCLOSE", "OK", 2000);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    // Cấu hình account
    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("[1] Setting FTP account...");
    }
    String cmd = "AT+QFTPCFG=\"account\",\"" + ftp_user + "\",\"" + ftp_pass + "\"";
    if (!sendATCommand(cmd.c_str(), "OK", 3000))
    {
        if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("❌ Failed to set FTP account");
        }
        return false;
    }
    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("✅ Account set");
    }

    // Set file type (1 = Binary, 0 = ASCII)
    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("[2] Setting file type to binary...");
    }
    if (!sendATCommand("AT+QFTPCFG=\"filetype\",1", "OK", 2000))
    {
        if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("❌ Failed to set file type");
        }
        return false;
    }
    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("✅ File type set");
    }

    // Set transfer mode (1 = Passive, 0 = Active)
    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("[3] Setting transfer mode to passive...");
    }
    if (!sendATCommand("AT+QFTPCFG=\"transmode\",1", "OK", 2000))
    {
        if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("❌ Failed to set transfer mode");
        }
        return false;
    }
    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("✅ Transfer mode set");
    }

    // Mở kết nối
    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("[4] Opening FTP connection...");
    }
    cmd = "AT+QFTPOPEN=\"" + ftp_host + "\"," + ftp_port;
    if (!sendATCommand(cmd.c_str(), "+QFTPOPEN: 0,0", 15000))
    {
        if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("❌ Failed to open FTP connection");
        }
        return false;
    }

    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("✅ FTP connection opened successfully\n");
    }
    atObject_Data.ftpConnected = true;
    return true;
}
bool Service_EG800K::closeFTP()
{
    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("\n========== FTP CLOSE ==========");
    }
    if (!sendATCommand("AT+QFTPCLOSE", "OK", 5000))
    {
        if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("❌ Failed to close FTP connection");
        }
        return false;
    }

    atObject_Data.ftpConnected = false;
    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("✅ FTP connection closed\n");
    }
    return true;
}
/**
 * This function will check if the folder path exists, if not it will create the folder path
 * @param path The folder path to check/create
 */
bool Service_EG800K::mkdirFTP(const char *dirname)
{
    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("\n========== FTP MKDIR (Create Directory) ==========");
        Serial.println("Directory: " + String(dirname));
    }

    if (!atObject_Data.ftpConnected)
    {
        if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("❌ FTP not connected");
        }
        return false;
    }

    String cmd = "AT+QFTPMKDIR=\"" + String(dirname) + "\"";
    if (!sendATCommand(cmd.c_str(), "+QFTPMKDIR: 0,0", 10000))
    {
        if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("❌ Failed to create directory");
        }
        return false;
    }

    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("✅ Directory created successfully\n");
    }
    return true;
}

bool Service_EG800K::printWorkingDirectoryFTP()
{
    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("\n========== FTP PRINT WORKING DIRECTORY ==========");
    }

    if (!atObject_Data.ftpConnected)
    {
        if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("❌ FTP not connected");
        }
        return false;
    }

    if (!sendATCommand("AT+QFTPPWD", "+QFTPPWD:", 10000))
    {
        if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("❌ Failed to get current directory");
        }
        return false;
    }

    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("✅ PWD command successful\n");
    }
    return true;
}

bool Service_EG800K::listFilesFTP(const char *path)
{
    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("\n========== FTP LIST (List Files) ==========");
    }

    if (!atObject_Data.ftpConnected)
    {
        if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("❌ FTP not connected");
        }
        return false;
    }

    String cmd = "AT+QFTPLIST=\"" + String(path) + "\"";

    eg800k.println(cmd);
    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println(">> " + cmd);
    }

    String response = "";
    unsigned long startTime = millis();

    while (millis() - startTime < 15000)
    {
        if (eg800k.available())
        {
            response += eg800k.readString();
            if (response.indexOf("+QFTPLIST:") != -1)
            {
                if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
                {
                    Serial.println("<< " + response);
                    Serial.println("✅ List command successful\n");
                }
                return true;
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("❌ Failed to list directory");
        Serial.println("<< " + response);
    }
    return false;
}

bool Service_EG800K::changeDirectoryFTP(const char *path)
{
    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("\n========== FTP CWD (Change Working Directory) ==========");
        Serial.println("Changing directory to: " + String(path));
    }

    if (!atObject_Data.ftpConnected)
    {
        if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("❌ FTP not connected");
        }
        return false;
    }

    String cmd = "AT+QFTPCWD=\"" + String(path) + "\"";
    if (!sendATCommand(cmd.c_str(), "+QFTPCWD: 0,0", 10000))
    {
        if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("❌ Failed to change directory");
        }
        return false;
    }

    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("✅ Directory changed successfully\n");
    }
    return true;
}

bool Service_EG800K::uploadFTP(const char *filename, const char *data, size_t dataLen)
{
    // Add validation
    if (!data || dataLen == 0)
    {
        if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("❌ Data is NULL or empty");
        }
        return false;
    }
    
    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("\n========== FTP Upload ==========");
        Serial.println("Filename: " + String(filename));
        Serial.println("Data length: " + String(dataLen) + " bytes");
        Serial.println("Data preview: " + String(data).substring(0, 100) + "..."); // Show first 100 chars
    }

    if (!atObject_Data.ftpConnected)
    {
        if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("❌ FTP not connected");
        }
        return false;
    }

    // send upload command
    String cmd = "AT+QFTPPUT=\"" + String(filename) + "\",\"COM:\",0," + String(dataLen) + ",1";
    eg800k.println(cmd);
    // println(cmd);
    // Serial.println(cmd);
    // if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    // {
    //     Serial.println(">> " + cmd);
    // }

    // wait "CONNECT" response
    String response = "";
    unsigned long startTime = millis();
    bool gotConnect = false;

    while (millis() - startTime < 10000)
    {
        if (eg800k.available())
        {
            response += eg800k.readString();
            if (response.indexOf("CONNECT") != -1)
            {
                gotConnect = true;
                if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
                {
                    Serial.println("<< Got CONNECT prompt");
                }
                break;
            }
            if (response.indexOf("ERROR") != -1)
            {
                if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
                {
                    Serial.println("❌ FTP PUT failed: " + response);
                }
                return false;
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    if (!gotConnect)
    {
        if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("❌ Failed to get CONNECT prompt");
            Serial.println("<< " + response);
        }
        return false;
    }

    // send data
    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("Sending data...");
    }
    eg800k.write((const uint8_t *)data, dataLen);

    // wait for upload response 
    response = "";
    startTime = millis();
    bool success = false;

    while (millis() - startTime < 20000)
    {
        if (eg800k.available())
        {
            response += eg800k.readString();
            if (response.indexOf("+QFTPPUT:") != -1)
            {
                success = true;
                if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
                {
                    Serial.println("<< " + response);
                }
                // print the file have been uploaded

                Serial.printf("✅ File %s uploaded successfully to  \n", filename);
                break;
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    if (!success)
    {
        if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("❌ Upload failed or timeout");
            Serial.println("<< " + response);
        }
    }

    return success;
}

bool Service_EG800K::downloadFTP(const char *filename)
{
    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("\n========== FTP Download ==========");
        Serial.println("Filename: " + String(filename));
    }

    if (!atObject_Data.ftpConnected)
    {
        if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("❌ FTP not connected");
        }
        return false;
    }

    String cmd = "AT+QFTPGET=\"" + String(filename) + "\",\"COM:\"";
    eg800k.println(cmd);
    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println(">> " + cmd);
    }

    String response = "";
    unsigned long startTime = millis();

    while (millis() - startTime < 30000)
    {
        if (eg800k.available())
        {
            response += eg800k.readString();
            if (response.indexOf("+QFTPGET:") != -1)
            {
                if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
                {
                    Serial.println("<< " + response);
                    Serial.println("✅ File downloaded (check response for data)\n");
                }
                return true;
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("❌ Download failed or timeout");
        Serial.println("<< " + response);
    }
    return false;
}

bool Service_EG800K::deleteFileFTP(const char *filename)
{
    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("\n========== FTP DELETE ==========");
        Serial.println("Filename: " + String(filename));
    }

    if (!atObject_Data.ftpConnected)
    {
        if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("❌ FTP not connected");
        }
        return false;
    }

    String cmd = "AT+QFTPDELE=\"" + String(filename) + "\"";
    if (!sendATCommand(cmd.c_str(), "+QFTPDELE: 0,0", 10000))
    {
        if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("❌ Failed to delete file");
        }
        return false;
    }

    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("✅ File deleted successfully\n");
    }
    return true;
}

bool Service_EG800K::renameFTP(const char *oldName, const char *newName)
{
    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("\n========== FTP RENAME ==========");
        Serial.println("From: " + String(oldName));
        Serial.println("To: " + String(newName));
    }

    if (!atObject_Data.ftpConnected)
    {
        if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("❌ FTP not connected");
        }
        return false;
    }

    String cmd = "AT+QFTPRENAME=\"" + String(oldName) + "\",\"" + String(newName) + "\"";
    if (!sendATCommand(cmd.c_str(), "+QFTPRENAME: 0,0", 10000))
    {
        if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("❌ Failed to rename file");
        }
        return false;
    }

    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("✅ File renamed successfully\n");
    }
    return true;
}

bool Service_EG800K::removeDirectoryFTP(const char *dirname)
{
    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("\n========== FTP RMDIR (Remove Directory) ==========");
        Serial.println("Directory: " + String(dirname));
    }

    if (!atObject_Data.ftpConnected)
    {
        if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("❌ FTP not connected");
        }
        return false;
    }

    String cmd = "AT+QFTPRMDIR=\"" + String(dirname) + "\"";
    if (!sendATCommand(cmd.c_str(), "+QFTPRMDIR: 0,0", 10000))
    {
        if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("❌ Failed to remove directory");
        }
        return false;
    }

    if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("✅ Directory removed successfully\n");
    }
    return true;
}

// void Service_EG800K::configGPS()
// {
//     if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
//     {
//         Serial.println("Configuring GPS...");
//     }
//     sendATCommand("AT+QGPS=1", 10000);          // Enable GPS
//     sendATCommand("AT+QGPS?", DEFAULT_TIMEOUT); // Check GPS status
//     // sendATCommand("AT+QGPSCFG=?", DEFAULT_TIMEOUT); // Kiểm tra cấu hình GPS
//     // sendATCommand("AT+QGPSCFG=\"gpsnmeatype\",2", DEFAULT_TIMEOUT); // Chỉ định dạng NMEA
//     // sendATCommand("AT+QGPSCFG=\"gnssconfig\",0", DEFAULT_TIMEOUT);  // Cấu hình GNSS: GPS
//     // sendATCommand("AT+QGPSSTATUS?", DEFAULT_TIMEOUT); // Kiểm tra trạng thái GPS
//     // sendATCommand("AT+QGPSLOC?", DEFAULT_TIMEOUT); // Lấy vị trí GPS
// }

// void Service_EG800K::getPosition()
// {
//     // Lấy vị trí GPS
//     if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
//     {
//         Serial.println("Getting GPS position...");
//     }
//     sendATCommand("AT+QGPSLOC=0", DEFAULT_TIMEOUT); // Lấy vị trí GPS
// }

/**
 * This start function will init some critical function
 */
void Service_EG800K::Service_EG800K_Start()
{
}
/**
 * Execute fuction of SNM app
 */
void Service_EG800K::Service_EG800K_Execute()
{
    // if (atService_EG800K.User_Mode == SER_USER_MODE_DEBUG)
    // {
    // }
}
void Service_EG800K::Service_EG800K_End() {}

#endif
