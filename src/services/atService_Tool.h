#ifndef _Service_atService_Tool_
#define _Service_atService_Tool_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "Service.h"
/* _____DEFINATIONS__________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */
/**
 * This Service class is the Service to manage the
 */
class Service_Tool : public Service
{
public:
    Service_Tool();
    ~Service_Tool();
    static float randomFloat(float minVal, float maxVal);
    static int splitPath(String input, String parts[], int maxParts);

protected:
private:
    static void Service_Tool_Start();
    static void Service_Tool_Execute();
    static void Service_Tool_End();
} atService_Tool;
/**
 * This function will be automaticaly called when a object is created by this class
 */
Service_Tool::Service_Tool(/* args */)
{
    _Start_User = *Service_Tool_Start;
    _Execute_User = *Service_Tool_Execute;
    _End_User = *Service_Tool_End;

    // change the ID of Service
    ID_Service = 1;
    // change the Service name
    Name_Service = (char *)"Tool Service";
    // change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
Service_Tool::~Service_Tool()
{
}
/**
 * This start function will init some critical function
 */
void Service_Tool::Service_Tool_Start()
{
}

/**
 * Execute fuction of SNM app
 */
void Service_Tool::Service_Tool_Execute()
{
    if (atService_Tool.User_Mode == SER_USER_MODE_DEBUG)
    {
    }
}
void Service_Tool::Service_Tool_End() {}

/**
 *
 */
float Service_Tool::randomFloat(float minVal, float maxVal)
{
    return minVal + ((float)random() / (float)RAND_MAX) * (maxVal - minVal);
}

/**
 * @brief Split a path into parts
 * @param input: a long path to split
 * @param parts: array tt save data
 * @param maxParts : max of parts
 * @return the number pof paths
 */
int Service_Tool::splitPath(String input, String parts[], int maxParts)
{
    int count = 0;
    int start = 1; // skip the leading '/'
    int slashIndex;

    while (count < maxParts)
    {
        slashIndex = input.indexOf('/', start);
        if (slashIndex == -1)
        {
            // Last part
            parts[count++] = input.substring(start);
            break;
        }
        else
        {
            parts[count++] = input.substring(start, slashIndex);
            start = slashIndex + 1;
        }
    }

    return count; // return number of parts found
}
#endif
