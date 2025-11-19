#ifndef _Service_atService_Modbus_
#define _Service_atService_Modbus_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "Service.h"
#include <ModbusMaster.h>
/* _____DEFINATIONS__________________________________________________________ */
#define MB1_RX_PIN 18 // RO  ← RX1
#define MB1_TX_PIN 17 // DI  → TX1
#define MB2_RX_PIN 5  // RO  ← RX2
#define MB2_TX_PIN 4  // DI  → TX2
/* _____GLOBAL VARIABLES_____________________________________________________ */
/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */
/**
 * This Service class is the Service to manage the
 */
class Service_Modbus //: public Service
{
public:
    Service_Modbus(HardwareSerial &serial);
    ~Service_Modbus();

    // static void mb1Init();
    // static void mb2Init();
    int User_Mode = SER_USER_MODE_NORMAL;
    void init(
        int32_t baud_rate,
        uint32_t config,
        int pin_rx,
        int pin_tx);
    // static uint16_t mb1ReadHoldingRegisters(uint16_t u16ReadAddress);
    // static uint16_t mb2ReadHoldingRegisters(uint16_t u16ReadAddress);
    SemaphoreHandle_t mutex_lock = NULL;
    // read multiple holding registers
    uint16_t atReadHoldingRegisters(uint8_t slave_id, uint16_t timeout, uint16_t u16ReadAddress, uint16_t u16ReadQty, uint16_t *data);
    // read multiple input registers
    uint16_t atReadInputRegisters(uint8_t slave_id, uint16_t timeout, uint16_t u16ReadAddress, uint16_t u16ReadQty, uint16_t *data);

    void setBaudRate(int32_t baud_rate)
    {
        this->baud_rate = baud_rate;
        this->serial.updateBaudRate(baud_rate);
    }
    void setConfig(uint32_t config)
    {
        this->config = config;
        // this->serial.updateConfig(config);
    }

protected:
private:
    int pin_rx = MB1_RX_PIN;
    int pin_tx = MB1_TX_PIN;
    int32_t baud_rate = 9600;
    uint32_t config = SERIAL_8N1;
    HardwareSerial &serial;
    ModbusMaster modbus_master;
    // static void Service_Modbus_Start();
    // static void Service_Modbus_Execute();
    // static void Service_Modbus_End();
};
Service_Modbus atService_Modbus1(Serial1);
Service_Modbus atService_Modbus2(Serial2);
/**
 * This function will be automaticaly called when a object is created by this class
 */
Service_Modbus::Service_Modbus(HardwareSerial &ser) : serial(ser)
{
    // this->serial = serial;
    // _Start_User = *Service_Modbus_Start;
    // _Execute_User = *Service_Modbus_Execute;
    // _End_User = *Service_Modbus_End;

    // // change the ID of Service
    // ID_Service = 1;
    // // change the Service name
    // Name_Service = (char *)"Modbus Service";
    // // change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
Service_Modbus::~Service_Modbus()
{
}
// /**
//  * This start function will init some critical function
//  */
// void Service_Modbus::Service_Modbus_Start() {}

// /**
//  * Execute fuction of SNM app
//  */
// void Service_Modbus::Service_Modbus_Execute()
// {
//     // if (atService_Modbus.User_Mode == SER_USER_MODE_DEBUG)
//     // {
//     // }
// }
// void Service_Modbus::Service_Modbus_End() {}

// void Service_Modbus::mb1Init()
// {
//     Serial1.begin(atObject_Param.baudRateMB1, SERIAL_8N1, MB1_RX_PIN, MB1_TX_PIN);
//     vTaskDelay(100 / portTICK_PERIOD_MS); // wait for serial to be ready
//     mb1.begin(atObject_Param.slaveIDMB1, Serial1);
//     if (atService_Modbus.User_Mode == SER_USER_MODE_DEBUG)
//     {
//         Serial.printf("Modbus RTU 1 over RS-485 ready!\nBaud rate: %d\n", atObject_Param.baudRateMB1);
//     }
// }

// void Service_Modbus::mb2Init()
// {
//     Serial2.begin(atObject_Param.baudRateMB2, SERIAL_8N1, MB2_RX_PIN, MB2_TX_PIN);
//     vTaskDelay(100 / portTICK_PERIOD_MS); // wait for serial to be ready
//     mb2.begin(atObject_Param.slaveIDMB2, Serial2);
//     if (atService_Modbus.User_Mode == SER_USER_MODE_DEBUG)
//     {
//         Serial.printf("Modbus RTU 2 over RS-485 ready!\nBaud rate: %d\n", atObject_Param.baudRateMB2);
//     }
// }
/**
 * @brief Initialize Modbus communication with specified parameters
 * @param baud_rate Baud rate for serial communication
 * @param config Configuration for serial communication (e.g., SERIAL_8N1)
 * @param pin_rx Pin number for RX
 * @param pin_tx Pin number for TX
 */
void Service_Modbus::init(int32_t baud_rate, uint32_t config, int pin_rx, int pin_tx)
{
    // begin serial communication
    this->serial.begin(baud_rate, config, pin_rx, pin_tx);
    // wait for serial to initialize
    vTaskDelay(100 / portTICK_PERIOD_MS);
    // begin Modbus communication
    // this->modbus_master.begin(slave_id, this->serial);

    // create mutex for thread-safe Modbus access
    this->mutex_lock = xSemaphoreCreateMutex();

    this->modbus_master.begin(2, this->serial); // default slave ID 1, can be changed in read function
    if (this->User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.printf("=== MODBUS INIT DEBUG ===\n");
        Serial.printf("RX Pin: %d, TX Pin: %d\n", pin_rx, pin_tx);
        Serial.printf("Baudrate: %d\n", baud_rate);

        // In ra địa chỉ object để xem có chung instance không
        Serial.printf("ModbusMaster object address: [%p]\n", &this->modbus_master);
        Serial.printf("Service object address: %p\n", this);

        Serial.printf("Serial [%p] configured with RX:%d, TX:%d\n", &this->serial, pin_rx, pin_tx);
        Serial.println("========================\n");
    }
}
/**
 * @brief Read multiple holding registers from a Modbus slave device
 * @param slave_id Modbus slave ID
 * @param u16ReadAddress Starting address to read from
 * @param u16ReadQty Number of registers to read
 * @param data Pointer to store the read data example data[10]
 *
 */
uint16_t Service_Modbus::atReadHoldingRegisters(uint8_t slave_id, uint16_t timeout, uint16_t u16ReadAddress, uint16_t u16ReadQty, uint16_t *data)
{
    xSemaphoreTake(this->mutex_lock, portMAX_DELAY); // Take the mutex to access Serial bus
    // change the slave ID for the modbus master
    this->modbus_master.begin(slave_id, this->serial);
    this->modbus_master.ku16MBResponseTimeout = timeout; // set response timeout
    // read holding registers
    uint8_t result = this->modbus_master.readHoldingRegisters(u16ReadAddress, u16ReadQty);
    if (result == modbus_master.ku8MBSuccess)
    {
        for (uint16_t i = 0; i < u16ReadQty; i++)
        {
            data[i] = this->modbus_master.getResponseBuffer(i);
        }
        if (this->User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.printf("MB: Slave ID [%d] Registers [%d-%d] = ", slave_id, u16ReadAddress, u16ReadAddress + u16ReadQty - 1);
            for (uint16_t i = 0; i < u16ReadQty; i++)
            {
                Serial.print(data[i]);
                if (i < u16ReadQty - 1)
                    Serial.print(", ");
            }
            Serial.println();
        }
    }
    else
    {
        if (this->User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.print("MB: Modbus error code: ");
            Serial.println(result, DEC);
        }
    }
    xSemaphoreGive(this->mutex_lock); // Release the mutex after accessing I2C bus
    return result;                    // Successfully read the requested number of registers
}

/**
 * @brief Read multiple input registers from a Modbus slave device
 * @param slave_id Modbus slave ID
 * @param u16ReadAddress Starting address to read from
 * @param u16ReadQty Number of registers to read
 * @param data Pointer to store the read data example data[10]
 *
 */
uint16_t Service_Modbus::atReadInputRegisters(uint8_t slave_id, uint16_t timeout, uint16_t u16ReadAddress, uint16_t u16ReadQty, uint16_t *data)
{
    xSemaphoreTake(this->mutex_lock, portMAX_DELAY); // Take the mutex to access I2C bus
    // change the slave ID for the modbus master
    this->modbus_master.begin(slave_id, this->serial);
    this->modbus_master.ku16MBResponseTimeout = timeout; // set response timeout
    // read input registers
    uint8_t result = this->modbus_master.readInputRegisters(u16ReadAddress, u16ReadQty);
    if (result == modbus_master.ku8MBSuccess)
    {
        for (uint16_t i = 0; i < u16ReadQty; i++)
        {
            data[i] = this->modbus_master.getResponseBuffer(i);
        }
        if (this->User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.printf("MB: Slave ID [%d] Input Registers [%d-%d] = ", slave_id, u16ReadAddress, u16ReadAddress + u16ReadQty - 1);
            for (uint16_t i = 0; i < u16ReadQty; i++)
            {
                Serial.print(data[i]);
                if (i < u16ReadQty - 1)
                    Serial.print(", ");
            }
            Serial.println();
        }
    }
    else
    {
        if (this->User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.print("MB: Modbus error code: ");
            Serial.println(result, DEC);
        }
    }
    xSemaphoreGive(this->mutex_lock); // Release the mutex after accessing I2C bus
    return result;                    // Successfully read the requested number of registers
}

#endif
