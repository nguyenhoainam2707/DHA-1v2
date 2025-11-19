#ifndef _Service_atService_MCP23008_
#define _Service_atService_MCP23008_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "Service.h"
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23008.h>
#include "../src/services/i2c/atService_I2C.h"

/* _____DEFINATIONS__________________________________________________________ */
#define SDA_PIN 48
#define SCL_PIN 45
#define INT_PIN 47
#define RESET_PIN 8
/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */
/**
 * This Service class is the Service to manage the 
 */
class Service_MCP23008 : public Adafruit_MCP23008
{
public:
    Service_MCP23008();
    ~Service_MCP23008();
    bool  User_Mode = SER_USER_MODE_DEBUG;
    void  Service_MCP23008_Start();
    void  Service_MCP23008_Execute();    
    void  Service_MCP23008_End();
protected:
     
private:
}  ;
/**
 * This function will be automaticaly called when a object is created by this class
 */
Service_MCP23008::Service_MCP23008(/* args */)
{

}
/**
 * This function will be automaticaly called when the object of class is delete
 */
Service_MCP23008::~Service_MCP23008()
{
    
}
/**
 * This start function will init some critical function 
 */
void  Service_MCP23008::Service_MCP23008_Start()
{
    // check I2C bus is initialized
    atService_I2C.Run_Service();
    atService_I2C.checkIn();
    this->begin(0x20);
    atService_I2C.checkOut();
}  

/**
 * Execute fuction of SNM app
 */
void  Service_MCP23008::Service_MCP23008_Execute()
{   
    if(this->User_Mode == SER_USER_MODE_DEBUG)
    {
        
    }   
}    
void  Service_MCP23008::Service_MCP23008_End(){}

#endif


