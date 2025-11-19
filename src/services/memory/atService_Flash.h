#ifndef _Service_atService_Flash_
#define _Service_atService_Flash_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "../src/services/Service.h"
/* _____DEFINATIONS__________________________________________________________ */
/* _____GLOBAL VARIABLES_____________________________________________________ */
SemaphoreHandle_t Flash_mutex = NULL; // Mutex for Flash bus access
/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */
/**
 * This Service class is the Service to manage the 
 */
class Service_Flash : public Service
{
public:
    Service_Flash();
    ~Service_Flash();

    void checkIn();
    void checkOut();

protected:
     
private:
    static void  Service_Flash_Start();
    static void  Service_Flash_Execute();    
    static void  Service_Flash_End();
} atService_Flash ;
/**
 * This function will be automaticaly called when a object is created by this class
 */
Service_Flash::Service_Flash(/* args */)
{
    _Start_User      = *Service_Flash_Start;
    _Execute_User    = *Service_Flash_Execute;
    _End_User        = *Service_Flash_End;

    // change the ID of Service
    ID_Service = 1;
    // change the Service name
    Name_Service = (char*)"Flash Service";
    // change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
Service_Flash::~Service_Flash()
{
    
}
/**
 * This start function will init some critical function 
 */
void  Service_Flash::Service_Flash_Start()
{
    // Initialize Flash bus
    if(Flash_mutex == NULL){
        Flash_mutex = xSemaphoreCreateMutex();
    }
}

/**
 * Execute fuction of SNM app
 */
void  Service_Flash::Service_Flash_Execute()
{   
    // if(atService_Flash.User_Mode == SER_USER_MODE_DEBUG)
    // {
        
    // }   
}    
void  Service_Flash::Service_Flash_End(){}

void Service_Flash::checkIn()
{
    xSemaphoreTake(Flash_mutex, portMAX_DELAY); // Take the mutex to access Flash bus
}

void Service_Flash::checkOut()
{
    xSemaphoreGive(Flash_mutex); // Release the mutex
}

#endif


