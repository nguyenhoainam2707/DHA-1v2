#ifndef Object_Data_h
#define Object_Data_h

/* _____PROJECT INCLUDES____________________________________________________ */
#include "Arduino.h"
#include "Object.h"
/* _____DEFINITIONS__________________________________________________________ */
#define MAX_LED_CHANNEL 10
#define MAX_BUTTON_HOLD_TIME_SECONDS 20
#define MAX_BUTTON_MULTI_CLICK 10
enum WORKING_MODE
{
    WORKING_MODE_NORMAL,
    WORKING_MODE_CONFIG,
    WORKING_MODE_DEBUG_ALL,
    WORKING_MODE_DEBUG_AI,
    WORKING_MODE_DEBUG_AP,
    WORKING_MODE_DEBUG_BT,
    WORKING_MODE_DEBUG_CP,
    WORKING_MODE_DEBUG_DI,
    WORKING_MODE_DEBUG_DO,
    WORKING_MODE_DEBUG_FTP,
    WORKING_MODE_DEBUG_LCD,
    WORKING_MODE_DEBUG_LED,
    WORKING_MODE_DEBUG_MB1,
    WORKING_MODE_DEBUG_MB2,
    WORKING_MODE_DEBUG_RTC_V2,
    WORKING_MODE_DEBUG_SENSOR,
    WORKING_MODE_DEBUG_LOGGER
};

enum LED_CHANNEL
{
    LED_NONE,
    LED_SIM_STATUS,
    LED_ACCESS_POINT,
    LED_POWER,
    LED_RS485_CHANNEL_1,
    LED_RS485_CHANNEL_2,
    LED_FTP,
    LED_TCPIP,
    LED_LOGGER,
    LED_SD_CARD,
};
enum SENSOR_STATUS
{
    SENSOR_STATUS_NORMAL,
    SENSOR_STATUS_CALIB,
    SENSOR_STATUS_ERROR,
};
enum SENSOR_ALARM_STATUS
{
    SENSOR_ALARM_STATUS_NORMAL,
    SENSOR_ALARM_STATUS_HIGH,
    SENSOR_ALARM_STATUS_LOW,
};
struct RS485_DATA
{
    uint16_t high16bitByte = 0;
    uint16_t low16bitByte = 0;
};
/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */
/**
 * This class is the application to manage the
 */

class Object_Data : public Object
{
public:
    static void Object_Data_Start();
    static void Object_Data_Execute();
    static void Object_Data_End();

    // -------------------------------------- > AI Data channels < --------------------------------------
    uint16_t ai_raw[MAX_AI_CHANNEL] = {0, 0, 0, 0};              // Raw ADC values for 4 channels
    float ai_voltage[MAX_AI_CHANNEL] = {0.0f, 0.0f, 0.0f, 0.0f}; // Voltage values for 4 channels in volts
    float ai_current[MAX_AI_CHANNEL] = {0.0f, 0.0f, 0.0f, 0.0f}; // Current values for 4 channels in mA
    // bool ai_channel_connected[MAX_AI_CHANNEL] = {true, true, true, true}; // Channel connected status

    // -------------------------------------- > DO Data channels < --------------------------------------
    bool do_state[MAX_DO_CHANNEL] = {false, false, false, false};

    // -------------------------------------- > DI Data channels < --------------------------------------
    bool di_state[MAX_DI_CHANNEL] = {false, false, false, false};

    // -------------------------------------- > RS485 Data channels < ----------------------------------
    RS485_DATA rs485_data_list[MAX_RS485_CHANNEL][MAX_SENSORS];  // Data read from each slave
    uint8_t rs485_response_list[MAX_RS485_CHANNEL][MAX_SENSORS]; // save the response codes for each slave
    uint16_t rs485_calib_status_reg_list[MAX_RS485_CHANNEL][MAX_SENSORS]; // save the calibration status for each slave
    uint16_t rs485_error_status_reg_list[MAX_RS485_CHANNEL][MAX_SENSORS]; // save the error status for each slave

    // -------------------------------------- > Sensor Data < ------------------------------------------
    // Raw value of each sensor
    float sensor_raw_value_list[MAX_SENSORS] = {
        0.0, // Sensor 1
        1.0, // Sensor 2
        2.0, // Sensor 3
        3.0, // Sensor 4
        4.0, // Sensor 5
        5.0, // Sensor 6
        6.0, // Sensor 7
        7.0, // Sensor 8
        8.0, // Sensor 9
        9.0  // Sensor 10
    };
    // Calibrated value of each sensor_t after apply calibration function
    float sensor_calib_value_list[MAX_SENSORS] = {
        10.0, // Sensor 1
        11.0, // Sensor 2
        12.0, // Sensor 3
        13.0, // Sensor 4
        14.0, // Sensor 5
        15.0, // Sensor 6
        16.0, // Sensor 7
        17.0, // Sensor 8
        18.0, // Sensor 9
        19.0  // Sensor 10
    };
    // Sum value of each sensor after apply filter
    float sensor_sum_value_list[MAX_SENSORS] = {
        20.0, // Sensor 1
        21.0, // Sensor 2
        22.0, // Sensor 3
        23.0, // Sensor 4
        24.0, // Sensor 5
        25.0, // Sensor 6
        26.0, // Sensor 7
        27.0, // Sensor 8
        28.0, // Sensor 9
        29.0  // Sensor 10
    };
    // Status of each sensor
    uint8_t sensor_status_list[MAX_SENSORS] = {
        SENSOR_STATE_NORMAL, // Sensor 1
        SENSOR_STATE_NORMAL, // Sensor 2
        SENSOR_STATE_NORMAL, // Sensor 3
        SENSOR_STATE_NORMAL, // Sensor 4
        SENSOR_STATE_NORMAL, // Sensor 5
        SENSOR_STATE_NORMAL, // Sensor 6
        SENSOR_STATE_NORMAL, // Sensor 7
        SENSOR_STATE_NORMAL, // Sensor 8
        SENSOR_STATE_NORMAL, // Sensor 9
        SENSOR_STATE_NORMAL  // Sensor 10
    }; // use enum SENSOR_STATUS
    String sensor_status_code_list[MAX_SENSOR_STATUS] = {
        "00", // Normal
        "01", // Calib
        "02", // Error
        "03"  // Disable
    };
    // Alarm status of each sensor
    uint8_t sensor_alarm_status_list[MAX_SENSORS]; // use enum SENSOR_ALARM_STATUS
    uint8_t sensor_warning_status_list[MAX_SENSORS]; // use enum SENSOR_WARNING_STATUS

    // ------------------------------- > MQTT Data < ------------------------------------------
    String mqtt_payload; // Variable declaration, no value assignment here
    // ------------------------------- > SYSTEM Data < ------------------------------------------
    bool simReady;
    bool ftpConnected;
    bool mqttConnected;
    bool sdCardMounted;


    // ------------------------------------- > FTP Server Data < --------------------------------------
    // Content to send to each FTP server
    String ftp_file_content_list[MAX_FTP_SERVER] = {
        "Test Server 1 Content", // FTP Server 1
        "Test Server 2 Content", // FTP Server 2
        "Test Server 3 Content", // FTP Server 3
        "Test Server 4 Content", // FTP Server 4
    };
    bool ftp_sending_status_list[MAX_FTP_SERVER] = {false, false, false, false}; // Status of sending to each FTP server
    String ftp_response_string_list[MAX_FTP_SERVER] = {
        "Sent successfully", // FTP Server 1
        "Sent successfully", // FTP Server 2
        "Sent successfully", // FTP Server 3
        "Sent successfully", // FTP Server 4
    };
    // flag to send the new log
    bool ftp_send_log_flag_list[MAX_FTP_SERVER] = {
        false, // FTP Server 1
        false, // FTP Server 2
        false, // FTP Server 3
        false // FTP Server 4
    };
    // ------------------------------------- > Logger Data < --------------------------------------
    String logger_content_list[MAX_FTP_SERVER] = {
        "Test Server 1 Content", // FTP Server 1
        "Test Server 2 Content", // FTP Server 2
        "Test Server 3 Content", // FTP Server 3
        "Test Server 4 Content", // FTP Server 4
    };
    bool logger_new_log_list[MAX_FTP_SERVER] = {false};
    // ----------------------------------------- >RTC < ----------------------------------------------
    uint8_t rtc_hour = 0;
    uint8_t rtc_minute = 0;
    uint8_t rtc_second = 0;
    uint8_t rtc_day = 0;
    uint8_t rtc_month = 0;
    uint16_t rtc_year = 0;
    uint8_t rtc_day_of_week = 0;
    float rtc_temperature = 0;

    // ----------------------------------------> LEDS < ----------------------------------------------
    bool status_led[MAX_LED_CHANNEL] = {
        false, // LED_NONE
        true, // LED_SIM_STATUS
        false, // LED_ACCESS_POINT
        false, // LED_RS485_CHANNEL_1
        false, // LED_RS485_CHANNEL_2
        true, // LED_FTP
        false, // LED_TCPIP
        false, // LED_LOGGER
        true, // LED_SD_CARD
        true  // LED_POWER
    };
    // -----------------------------------------> Button <--------------------------------------------
    bool flagButtonHeldSecond[MAX_BUTTON_HOLD_TIME_SECONDS] = {false}; // Flag for button held send events
    bool flagButtonMultiClick[MAX_BUTTON_MULTI_CLICK] = {false};// Flag for button multi click events
    uint8_t buttonClickCount = 0;          // button click count
    // button pressed time number
    uint8_t buttonPressedTimeNumber = 0;
    // last time pressed
    uint32_t lastButtonPressedTime = 0;
    uint32_t lastButtonPressedTime_seconds = 0;
    uint32_t lastButtonPressedTime_minutes = 0;

    // -----------------------------------------> Working Mode <--------------------------------------------
    uint8_t working_mode = WORKING_MODE_NORMAL;

    // ------------------------------------------> ParamSync <----------------------------------------------
    bool sd_to_param_request = true; // Flag to request parameter sync
    bool param_to_sd_request = false; // Flag to request parameter sync
    Object_Data(/* args */);
    ~Object_Data();

protected:
private:
    /* data */
} atObject_Data;

/**
 * @brief This function code is executed one time when class is init
 *
 */
Object_Data::Object_Data(/* args */)
{
    _Start_User = *Object_Data_Start;
    _Execute_User = *Object_Data_Execute;
    _End_User = *Object_Data_End;
}
/**
 * @brief This function code is executed one time when class is deinit
 *
 */
Object_Data::~Object_Data()
{
}
/**
 * This start function will init some critical function
 */
void Object_Data::Object_Data_Start()
{
}
/**
 * Execute fuction of SNM app
 */
void Object_Data::Object_Data_Execute()
{
}
void Object_Data::Object_Data_End() {}

#endif