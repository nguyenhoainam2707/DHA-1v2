#ifndef _Service_atService_DS3231_
#define _Service_atService_DS3231_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "Service.h"
#include "../src/services/i2c/atService_I2C.h"
/* _____DEFINATIONS__________________________________________________________ */
// I2C Addresses
#define DS3231_ADDR 0x68
// DS3231 Registers
#define DS3231_REG_TIME 0x00
#define DS3231_REG_TEMP 0x11
#define DS3231_REG_CONTROL 0x0E
#define DS3231_REG_STATUS 0x0F
// Time offset to compensate for upload delay (seconds)
#define TIME_OFFSET_SECONDS 18
/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */
/**
 * This Service class is the Service to manage the
 */
class Service_DS3231 : public Service
{
public:
    Service_DS3231();
    ~Service_DS3231();

    bool readDS3231Time(Object_Data &dataObj);
    bool readDS3231Temperature(Object_Data &dataObj);
    bool setDS3231Time(Object_Data &dataObj);
    void autoSetTimeFromCompile(Object_Data &dataObj);

protected:
private:
    uint8_t bcd2dec(uint8_t bcd);
    uint8_t dec2bcd(uint8_t dec);
    static void Service_DS3231_Start();
    static void Service_DS3231_Execute();
    static void Service_DS3231_End();
} atService_DS3231;
/**
 * This function will be automaticaly called when a object is created by this class
 */
Service_DS3231::Service_DS3231(/* args */)
{
    _Start_User = *Service_DS3231_Start;
    _Execute_User = *Service_DS3231_Execute;
    _End_User = *Service_DS3231_End;

    // change the ID of Service
    ID_Service = 1;
    // change the Service name
    Name_Service = (char *)"DS3231 Service";
    // change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
Service_DS3231::~Service_DS3231()
{
}

bool Service_DS3231::readDS3231Time(Object_Data &dataObj)
{
    atService_I2C.checkIn();
    Wire.beginTransmission(DS3231_ADDR);
    Wire.write(DS3231_REG_TIME);
    Wire.endTransmission();

    Wire.requestFrom(DS3231_ADDR, 7);

    if (Wire.available() >= 7)
    {
        dataObj.rtc_second = bcd2dec(Wire.read() & 0x7F);
        dataObj.rtc_minute = bcd2dec(Wire.read());
        dataObj.rtc_hour = bcd2dec(Wire.read() & 0x3F);
        dataObj.rtc_day_of_week = bcd2dec(Wire.read());
        dataObj.rtc_day = bcd2dec(Wire.read());
        dataObj.rtc_month = bcd2dec(Wire.read());
        dataObj.rtc_year = bcd2dec(Wire.read());
        atService_I2C.checkOut();
        if (atService_DS3231.User_Mode == SER_USER_MODE_DEBUG)
        {
            // Array of weekday names
            const char *daysOfWeek[] = {"Invalid", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
            Serial.println("\n=== DS3231 TIME ===");
            Serial.printf("Date: %02d/%02d/20%02d\n", dataObj.rtc_day, dataObj.rtc_month, dataObj.rtc_year);
            Serial.printf("Time: %02d:%02d:%02d\n", dataObj.rtc_hour, dataObj.rtc_minute, dataObj.rtc_second);
            if (dataObj.rtc_day_of_week >= 1 && dataObj.rtc_day_of_week <= 7)
            {
                Serial.printf("Day of week: %s (%d)\n", daysOfWeek[dataObj.rtc_day_of_week], dataObj.rtc_day_of_week);
            }
            else
            {
                Serial.printf("Day of week: Invalid (%d)\n", dataObj.rtc_day_of_week);
            }
        }
        return true;
    }
    else
    {
        atService_I2C.checkOut();
        if (atService_DS3231.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("Error: Cannot read time from DS3231");
        }
        return false;
    }
}

bool Service_DS3231::readDS3231Temperature(Object_Data &dataObj)
{
    atService_I2C.checkIn();
    Wire.beginTransmission(DS3231_ADDR);
    Wire.write(DS3231_REG_TEMP);
    Wire.endTransmission();

    Wire.requestFrom(DS3231_ADDR, 2);

    if (Wire.available() >= 2)
    {
        int8_t tempMSB = Wire.read();
        uint8_t tempLSB = Wire.read();
        atService_I2C.checkOut();
        dataObj.rtc_temperature = tempMSB + ((tempLSB >> 6) * 0.25);
        if (atService_DS3231.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("\n=== DS3231 TEMPERATURE ===");
            Serial.printf("Temperature: %.2f °C\n", dataObj.rtc_temperature);
        }
        return true;
    }
    else
    {
        atService_I2C.checkOut();
        if (atService_DS3231.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("Error: Cannot read temperature from DS3231");
        }
        return false;
    }
}

bool Service_DS3231::setDS3231Time(Object_Data &dataObj)
{
    atService_I2C.checkIn();
    Wire.beginTransmission(DS3231_ADDR);
    Wire.write(DS3231_REG_TIME);
    Wire.write(dec2bcd(dataObj.rtc_second));
    Wire.write(dec2bcd(dataObj.rtc_minute));
    Wire.write(dec2bcd(dataObj.rtc_hour));
    Wire.write(dec2bcd(dataObj.rtc_day_of_week));
    Wire.write(dec2bcd(dataObj.rtc_day));
    Wire.write(dec2bcd(dataObj.rtc_month));
    Wire.write(dec2bcd(dataObj.rtc_year));
    uint8_t status = Wire.endTransmission();
    atService_I2C.checkOut();

    bool success = (status == 0); // status = 0 means success

    if (atService_DS3231.User_Mode == SER_USER_MODE_DEBUG)
    {
        if (success)
        {
            Serial.println("\n=== DS3231 TIME SET ===");
            Serial.printf("Set time: %02d/%02d/20%02d %02d:%02d:%02d (DoW: %d)\n",
                          dataObj.rtc_day, dataObj.rtc_month, dataObj.rtc_year, dataObj.rtc_hour, dataObj.rtc_minute, dataObj.rtc_second, dataObj.rtc_day_of_week);
        }
        else
        {
            Serial.println("\n=== DS3231 TIME SET FAILED ===");
            Serial.printf("Error code: %d\n", status);
        }
    }

    return success;
}

// Automatically set compile time from laptop
void Service_DS3231::autoSetTimeFromCompile(Object_Data &dataObj)
{
    // Get compile time from __DATE__ and __TIME__ macros
    // __DATE__ format: "MMM DD YYYY" (e.g., "Oct 12 2025")
    // __TIME__ format: "HH:MM:SS" (e.g., "14:30:45")
    const char *date = __DATE__;
    const char *time = __TIME__;

    if (atService_DS3231.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("\n=== AUTO SET TIME FROM COMPILE ===");
        Serial.printf("Compile date: %s\n", date);
        Serial.printf("Compile time: %s\n", time);
    }

    // Parse month
    const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    uint8_t month = 0;
    for (uint8_t i = 0; i < 12; i++)
    {
        if (strncmp(date, months[i], 3) == 0)
        {
            month = i + 1;
            break;
        }
    }

    // Parse day
    uint8_t day = atoi(date + 4);

    // Parse year (only take last 2 digits, don't subtract 2000)
    int fullYear = atoi(date + 7);
    uint8_t year = fullYear % 100;

    // Parse time
    uint8_t hour = atoi(time);
    uint8_t minute = atoi(time + 3);
    uint8_t second = atoi(time + 6);

    // Upload time compensation
    second += TIME_OFFSET_SECONDS;
    if (second >= 60)
    {
        minute += second / 60;
        second %= 60;
    }
    if (minute >= 60)
    {
        hour += minute / 60;
        minute %= 60;
    }
    if (hour >= 24)
    {
        day += hour / 24;
        hour %= 24;
    }

    // Check number of days in month
    uint8_t daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    // Check leap year
    if (month == 2 && ((fullYear % 4 == 0 && fullYear % 100 != 0) || (fullYear % 400 == 0)))
    {
        daysInMonth[1] = 29;
    }
    if (day > daysInMonth[month - 1])
    {
        day -= daysInMonth[month - 1];
        month++;
        if (month > 12)
        {
            month = 1;
            year++;
        }
    }

    // Calculate day of week (Sakamoto's algorithm - simple and accurate)
    int y = fullYear;
    int m = month;
    int d = day;

    static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    y -= m < 3;
    int dayOfWeek = (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
    // Convert to DS3231 format (1=Sunday, 2=Monday, ..., 7=Saturday)
    dayOfWeek = (dayOfWeek == 0) ? 1 : dayOfWeek + 1;

    // Update dataObj again
    dataObj.rtc_year = year;
    dataObj.rtc_month = month;
    dataObj.rtc_day = day;
    dataObj.rtc_hour = hour;
    dataObj.rtc_minute = minute;
    dataObj.rtc_second = second;
    dataObj.rtc_day_of_week = dayOfWeek;

    if (atService_DS3231.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.printf("Parsed: 20%02d-%02d-%02d %02d:%02d:%02d (DoW: %d)\n",
                      year, month, day, hour, minute, second, dayOfWeek);
        Serial.println("✓ Time synchronized from compile timestamp!");
    }

    // Set to DS3231
    // setDS3231Time(year, month, day, hour, minute, second, dayOfWeek);
}

// Convert BCD to Decimal
uint8_t Service_DS3231::bcd2dec(uint8_t bcd)
{
    return ((bcd / 16) * 10) + (bcd % 16);
}

// Convert Decimal to BCD
uint8_t Service_DS3231::dec2bcd(uint8_t dec)
{
    return ((dec / 10) * 16) + (dec % 10);
}

/**
 * This start function will init some critical function
 */
void Service_DS3231::Service_DS3231_Start()
{
    atService_I2C.Run_Service();
}

/**
 * Execute fuction of SNM app
 */
void Service_DS3231::Service_DS3231_Execute()
{
    // if (atService_DS3231.User_Mode == SER_USER_MODE_DEBUG)
    // {
    // }
}
void Service_DS3231::Service_DS3231_End() {}

#endif
