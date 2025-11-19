#ifndef _Service_atService_I2C_
#define _Service_atService_I2C_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "../src/services/Service.h"
#include <Wire.h>
/* _____DEFINATIONS__________________________________________________________ */
#define SDA_PIN 48
#define SCL_PIN 45
/* _____GLOBAL VARIABLES_____________________________________________________ */
SemaphoreHandle_t i2c_mutex = NULL; // Mutex for I2C bus access

/* _____GLOBAL FUNCTION______________________________________________________ */
// TwoWire Wire1 = TwoWire(1); // Create a second I2C bus instance
/* _____CLASS DEFINITION_____________________________________________________ */
/**
 * This Service class is the Service to manage the
 */
class Service_I2C : public Service
{
public:
    Service_I2C();
    ~Service_I2C();

    void checkIn();
    void checkOut();
    void scanI2C();

    TwoWire WireInstance = TwoWire(0); // Create a second I2C bus instance
protected:
private:
    static void Service_I2C_Start();
    static void Service_I2C_Execute();
    static void Service_I2C_End();
} atService_I2C;
/**
 * This function will be automaticaly called when a object is created by this class
 */
Service_I2C::Service_I2C(/* args */)
{
    _Start_User = *Service_I2C_Start;
    _Execute_User = *Service_I2C_Execute;
    _End_User = *Service_I2C_End;

    // change the ID of Service
    ID_Service = 1;
    // change the Service name
    Name_Service = (char *)"I2C Service";
    // change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
Service_I2C::~Service_I2C()
{
}
/**
 * This start function will init some critical function
 */
void Service_I2C::Service_I2C_Start()
{

    // Initialize I2C bus
    if (i2c_mutex == NULL)
    {
        i2c_mutex = xSemaphoreCreateMutex();
    }
    atService_I2C.checkIn();
    Wire.begin(SDA_PIN, SCL_PIN);
    atService_I2C.checkOut();
}

/**
 * Execute fuction of SNM app
 */
void Service_I2C::Service_I2C_Execute()
{
    // if(atService_I2C.User_Mode == SER_USER_MODE_DEBUG)
    // {

    // }
}
void Service_I2C::Service_I2C_End() {}

void Service_I2C::checkIn()
{
    xSemaphoreTake(i2c_mutex, portMAX_DELAY); // Take the mutex to access I2C bus
}

void Service_I2C::checkOut()
{
    xSemaphoreGive(i2c_mutex); // Release the mutex
}

void Service_I2C::scanI2C()
{
    if (atService_I2C.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("\n=== I2C DEVICE SCAN ===");
    }
    uint8_t count = 0;

    checkIn(); // Lock I2C bus
    for (uint8_t addr = 1; addr < 127; addr++)
    {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0)
        {
            if (atService_I2C.User_Mode == SER_USER_MODE_DEBUG)
            {
                Serial.printf("Device found at 0x%02X\n", addr);
            }
            count++;
        }
    }
    checkOut(); // Unlock I2C bus
    if (atService_I2C.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.printf("Total devices found: %d\n", count);
        Serial.println("I2C Scan Complete\n");
    }
}

#endif
