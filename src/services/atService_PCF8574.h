#ifndef _Service_atService_PCF8574_
#define _Service_atService_PCF8574_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "Service.h"
#include <PCF8574.h>
#include "../src/services/i2c/atService_I2C.h"
/* _____DEFINATIONS__________________________________________________________ */
#define PCF_ADDR 0x20 // PCF8574 I2C address
// #ifndef SDA_PIN
// #define SDA_PIN 48 // Default SDA pin
// #endif
// #ifndef SCL_PIN
// #define SCL_PIN 45 // Default SCL pin
// #endif
/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */
PCF8574 pcf(0x20);
/**
 * This Service class is the Service to manage the
 */
class Service_PCF8574 : public Service
{
public:
    Service_PCF8574();
    ~Service_PCF8574();

    void blinkLED(uint8_t ledPin, uint16_t delayTime);
    void pcfDigitalWrite(uint8_t ledPin, bool value);
    bool pcfDigitalRead(uint8_t pin);

protected:
private:
    static void Service_PCF8574_Start();
    static void Service_PCF8574_Execute();
    static void Service_PCF8574_End();
} atService_PCF8574;
/**
 * This function will be automaticaly called when a object is created by this class
 */
Service_PCF8574::Service_PCF8574(/* args */)
{
    _Start_User = *Service_PCF8574_Start;
    _Execute_User = *Service_PCF8574_Execute;
    _End_User = *Service_PCF8574_End;

    // change the ID of Service
    ID_Service = 1;
    // change the Service name
    Name_Service = (char *)"PCF8574 Service";
    // change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
Service_PCF8574::~Service_PCF8574()
{
}
/**
 * This start function will init some critical function
 */
void Service_PCF8574::Service_PCF8574_Start()
{
    atService_I2C.Run_Service();
    atService_I2C.checkIn(); // Ensure I2C bus is ready
    Wire.begin(SDA_PIN, SCL_PIN);
    pcf.pinMode(P2, OUTPUT);
    pcf.pinMode(P3, OUTPUT);
    pcf.pinMode(P4, INPUT);
    pcf.pinMode(P5, INPUT);
    pcf.pinMode(P6, INPUT);
    pcf.pinMode(P7, INPUT);
    if (!pcf.begin())
    {
        if (atService_PCF8574.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("Không tìm thấy PCF8574!");
        }

        for (int i = 0; i < 10; i++)
        {
            if (atService_PCF8574.User_Mode == SER_USER_MODE_DEBUG)
            {
                Serial.print("Retrying to connect to PCF8574...");
            }

            if (pcf.begin())
            {
                if (atService_PCF8574.User_Mode == SER_USER_MODE_DEBUG)
                {
                    Serial.println("PCF8574 initialized successfully!");
                }
                break;
            }
            if (i == 9)
            {
                if (atService_PCF8574.User_Mode == SER_USER_MODE_DEBUG)
                {
                    Serial.println("Failed to connect to PCF8574 after 10 attempts.");
                    atService_PCF8574.Service_State = SER_STATE_ENDING; // Set error state if initialization fails
                }
            }
            vTaskDelay(2000 / portTICK_PERIOD_MS); // Delay 2 seconds before retrying
        }
    }
    else
    {
        Serial.println("PCF8574 initialized successfully!");
    }
    pcf.digitalWrite(P2, HIGH); // Set P2 to HIGH
    pcf.digitalWrite(P3, HIGH); // Set P3 to HIGH
    atService_I2C.checkOut();   // Release I2C bus access
}
void Service_PCF8574::Service_PCF8574_Execute()
{
}
void Service_PCF8574::Service_PCF8574_End()
{
}
void Service_PCF8574::blinkLED(uint8_t ledPin, uint16_t delayTime)
{
    atService_I2C.checkIn();       // Ensure I2C bus is ready
    pcf.digitalWrite(ledPin, LOW); // Turn on the LED
    atService_I2C.checkOut();      // Release I2C bus access
    vTaskDelay(delayTime / portTICK_PERIOD_MS);
    atService_I2C.checkIn();        // Ensure I2C bus is ready
    pcf.digitalWrite(ledPin, HIGH); // Turn off the LED
    atService_I2C.checkOut();       // Release I2C bus access
    vTaskDelay(delayTime / portTICK_PERIOD_MS);
}
void Service_PCF8574::pcfDigitalWrite(uint8_t ledPin, bool value)
{
    atService_I2C.checkIn();          // Ensure I2C bus is ready
    pcf.digitalWrite(ledPin, !value); // Write HIGH for ON and LOW for OFF
    atService_I2C.checkOut();         // Release I2C bus access
}
bool Service_PCF8574::pcfDigitalRead(uint8_t pin)
{
    bool state = false;
    atService_I2C.checkIn(); // Ensure I2C bus is ready
    switch (pin)
    {
    case 1:
        state = pcf.digitalRead(P4);
        break;
    case 2:
        state = pcf.digitalRead(P5);
        break;
    case 3:
        state = pcf.digitalRead(P6);
        break;
    case 4:
        state = pcf.digitalRead(P7);
        break;
    default:
        if (atService_PCF8574.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("Reading state of unknown pin!");
        }
    }
    atService_I2C.checkOut(); // Release I2C bus access
    return state;
}

#endif
