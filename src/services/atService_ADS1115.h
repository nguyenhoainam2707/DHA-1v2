#ifndef _Service_atService_ADS1115_
#define _Service_atService_ADS1115_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "Service.h"
#include <Adafruit_ADS1X15.h>
#include "../src/services/i2c/atService_I2C.h"
/* _____DEFINATIONS__________________________________________________________ */
#define ADS1115_ADDRESS 0x48 // Default I2C address for ADS1115
/* _____GLOBAL VARIABLES_____________________________________________________ */
/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */

Adafruit_ADS1115 ads; // Tạo đối tượng cho ADS1115
/**
 * This Service class is the Service to manage the
 */
class Service_ADS1115 : public Service
{
public:
    Service_ADS1115();
    ~Service_ADS1115();

    static bool ADS1115_Init(adsGain_t gain, uint16_t rate);
    static int16_t ADS1115_readAI(uint8_t channel);
    static float ADS1115_readAI_Voltage(uint8_t channel);
    static float ADS1115_readAI_Current(uint8_t channel);
    static float computeVolts(int16_t counts)
    {
        return ads.computeVolts(counts);
    }
    static float computeCurrent(float voltage)
    {
        // Assuming a 100 Ohm shunt resistor for current measurement
        return voltage * 10; // Current in milliamperes (V = I * R, where R = 100 Ohm)
    }
protected:
private:
    static void Service_ADS1115_Start();
    static void Service_ADS1115_Execute();
    static void Service_ADS1115_End();
} atService_ADS1115;
/**
 * This function will be automaticaly called when a object is created by this class
 */
Service_ADS1115::Service_ADS1115(/* args */)
{
    _Start_User = *Service_ADS1115_Start;
    _Execute_User = *Service_ADS1115_Execute;
    _End_User = *Service_ADS1115_End;

    // change the ID of Service
    ID_Service = 1;
    // change the Service name
    Name_Service = (char *)"ADS1115 Service";
    // change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
Service_ADS1115::~Service_ADS1115()
{
}
/**
 * This start function will init some critical function
 */
void Service_ADS1115::Service_ADS1115_Start()
{
    atService_I2C.Run_Service();
}
/**
 * Execute fuction of SNM app
 */
void Service_ADS1115::Service_ADS1115_Execute() {}

void Service_ADS1115::Service_ADS1115_End() {}

bool Service_ADS1115::ADS1115_Init(adsGain_t gain, uint16_t rate)
{
    atService_I2C.checkIn(); // Ensure I2C bus is ready
    ads.begin(ADS1115_ADDRESS); // Lưu ý hàm này nếu không tìm được thiết bị giao tiếp I2C sẽ bị treo khoảng 2 giây.
    if (!ads.begin(ADS1115_ADDRESS))
    {
        atService_I2C.checkOut(); // Release I2C bus access
        return false;             // Check if the ADS1115 is connected
    }
    ads.setGain(gain);
    ads.setDataRate(rate);
    atService_I2C.checkOut(); // Release I2C bus access
    return true;
}
int16_t Service_ADS1115::ADS1115_readAI(uint8_t channel)
{
    atService_I2C.checkIn(); // Ensure I2C bus is ready
    int16_t value = ads.readADC_SingleEnded(channel);
    atService_I2C.checkOut();
    return value;
}
float Service_ADS1115::ADS1115_readAI_Voltage(uint8_t channel)
{
    atService_I2C.checkIn(); // Ensure I2C bus is ready
    float voltage = ads.computeVolts(ads.readADC_SingleEnded(channel));
    atService_I2C.checkOut();
    return voltage;
}
float Service_ADS1115::ADS1115_readAI_Current(uint8_t channel)
{
    atService_I2C.checkIn(); // Ensure I2C bus is ready
    float voltage = ads.computeVolts(ads.readADC_SingleEnded(channel));
    atService_I2C.checkOut();
    // Assuming a 1 Ohm shunt resistor for current measurement
    return voltage * 10; // Current in milliamperes (V = I * R, where R = 100 Ohm)
}
#endif
