#ifndef _Service_atService_74HC595D_
#define _Service_atService_74HC595D_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "Service.h"
/* _____DEFINATIONS__________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */
/**
 * This Service class is the Service to manage the 
 */
class Service_74HC595D : public Service
{
public:
    Service_74HC595D();
    ~Service_74HC595D();
    
protected:
     
private:
    static void  Service_74HC595D_Start();
    static void  Service_74HC595D_Execute();    
    static void  Service_74HC595D_End();
} atService_74HC595D ;
/**
 * This function will be automaticaly called when a object is created by this class
 */
Service_74HC595D::Service_74HC595D(/* args */)
{
    _Start_User      = *Service_74HC595D_Start;
    _Execute_User    = *Service_74HC595D_Execute;
    _End_User        = *Service_74HC595D_End;

    // change the ID of Service
    ID_Service = 1;
    // change the Service name
    Name_Service = (char*)"74HC595D Service";
    // change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
Service_74HC595D::~Service_74HC595D()
{
    
}
/**
 * This start function will init some critical function 
 */
void  Service_74HC595D::Service_74HC595D_Start()
{
    
}  

/**
 * Execute fuction of SNM app
 */
void  Service_74HC595D::Service_74HC595D_Execute()
{   
    if(atService_74HC595D.User_Mode == SER_USER_MODE_DEBUG)
    {
        
    }   
}    
void  Service_74HC595D::Service_74HC595D_End(){}

#endif


