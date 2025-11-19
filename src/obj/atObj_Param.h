#ifndef Object_Param_h
#define Object_Param_h

/* _____PROJECT INCLUDES____________________________________________________ */
#include "Arduino.h"
#include "Object.h"
/* _____DEFINITIONS__________________________________________________________ */
#define MAX_SENSORS 10
#define MAX_SENSOR_STATUS 4
#define MAX_RS485_CHANNEL 2
#define MAX_FTP_SERVER 4
#define MAX_DO_CHANNEL 4
#define MAX_DI_CHANNEL 4
#define MAX_AI_CHANNEL 4
#define MAX_FTP_FILE_SEND_NUMBER_ONE_TIME 10
#define MAX_CALIB_TABLE_POINTS 10
#define LOGGER_CODE_YEAR "%YYYY%"
#define LOGGER_CODE_MONTH "%MM%"
#define LOGGER_CODE_DAY "%DD%"
#define LOGGER_CODE_HOUR "%HH%"
#define LOGGER_CODE_MINUTE "%mm%"
#define LOGGER_CODE_SECOND "%SS%"
#define LOGGER_CODE_MILLISECOND "%MS%"



struct ActionTime{
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
	bool request;
};
enum DO_Protocols
{
    PROTOCOL_DO_SENSOR_SCHMITT_TRIGGER,
    PROTOCOL_DO_SENSOR_STATE,
    PROTOCOL_DO_MQTT,
    PROTOCOL_DO_INPUT_DIGITAL,
};
enum FTP_SERVER_INDEX
{
    FTP_SERVER_1,
    FTP_SERVER_2,
    FTP_SERVER_3,
    FTP_SERVER_4
};
enum CalibFunctionOrder
{
    EQUATION_LINEAR,    // y = A + Bx
    EQUATION_QUADRATIC, // y = A + Bx + Cx^2
    EQUATION_CUBIC  ,   // y = A + Bx + Cx^2 + Dx^3
    EQUATION_TABLE     // Calib from table with linear regression between points
};
enum Sensor_Protocols
{
    PROTOCOL_ADC_4_20mA_CH1,
    PROTOCOL_ADC_4_20mA_CH2,
    PROTOCOL_ADC_4_20mA_CH3,
    PROTOCOL_ADC_4_20mA_CH4,
    PROTOCOL_RS485_CH1,
    PROTOCOL_RS485_CH2
};
enum Sensor_State_Reading_Protocols
{
    PROTOCOL_STATE_READING_AUTO,
    PROTOCOL_STATE_READING_DIGITAL_INPUT,
    PROTOCOL_STATE_READING_RS485,
};
enum Sensor_Value_Type
{
    SENSOR_VALUE_TYPE_RAW,
    SENSOR_VALUE_TYPE_CALIB,
    SENSOR_VALUE_TYPE_SUM
};
enum Sensor_Status
{
    SENSOR_STATE_NORMAL,
    SENSOR_STATE_CALIB,
    SENSOR_STATE_ERROR,
    SENSOR_STATE_DISABLE
};
enum RS485_Endian
{
    RS485_LITTLE_ENDIAN,
    RS485_BIG_ENDIAN
};
enum Sensor_Data_Type
{
    SENSOR_DATA_TYPE_UINT16,
    SENSOR_DATA_TYPE_INT16,
    SENSOR_DATA_TYPE_UINT32,
    SENSOR_DATA_TYPE_INT32,
    SENSOR_DATA_TYPE_FLOAT_BIG_ENDIAN,
    SENSOR_DATA_TYPE_FLOAT_BIG_ENDIAN_SWAPPED,
    SENSOR_DATA_TYPE_FLOAT_LITTLE_ENDIAN,
    SENSOR_DATA_TYPE_FLOAT_LITTLE_ENDIAN_SWAPPED
};
enum RS485_Channel
{
    RS485_CHANNEL_1,
    RS485_CHANNEL_2
};
enum AI_Channel
{
    AI_CHANNEL_1,
    AI_CHANNEL_2,
    AI_CHANNEL_3,
    AI_CHANNEL_4
};
enum DO_Channel
{
    DO_CHANNEL_1,
    DO_CHANNEL_2,
    DO_CHANNEL_3,
    DO_CHANNEL_4
};
enum DI_Channel
{
    DI_CHANNEL_1,
    DI_CHANNEL_2,
    DI_CHANNEL_3,
    DI_CHANNEL_4
};
struct CalibFunctionIndex
{
    float index_A = 1.0f;
    float index_B = 0.0f;
    float index_C = 0.0f;
    float index_D = 0.0f;

    CalibFunctionIndex() : index_A(0.0f), index_B(1.0f), index_C(0.0f), index_D(0.0f) {}
    CalibFunctionIndex(float a, float b, float c, float d) : index_A(a), index_B(b), index_C(c), index_D(d) {}
};

struct SensorCalibTablePoint{
    float raw_value;
    float calib_value;
};
struct CalibTablePoint
{
    float raw_value;
    float calib_value;

    CalibTablePoint() : raw_value(0.0f), calib_value(0.0f) {}
    CalibTablePoint(float raw, float calib) : raw_value(raw), calib_value(calib) {}
};
/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */
/**
 * This class is the application to manage the
 */

class Object_Param : public Object
{
public:
    static void Object_Param_Start();
    static void Object_Param_Execute();
    static void Object_Param_End();
    // ------------------------------ > Device Info < ------------------------------
    String device_name = "DHA-1"; // Device name
    String device_id = "DHA-100001"; // Device ID
    String sub_id = "0001"; // Sub ID
    String location_X = "11.1744424"; // Location X
    String location_Y = "106.1366673"; // Location Y
    String area = "TP. Ho Chi Minh"; // Area
    String device_type = "Nước ngầm"; // Device type
    
    // ----------------------- LCD Config Parameters ------------------------
    bool lcd_enable = true;           // Enable LCD
    bool lcd_backlight_enable = true; // Enable LCD backlight
    uint16_t lcd_refresh_rate = 1;    // Default refresh rate for LCD in seconds

    // ------------------------ AI Config Parameters ------------------------
    // Parameters for ADC 4-20mA
    // Enable ADC channels
    bool ai_enable_list[MAX_AI_CHANNEL] = {true, true, true, true};
    // Refresh rate for reading AI channels
    uint32_t ai_refresh_rate = 1; // in seconds

    // ------------------------ DO Config Parameters ------------------------
    // Enable DO channels
    bool do_enable_list[MAX_DO_CHANNEL] = {true, true, true, true};
    uint16_t do_refresh_rate = 1; // in seconds
    // --------- for trigger schmitt method
    uint8_t do_protocol_list[MAX_DO_CHANNEL] = {
        PROTOCOL_DO_SENSOR_SCHMITT_TRIGGER, // DO channel 1
        PROTOCOL_DO_SENSOR_STATE, // DO channel 2
        PROTOCOL_DO_SENSOR_SCHMITT_TRIGGER, // DO channel 3
        PROTOCOL_DO_INPUT_DIGITAL  // DO channel 4
  // DO channel 4
    };
    // Control DO from which sensor
    uint8_t do_control_schmitt_trigger_sensor_list[MAX_DO_CHANNEL] = {
        6, // DO channel 1 select sensor 7 to be control source
        7, // DO channel 2 select sensor 8 to be control source
        8, // DO channel 3 select sensor 9 to be control source
        9  // DO channel 4 select sensor 10 to be control source
    };
    // Control DO from sensor status
    uint8_t do_control_schmitt_trigger_sensor_valuetype_list[MAX_DO_CHANNEL] = {
        SENSOR_VALUE_TYPE_CALIB, // DO channel 1 select calib value of sensor 7 to be control source
        SENSOR_VALUE_TYPE_CALIB, // DO channel 2 select calib value of sensor 8 to be control source
        SENSOR_VALUE_TYPE_CALIB, // DO channel 3 select calib value of sensor 9 to be control source
        SENSOR_VALUE_TYPE_CALIB  // DO channel 4 select calib value of sensor 10 to be control source
    };
    float do_schmitt_trigger_upper_threshold_list[MAX_DO_CHANNEL] = {
        75.0f, // DO channel 1 upper threshold
        75.0f, // DO channel 2 upper threshold
        75.0f, // DO channel 3 upper threshold
        75.0f  // DO channel 4 upper threshold
    };
    float do_schmitt_trigger_lower_threshold_list[MAX_DO_CHANNEL] = {
        10.0f, // DO channel 1 lower threshold
        10.0f, // DO channel 2 lower threshold
        10.0f, // DO channel 3 lower threshold
        10.0f  // DO channel 4 lower threshold
    };
    // ------------------------  for control DO by sensor states
    uint8_t do_control_sensor_list[MAX_DO_CHANNEL] = {
        0, // Control DO channel 1 from sensor 1 state
        1, // Control DO channel 2 from sensor 2 state
        2, // Control DO channel 3 from sensor 3 state
        3  // Control DO channel 4 from sensor 4 state
    };
    uint8_t do_control_sensor_state_list[MAX_DO_CHANNEL] = {
        SENSOR_STATE_ERROR, // Desired sensor state to turn ON DO channel 1
        SENSOR_STATE_ERROR, // Desired sensor state to turn ON DO channel 2
        SENSOR_STATE_ERROR, // Desired sensor state to turn ON DO channel 3
        SENSOR_STATE_ERROR  // Desired sensor state to turn ON DO channel 4
    };
    // ------------------------  for control DO by MQTT command
    // to be implemented
    // ------------------------- for control DO by Digital Input
    uint8_t do_control_input_digital_list[MAX_DO_CHANNEL] = {
        DI_CHANNEL_1, // Control DO channel 1 from DI1
        DI_CHANNEL_2, // Control DO channel 2 from DI2
        DI_CHANNEL_3, // Control DO channel 3 from DI3
        DI_CHANNEL_4  // Control DO channel 4 from DI4
    };
    // ------------------------- DI Config Parameters ------------------------
    // Enable DI channels
    bool di_enable_list[MAX_DI_CHANNEL] = {true, true, true, true};
    // DI refresh   rate
    uint16_t di_refresh_rate = 1; // in seconds
    bool di_control_sensor_state_enable_list[MAX_DI_CHANNEL] = {
        true, // DI channel 1
        true, // DI channel 2
        true, // DI channel 3
        true  // DI channel 4
    };
    uint8_t di_control_sensor_list[MAX_DI_CHANNEL] = {
        0, // sensor number 1
        1, // sensor number 2
        2, // sensor number 3
        3  // sensor number 4
    };
    uint8_t di_control_sensor_state_list[MAX_DI_CHANNEL] = {
        SENSOR_STATE_CALIB, 
        SENSOR_STATE_ERROR, 
        SENSOR_STATE_ERROR, 
        SENSOR_STATE_ERROR
    };

    // ------------------------------ > RS485s Config  < ------------------------------
    bool rs485_enable[MAX_RS485_CHANNEL] = {true, true}; // Enable RS485 channels
    // Config RS485 serial for channel
    bool rs485_slave_enable_list[MAX_RS485_CHANNEL][MAX_SENSORS] = {
        {true, true, true, true, true, true, true, true, true, true},   // RS485 Channel 1
        {true, true, true, true, true, true, true, true, true, true}    // RS485 Channel 2
    };
    // all value is 9600
    uint32_t rs485_slave_baudrate_list[MAX_RS485_CHANNEL][MAX_SENSORS] = {
        {
            9600, 9600, 9600, 9600, 9600,
            9600, 9600, 9600, 9600, 9600
        }, // RS485 Channel 1
        {
            9600, 9600, 9600, 9600, 9600,
            9600, 9600, 9600, 9600, 9600
        }  // RS485 Channel 2;
    };
    // Parity, Stop bit, Data bit config
    uint32_t rs485_slave_serial_config_list[MAX_RS485_CHANNEL][MAX_SENSORS] = {
        {SERIAL_8N1, SERIAL_8N1, SERIAL_8N1, SERIAL_8N1, SERIAL_8N1, SERIAL_8N1, SERIAL_8N1, SERIAL_8N1, SERIAL_8N1, SERIAL_8N1},  // RS485 Channel 1
        {SERIAL_8N1, SERIAL_8N1, SERIAL_8N1, SERIAL_8N1, SERIAL_8N1, SERIAL_8N1, SERIAL_8N1, SERIAL_8N1, SERIAL_8N1, SERIAL_8N1} // RS485 Channel 2
    }; 
    //  Config type data to read from slave
    uint8_t rs485_slave_id_list[MAX_RS485_CHANNEL][MAX_SENSORS] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},   // Slave ID of RS485 Channel 1
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}    // Slave ID of RS485 Channel 2
    };
    uint16_t rs485_slave_timeout_list[MAX_RS485_CHANNEL][MAX_SENSORS] = {
        {500, 500, 500, 500, 500, 500, 500, 500, 500, 500},   // Timeout in miliseconds
        {500, 500, 500, 500, 500, 500, 500, 500, 500, 500}
    };
    uint8_t rs485_slave_function_code_list[MAX_RS485_CHANNEL][MAX_SENSORS] = {
        {3, 3, 3, 3, 3, 3, 3, 3, 3, 3},// 1: Read Coils, 2: Read Discrete Inputs, 3: Read Holding Registers, 4: Read Input Registers
        {4, 4, 4, 4, 4, 4, 4, 4, 4, 4} // 1: Read Coils, 2: Read Discrete Inputs, 3: Read Holding Registers, 4: Read Input Registers
    }; // 3: Read Holding Registers, 4: Read Input Registers
    uint16_t rs485_slave_starting_address_list[MAX_RS485_CHANNEL][MAX_SENSORS] = {
        {    // rs485 channel 1
            0, // Sensor 1
            2, // Sensor 2
            4, // Sensor 3
            6, // Sensor 4
            8, // Sensor 5
            0, // Sensor 6
            2, // Sensor 7
            4, // Sensor 8
            6, // Sensor 9
            8  // Sensor 10
        },   
        {    // rs485 channel 2
            0, // Sensor 1
            2, // Sensor 2
            4, // Sensor 3
            6, // Sensor 4
            8, // Sensor 5
            0, // Sensor 6
            2, // Sensor 7
            4, // Sensor 8
            6, // Sensor 9
            8  // Sensor 10
        }
    };
    uint8_t rs485_slave_quantity_list[MAX_RS485_CHANNEL][MAX_SENSORS] = {
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2},   // Quantity of registers to read for Channel 1
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}    // Quantity of registers to read for Channel 2
    };
    // Calibration status reading config
    boolean rs485_slave_calib_state_reading_enable_list[MAX_RS485_CHANNEL][MAX_SENSORS] = {
        {true, true, true, true, true, true, true, true, true, true},   // Enable calibration status reading for RS485 Channel 1
        {true, true, true, true, true, true, true, true, true, true}    // Enable calibration status reading for RS485 Channel 2
    };
    uint8_t rs485_slave_calib_state_function_code_list[MAX_RS485_CHANNEL][MAX_SENSORS] ={
        {3, 3, 3, 3, 3, 3, 3, 3, 3, 3},// 1: Read Coils, 2: Read Discrete Inputs, 3: Read Holding Registers, 4: Read Input Registers
        {3, 3, 3, 3, 3, 3, 3, 3, 3, 3} // 1: Read Coils, 2: Read Discrete Inputs, 3: Read Holding Registers, 4: Read Input Registers
    };
    uint16_t rs485_slave_state_calib_address_list[MAX_RS485_CHANNEL][MAX_SENSORS] = {
        {16, 16, 16, 16, 16, 16, 16, 16, 16, 16},   // Address to read status register from slave for RS485 Channel 1
        {16, 16, 16, 16, 16, 16, 16, 16, 16, 16}   // Address to read status register from slave for RS485 Channel 2
    };
    uint16_t rs485_slave_state_calib_value_list[MAX_RS485_CHANNEL][MAX_SENSORS] = {
        {100, 100, 100, 100, 100, 100, 100, 100, 100, 100},   // Bit position to read status register from slave for RS485 Channel 1
        {100, 100, 100, 100, 100, 100, 100, 100, 100, 100}    // Bit position to read status register from slave for RS485 Channel 2
    };
    // error status reading config
    boolean rs485_slave_error_state_reading_enable_list[MAX_RS485_CHANNEL][MAX_SENSORS] = {
        {true, true, true, true, true, true, true, true, true, true},   // Enable error status reading for RS485 Channel 1
        {true, true, true, true, true, true, true, true, true, true}    // Enable error status reading for RS485 Channel 2
    };
    uint8_t rs485_slave_error_state_function_code_list[MAX_RS485_CHANNEL][MAX_SENSORS] ={
        {3, 3, 3, 3, 3, 3, 3, 3, 3, 3},// 1: Read Coils, 2: Read Discrete Inputs, 3: Read Holding Registers, 4: Read Input Registers
        {3, 3, 3, 3, 3, 3, 3, 3, 3, 3} // 1: Read Coils, 2: Read Discrete Inputs, 3: Read Holding Registers, 4: Read Input Registers
    };
    uint16_t rs485_slave_state_error_address_list[MAX_RS485_CHANNEL][MAX_SENSORS] = {
        {17, 17, 17, 17, 17, 17, 17, 17, 17, 17},   // Address to read error status register from slave for RS485 Channel 1
        {17, 17, 17, 17, 17, 17, 17, 17, 17, 17}    // Address to read error status register from slave for RS485 Channel 2
    };
    uint16_t rs485_slave_state_error_value_list[MAX_RS485_CHANNEL][MAX_SENSORS] = {
        {200, 200, 200, 200, 200, 200, 200, 200, 200, 200},   // Bit position to read error status register from slave for RS485 Channel 1
        {200, 200, 200, 200, 200, 200, 200, 200, 200, 200}    // Bit position to read error status register from slave for RS485 Channel 2
    };


    
    // ------------------------------ > Sensor Config < ------------------------------
    // Sensor ID for each sensor
    uint8_t sensor_id_list[MAX_SENSORS] = {
        1, // Sensor 1
        2, // Sensor 2
        3, // Sensor 3
        4, // Sensor 4
        5, // Sensor 5
        6, // Sensor 6
        7, // Sensor 7
        8, // Sensor 8
        9, // Sensor 9
        10 // Sensor 10
    };
    // Enable each sensor
    bool sensor_enable_list[MAX_SENSORS] = {
        true, // Sensor 1
        true, // Sensor 2
        true, // Sensor 3
        true, // Sensor 4
        true, // Sensor 5
        true, // Sensor 6
        true, // Sensor 7
        true, // Sensor 8
        true, // Sensor 9
        true  // Sensor 10
    };
    // Name for each sensor
    String sensor_name_list[MAX_SENSORS] = {
        "NO1", // Sensor 1
        "NO1123123123123123", // Sensor 2
        "NO1 3", // Sensor 3
        "NO1 4", // Sensor 4
        "NO1 5", // Sensor 5
        "NO1123123123123123", // Sensor 6
        "NO1 7", // Sensor 7
        "NO1 8", // Sensor 8
        "NO1 9", // Sensor 9
        "NO1 10" // Sensor 10
    };
    // Unit for each sensor
    String sensor_unit_list[MAX_SENSORS] = {
        "m3/h", // Sensor 1
        "m3/h", // Sensor 2
        "m3/h", // Sensor 3
        "m3/h", // Sensor 4
        "m3/h", // Sensor 5
        "m3/h", // Sensor 6
        "m3/h", // Sensor 7
        "m3/h", // Sensor 8
        "m3/h", // Sensor 9
        "m3/h" // Sensor 10
    };
    // Protocol for each sensor
    uint8_t sensor_protocol_list[MAX_SENSORS] = {
        PROTOCOL_ADC_4_20mA_CH1, // Sensor 1
        PROTOCOL_ADC_4_20mA_CH2, // Sensor 2
        PROTOCOL_ADC_4_20mA_CH3, // Sensor 3
        PROTOCOL_ADC_4_20mA_CH4, // Sensor 4
        PROTOCOL_RS485_CH1,      // Sensor 5
        PROTOCOL_RS485_CH1,      // Sensor 6
        PROTOCOL_RS485_CH1,      // Sensor 7
        PROTOCOL_RS485_CH1,      // Sensor 8
        PROTOCOL_RS485_CH1,      // Sensor 9
        PROTOCOL_RS485_CH1       // Sensor 10
    };
    // Protocol for reading sensor state
    uint8_t sensor_state_protocol_list[MAX_SENSORS] = {
        PROTOCOL_STATE_READING_DIGITAL_INPUT,      // Sensor 1
        PROTOCOL_STATE_READING_DIGITAL_INPUT,      // Sensor 2
        PROTOCOL_STATE_READING_DIGITAL_INPUT,      // Sensor 3
        PROTOCOL_STATE_READING_DIGITAL_INPUT,      // Sensor 4
        PROTOCOL_STATE_READING_AUTO,      // Sensor 5
        PROTOCOL_STATE_READING_AUTO,      // Sensor 6
        PROTOCOL_STATE_READING_RS485,      // Sensor 7
        PROTOCOL_STATE_READING_RS485,      // Sensor 8
        PROTOCOL_STATE_READING_RS485,      // Sensor 9
        PROTOCOL_STATE_READING_RS485       // Sensor 10
    };
    // Just use within protocol RS485. When a sensor chooses PROTOCOL_RS485_CH1, or PROTOCOL_RS485_CH2
    // is its protocol, user has to config which sensor_rs485_index is used for this protocol.
    // By the default, each RS485 has MAX_SENSOR number of registers (1 byte or 2byte) to read
    // from slave modbus device.
    uint8_t sensor_rs485_index[MAX_RS485_CHANNEL][MAX_SENSORS] = {
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9},     // RS485 Channel 1
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}      // RS485 Channel 2
    };
    // data type of this sensor
    uint8_t sensor_data_type_list[MAX_SENSORS] = {
        SENSOR_DATA_TYPE_FLOAT_BIG_ENDIAN_SWAPPED, // Sensor 1
        SENSOR_DATA_TYPE_FLOAT_BIG_ENDIAN_SWAPPED, // Sensor 2
        SENSOR_DATA_TYPE_FLOAT_BIG_ENDIAN_SWAPPED, // Sensor 3
        SENSOR_DATA_TYPE_FLOAT_BIG_ENDIAN_SWAPPED, // Sensor 4
        SENSOR_DATA_TYPE_FLOAT_BIG_ENDIAN_SWAPPED, // Sensor 5
        SENSOR_DATA_TYPE_FLOAT_BIG_ENDIAN_SWAPPED, // Sensor 6
        SENSOR_DATA_TYPE_FLOAT_BIG_ENDIAN_SWAPPED, // Sensor 7
        SENSOR_DATA_TYPE_FLOAT_BIG_ENDIAN_SWAPPED, // Sensor 8
        SENSOR_DATA_TYPE_FLOAT_BIG_ENDIAN_SWAPPED, // Sensor 9
        SENSOR_DATA_TYPE_FLOAT_BIG_ENDIAN_SWAPPED, // Sensor 10
    };
    
    // Reading interval for each sensor
    uint16_t sensor_read_interval_list[MAX_SENSORS] = {
        1000, // Sensor 1
        1000, // Sensor 2
        1000, // Sensor 3
        1000, // Sensor 4
        1000, // Sensor 5
        1000, // Sensor 6
        1000, // Sensor 7
        1000, // Sensor 8
        1000, // Sensor 9
        1000  // Sensor 10
    };
    // Calibration function order for each sensor
    uint8_t sensor_calib_function_order_list[MAX_SENSORS] = {
        EQUATION_LINEAR, // Sensor 1
        EQUATION_LINEAR, // Sensor 2
        EQUATION_LINEAR, // Sensor 3
        EQUATION_LINEAR, // Sensor 4
        EQUATION_LINEAR, // Sensor 5
        EQUATION_LINEAR, // Sensor 6
        EQUATION_LINEAR, // Sensor 7
        EQUATION_LINEAR, // Sensor 8
        EQUATION_LINEAR, // Sensor 9
        EQUATION_TABLE  // Sensor 10
    };
    
    // Calibration function indexes for each sensor
    CalibFunctionIndex sensor_calib_function_index_list[MAX_SENSORS] = {
        {0.0f, 1.0f, 0.0f, 0.0f}, // Sensor 1
        {0.0f, 1.0f, 0.0f, 0.0f}, // Sensor 2
        {0.0f, 1.0f, 0.0f, 0.0f}, // Sensor 3
        {0.0f, 1.0f, 0.0f, 0.0f}, // Sensor 4
        {0.0f, 1.0f, 0.0f, 0.0f}, // Sensor 5
        {0.0f, 1.0f, 0.0f, 0.0f}, // Sensor 6
        {0.0f, 1.0f, 0.0f, 0.0f}, // Sensor 7
        {0.0f, 1.0f, 0.0f, 0.0f}, // Sensor 8
        {0.0f, 1.0f, 0.0f, 0.0f}, // Sensor 9
        {0.0f, 1000.0f, 0.0f, 0.0f}  // Sensor 10
    };
    // table of calibration points for each sensor
    CalibTablePoint sensor_calib_table_list[MAX_SENSORS][MAX_CALIB_TABLE_POINTS] = {
        {{0.0f, 0.0f}, {100.0f, 100.0f}, {200.0f, 200.0f}, {300.0f, 300.0f}, {400.0f, 400.0f}, {500.0f, 500.0f}, {600.0f, 600.0f}, {700.0f, 700.0f}, {800.0f, 800.0f}, {900.0f, 900.0f}}, // Sensor 1
        {{0.0f, 0.0f}, {100.0f, 100.0f}, {200.0f, 200.0f}, {300.0f, 300.0f}, {400.0f, 400.0f}, {500.0f, 500.0f}, {600.0f, 600.0f}, {700.0f, 700.0f}, {800.0f, 800.0f}, {900.0f, 900.0f}}, // Sensor 2
        {{0.0f, 0.0f}, {100.0f, 100.0f}, {200.0f, 200.0f}, {300.0f, 300.0f}, {400.0f, 400.0f}, {500.0f, 500.0f}, {600.0f, 600.0f}, {700.0f, 700.0f}, {800.0f, 800.0f}, {900.0f, 900.0f}}, // Sensor 3
        {{0.0f, 0.0f}, {100.0f, 100.0f}, {200.0f, 200.0f}, {300.0f, 300.0f}, {400.0f, 400.0f}, {500.0f, 500.0f}, {600.0f, 600.0f}, {700.0f, 700.0f}, {800.0f, 800.0f}, {900.0f, 900.0f}}, // Sensor 4
        {{0.0f, 0.0f}, {100.0f, 100.0f}, {200.0f, 200.0f}, {300.0f, 300.0f}, {400.0f, 400.0f}, {500.0f, 500.0f}, {600.0f, 600.0f}, {700.0f, 700.0f}, {800.0f, 800.0f}, {900.0f, 900.0f}}, // Sensor 5
        {{0.0f, 0.0f}, {100.0f, 100.0f}, {200.0f, 200.0f}, {300.0f, 300.0f}, {400.0f, 400.0f}, {500.0f, 500.0f}, {600.0f, 600.0f}, {700.0f, 700.0f}, {800.0f, 800.0f}, {900.0f, 900.0f}}, // Sensor 6
        {{0.0f, 0.0f}, {100.0f, 100.0f}, {200.0f, 200.0f}, {300.0f, 300.0f}, {400.0f, 400.0f}, {500.0f, 500.0f}, {600.0f, 600.0f}, {700.0f, 700.0f}, {800.0f, 800.0f}, {900.0f, 900.0f}}, // Sensor 7
        {{0.0f, 0.0f}, {100.0f, 100.0f}, {200.0f, 200.0f}, {300.0f, 300.0f}, {400.0f, 400.0f}, {500.0f, 500.0f}, {600.0f, 600.0f}, {700.0f, 700.0f}, {800.0f, 800.0f}, {900.0f, 900.0f}}, // Sensor 8
        {{0.0f, 0.0f}, {100.0f, 100.0f}, {200.0f, 200.0f}, {300.0f, 300.0f}, {400.0f, 400.0f}, {500.0f, 500.0f}, {600.0f, 600.0f}, {700.0f, 700.0f}, {800.0f, 800.0f}, {900.0f, 900.0f}}, // Sensor 9
        {{0.0f, 0.0f}, {100.0f, 100.0f}, {200.0f, 200.0f}, {300.0f, 303.0f}, {400.0f, 400.0f}, {500.0f, 501.0f}, {600.0f, 600.0f}, {700.0f, 701.0f}, {800.0f, 800.0f}, {900.0f, 899.0f}}  // Sensor 10
    };
    // table of enabled calibration points for each sensor
    bool sensor_calib_table_point_active_list[MAX_SENSORS][MAX_CALIB_TABLE_POINTS] = {
        {true, true, true, true, true, true, true, true, true, true}, // Sensor 1
        {true, true, true, true, true, true, true, true, true, true}, // Sensor 2
        {true, true, true, true, true, true, true, true, true, true}, // Sensor 3
        {true, true, true, true, true, true, true, true, true, true}, // Sensor 4
        {true, true, true, true, true, true, true, true, true, true}, // Sensor 5
        {true, true, true, true, true, true, true, true, true, true}, // Sensor 6
        {true, true, true, true, true, true, true, true, true, true}, // Sensor 7
        {true, true, true, true, true, true, true, true, true, true}, // Sensor 8
        {true, true, true, true, true, true, true, true, true, true}, // Sensor 9
        {true, true, true, true, true, true, true, true, true, true}  // Sensor 10
    };
    // Enable error limit for each sensor.
    bool sensor_error_enable_list[MAX_SENSORS] = {
        true, // Sensor 1
        true, // Sensor 2
        true, // Sensor 3
        true, // Sensor 4
        true, // Sensor 5
        true, // Sensor 6
        true, // Sensor 7
        true, // Sensor 8
        true, // Sensor 9
        true  // Sensor 10
    };
    // Error lower threshold for each raw value of sensor. Whenever the
    // raw value is out of this range, the sensor status will be set to ERROR
    float sensor_raw_error_lower_threshold_list[MAX_SENSORS] = {
        0.0f, // Sensor 1
        0.0f, // Sensor 2
        0.0f, // Sensor 3
        0.0f, // Sensor 4
        0.0f, // Sensor 5
        0.0f, // Sensor 6
        0.0f, // Sensor 7
        0.0f, // Sensor 8
        0.0f, // Sensor 9
        0.0f  // Sensor 10
    };
    // Error upper threshold for each raw value of sensor. Whenever the
    // raw value is out of this range, the sensor status will be set to ERROR
    float sensor_raw_error_upper_threshold_list[MAX_SENSORS] = {
        10.0f, // Sensor 1
        10.0f, // Sensor 2
        10.0f, // Sensor 3
        10.0f, // Sensor 4
        10.0f, // Sensor 5
        10.0f, // Sensor 6
        10.0f, // Sensor 7
        10.0f, // Sensor 8
        10.0f, // Sensor 9
        10.0f  // Sensor 10
    };
    // Error lower threshold for each calib value of sensor. Whenever the
    // calib value is out of this range, the sensor status will be set to ERROR
    float sensor_calib_error_lower_threshold_list[MAX_SENSORS] = {
        0.0f, // Sensor 1
        0.0f, // Sensor 2
        0.0f, // Sensor 3
        0.0f, // Sensor 4
        0.0f, // Sensor 5
        0.0f, // Sensor 6
        0.0f, // Sensor 7
        0.0f, // Sensor 8
        0.0f, // Sensor 9
        0.0f  // Sensor 10
    };
    // Error upper threshold for each calib value of sensor. Whenever the
    // calib value is out of this range, the sensor status will be set to ERROR
    float sensor_calib_error_upper_threshold_list[MAX_SENSORS] = {
        10000.0f, // Sensor 1
        10000.0f, // Sensor 2
        10000.0f, // Sensor 3
        10000.0f, // Sensor 4
        10000.0f, // Sensor 5
        10000.0f, // Sensor 6
        10000.0f, // Sensor 7
        10000.0f, // Sensor 8
        10000.0f, // Sensor 9
        10000.0f  // Sensor 10
    };
    // Enable limit for calib value
    bool sensor_limit_enable_list[MAX_SENSORS] = {
        false, // Sensor 1
        false, // Sensor 2
        false, // Sensor 3
        false, // Sensor 4
        false, // Sensor 5
        false, // Sensor 6
        false, // Sensor 7
        false, // Sensor 8
        false, // Sensor 9
        false  // Sensor 10
    };
    // Lower threshold for each calib value of sensor. Whenever the
    // calib value is out of this range, the calib value will be set to this lower threshold
    float sensor_calib_lower_limit_threshold_list[MAX_SENSORS] = {
        0.0f, // Sensor 1
        0.0f, // Sensor 2
        0.0f, // Sensor 3
        0.0f, // Sensor 4
        0.0f, // Sensor 5
        0.0f, // Sensor 6
        0.0f, // Sensor 7
        0.0f, // Sensor 8
        0.0f, // Sensor 9
        0.0f  // Sensor 10
    };
    // Lower limit noise for each sensor in the calib value is limit
    float sensor_lower_limit_noise_list[MAX_SENSORS] = {
        0.0f, // Sensor 1
        0.0f, // Sensor 2
        0.0f, // Sensor 3
        0.0f, // Sensor 4
        0.0f, // Sensor 5
        0.0f, // Sensor 6
        0.0f, // Sensor 7
        0.0f, // Sensor 8
        0.0f, // Sensor 9
        0.0f  // Sensor 10
    };
    // Upper threshold for each calib value of sensor. Whenever the
    // calib value is out of this range, the calib value will be set to this upper threshold
    float sensor_calib_upper_limit_threshold_list[MAX_SENSORS] = {
        100.0f, // Sensor 1
        100.0f, // Sensor 2
        100.0f, // Sensor 3
        100.0f, // Sensor 4
        100.0f, // Sensor 5
        100.0f, // Sensor 6
        100.0f, // Sensor 7
        100.0f, // Sensor 8
        100.0f, // Sensor 9
        100.0f  // Sensor 10
    };
    // Upper limit noise for each sensor in the calib value is limit
    float sensor_upper_limit_noise_list[MAX_SENSORS] = {
        1.0f, // Sensor 1
        1.0f, // Sensor 2
        1.0f, // Sensor 3
        1.0f, // Sensor 4
        1.0f, // Sensor 5
        1.0f, // Sensor 6
        1.0f, // Sensor 7
        1.0f, // Sensor 8
        1.0f, // Sensor 9
        1.0f  // Sensor 10
    };
    

    // ------------------------------ > MQTT Config < ------------------------------
    bool mqtt_public_enable = false;              // Enable MQTT
    bool mqtt_subscribe_enable = false;           // Enable MQTT subscribe
    uint16_t mqtt_sending_interval = 3;           // Default sending interval in seconds
    uint16_t mqtt_port = 1883;                    // Default MQTT port
    String mqtt_pub_topic = "at/esp32s3/data";    // Default MQTT topic to publish data
    String mqtt_sub_topic = "at/esp32s3/command"; // Default MQTT topic to subscribe command
    String mqtt_broker = "broker.emqx.io";        // Default MQTT server
    String mqtt_client_id = "DHA-1";              // Default MQTT client ID
    String mqtt_username = "username";            // Default MQTT username
    String mqtt_password = "password";            // Default MQTT password
    uint8_t mqtt_max_retry_times = 3;             // Max retry times if failed to connect to MQTT broker

    // ------------------------------------ > FTP Config < ------------------------------------
    // Active FTP server
    bool ftp_active_list[MAX_FTP_SERVER] = {
        true, // FTP Server 1
        false, // FTP Server 2
        false, // FTP Server 3
        false, // FTP Server 4
    };
    // FTP server host list
    String ftp_host_list[MAX_FTP_SERVER] = {
        "103.149.86.230", // FTP Server 1
        "103.149.86.230", // FTP Server 2
        "103.149.86.230", // FTP Server 3
        "103.149.86.230", // FTP Server 4 
    };
    // FTP server port list
    uint16_t ftp_port_list[MAX_FTP_SERVER] = {
        21, // FTP Server 1
        21, // FTP Server 2
        21, // FTP Server 3
        21, // FTP Server 4
    };
    // ftp_timeout for each FTP server
    uint16_t ftp_timeout_list[MAX_FTP_SERVER] = {
        10, // FTP Server 1
        10, // FTP Server 2
        10, // FTP Server 3
        10, // FTP Server 4
    };
    // FTP server user list
    String ftp_user_list[MAX_FTP_SERVER] = {
        "ftpuser", // FTP Server 1
        "ftpuser", // FTP Server 2
        "ftpuser", // FTP Server 3
        "ftpuser", // FTP Server 4
    };
    // FTP server password list
    String ftp_pass_list[MAX_FTP_SERVER] = {
        "abc123123", // FTP Server 1
        "abc123123", // FTP Server 2
        "abc123123", // FTP Server 3
        "abc123123", // FTP Server 4
    };
    // Path to send file to each FTP server
    String ftp_path_list[MAX_FTP_SERVER] = {
        "/Test/%YYYY%/%MM%/%DD%", // FTP Server 1
        "/Test/%YYYY%/%MM%/%DD%", // FTP Server 2
        "/Test/%YYYY%/%MM%/%DD%", // FTP Server 3
        "/Test/%YYYY%/%MM%/%DD%", // FTP Server 4
    };
    // File name to send to each FTP server
    String ftp_file_name_list[MAX_FTP_SERVER] = {
        "Test_%YYYY%%MM%%DD%%HH%%mm%%SS%.txt", // FTP Server 1
        "Test_%YYYY%%MM%%DD%%HH%%mm%%SS%.txt", // FTP Server 2
        "Test_%YYYY%%MM%%DD%%HH%%mm%%SS%.txt", // FTP Server 3
        "Test_%YYYY%%MM%%DD%%HH%%mm%%SS%.txt", // FTP Server 4
    };
    // // Sending interval for each FTP server in seconds
    // uint32_t ftp_sending_interval_list[MAX_FTP_SERVER] = {
    //     300, // FTP Server 1
    //     300, // FTP Server 2
    //     300, // FTP Server 3
    //     300, // FTP Server 4
    // };
    // Retry times if failed to send to each FTP server
    uint8_t ftp_retry_times_list[MAX_FTP_SERVER] = {
        3, // FTP Server 1
        3, // FTP Server 2
        3, // FTP Server 3
        3, // FTP Server 4
    };
    // Send Time ftp (minutes) for each FTP server
    uint8_t ftp_send_time_list[MAX_FTP_SERVER] = {
        1, // FTP Server 1 min 5, max 60
        1, // FTP Server 2 min 5, max 60
        1, // FTP Server 3 min 5, max 60
        1, // FTP Server 4 min 5, max 60
    };
    // Enable which sensor will use in content file. Examle, If
    // ftp_sensor_config[FTP_SERVER_1][0] = True, the content of this server will
    // be have information of sensor number 0.
    bool ftp_sensor_config[MAX_FTP_SERVER][MAX_SENSORS] = {
//Sensor 1  2  3  4  5  6  7  8  9 10
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // FTP Server 1
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // FTP Server 2
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // FTP Server 3
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // FTP Server 4
    };
    uint8_t ftp_max_file_number_per_one_send_time[MAX_FTP_SERVER] = {
        10, // FTP Server 1
        10, // FTP Server 2
        10, // FTP Server 3
        10, // FTP Server 4
    };
    // -------------------------------------------- > FTP (LOGGER) < ---------------------------------------------
    // Path to save file in sdCard
    String logger_save_path[MAX_FTP_SERVER] = {
        "/LOG/FTP1/UNSENT", // for FTP Server 1
        "/LOG/FTP2/UNSENT", // for FTP Server 2
        "/LOG/FTP3/UNSENT", // for FTP Server 3
        "/LOG/FTP4/UNSENT", // for FTP Server 4
    };
    String logger_save_sent_file_path[MAX_FTP_SERVER] = {
        "/LOG/FTP1/SENT", // for FTP Server 1
        "/LOG/FTP2/SENT", // for FTP Server 2
        "/LOG/FTP3/SENT", // for FTP Server 3
        "/LOG/FTP4/SENT", // for FTP Server 4
    };
    // -------------------------------------------- RTC Config --------------------------------------------
    bool rtc_setting_request = false; // Flag to request RTC time update from user setting
    uint8_t rtc_setting_hour    = 10;
    uint8_t rtc_setting_minute  = 10;
    uint8_t rtc_setting_second  = 10;
    uint8_t rtc_setting_day     = 10;
    uint8_t rtc_setting_month   = 10;
    uint16_t rtc_setting_year   = 2020;
    bool rtc_auto_update_from_ntp = true; // Enable auto update time from NTP server
    String ntp_server = "pool.ntp.org"; // NTP server address
    Object_Param(/* args */);
    ~Object_Param();

protected:
private:
    /* data */
} atObject_Param;

/**
 * @brief This function code is executed one time when class is init
 *
 */
Object_Param::Object_Param(/* args */)
{
    _Start_User = *Object_Param_Start;
    _Execute_User = *Object_Param_Execute;
    _End_User = *Object_Param_End;
}
/**
 * @brief This function code is executed one time when class is deinit
 *
 */
Object_Param::~Object_Param()
{
}
/**
 * This start function will init some critical function
 */
void Object_Param::Object_Param_Start()
{
}
/**
 * Execute fuction of SNM app
 */
void Object_Param::Object_Param_Execute()
{
}
void Object_Param::Object_Param_End() {}

#endif