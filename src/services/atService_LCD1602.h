#ifndef _Service_atService_LCD1602_
#define _Service_atService_LCD1602_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "Service.h"
#include <LiquidCrystal_I2C.h>
#include "../src/services/i2c/atService_I2C.h"
/* _____DEFINATIONS__________________________________________________________ */
// Text alignment options
enum LCD_Alignment
{
    LCD_ALIGN_LEFT,
    LCD_ALIGN_CENTER,
    LCD_ALIGN_RIGHT
};

// Line selection options
enum LCD_Line
{
    LCD_LINE_1 = 0,
    LCD_LINE_2 = 1,
    LCD_LINE_BOTH = 2
};

/* _____GLOBAL VARIABLES_____________________________________________________ */
// Khởi tạo LCD với địa chỉ I2C 0x27, 16 ký tự, 2 dòng
LiquidCrystal_I2C lcd(0x21, 16, 2);
/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */
/**
 * This Service class is the Service to manage the
 */
class Service_LCD1602 : public Service
{
public:
    Service_LCD1602();
    ~Service_LCD1602();

    // ==================== Basic Display Functions ====================
    /**
     * @brief Display text on LCD with alignment and auto-scroll for long text
     * @param text Text to display (auto-scroll if > 16 chars)
     * @param line Line selection: LCD_LINE_1, LCD_LINE_2, or LCD_LINE_BOTH
     * @param align Alignment: LCD_ALIGN_LEFT, LCD_ALIGN_CENTER, or LCD_ALIGN_RIGHT
     * @param autoScroll Enable auto-scroll for long text (default: true)
     * @param scrollSpeed Scroll speed in milliseconds (default: 300ms)
     * @note For LCD_LINE_BOTH, separate two lines with '|' character
     */
    void displayText(const String &text, LCD_Line line = LCD_LINE_1, LCD_Alignment align = LCD_ALIGN_LEFT,
                     bool autoScroll = true, uint16_t scrollSpeed = 300);

    /**
     * @brief Display two lines of text with individual alignment and auto-scroll
     * @param line1 Text for line 1 (auto-scroll if > 16 chars)
     * @param line2 Text for line 2 (auto-scroll if > 16 chars)
     * @param align1 Alignment for line 1
     * @param align2 Alignment for line 2
     * @param autoScroll Enable auto-scroll for long text (default: true)
     * @param scrollSpeed Scroll speed in milliseconds (default: 300ms)
     */
    void displayTwoLines(const String &line1, const String &line2,
                         LCD_Alignment align1 = LCD_ALIGN_LEFT,
                         LCD_Alignment align2 = LCD_ALIGN_LEFT,
                         bool autoScroll = true, uint16_t scrollSpeed = 300);
    void clearLine(LCD_Line line = LCD_LINE_BOTH);

    /**
     * @brief Display text at specific x, y coordinates
     * @param text Text to display
     * @param x Column position (0-15)
     * @param y Row position (0-1)
     * @note Text will be truncated if it exceeds LCD width
     */
    void displayTextAt(const String &text, uint8_t x, uint8_t y);

    // ==================== Scrolling Functions ====================
    /**
     * @brief Scroll text horizontally on one line (for text longer than 16 chars)
     * @param text Text to scroll
     * @param line Line number (LCD_LINE_1 or LCD_LINE_2)
     * @param speed Scroll speed in milliseconds (default: 300ms)
     */
    void scrollText(const String &text, LCD_Line line, uint16_t speed = 300);

    /**
     * @brief Display blinking text (on/off effect)
     * @param text Text to display
     * @param line Line selection
     * @param align Text alignment
     * @param blinkCount Number of blinks (default: 3)
     * @param blinkSpeed Blink speed in milliseconds (default: 500ms)
     */
    void blinkText(const String &text, LCD_Line line, LCD_Alignment align = LCD_ALIGN_CENTER,
                   uint8_t blinkCount = 3, uint16_t blinkSpeed = 500);

    // ==================== Special Effects ====================
    /**
     * @brief Display loading animation with custom message
     * @param message Message to display (e.g., "Loading")
     * @param dotCount Number of dots in animation (1-3)
     */
    void showLoading(const String &message = "Loading", uint8_t dotCount = 3);

    /**
     * @brief Display progress bar
     * @param progress Progress value (0-100)
     * @param line Line to display (LCD_LINE_1 or LCD_LINE_2)
     * @param showPercent Show percentage text (default: true)
     */
    void showProgressBar(uint8_t progress, LCD_Line line = LCD_LINE_2, bool showPercent = true);

    /**
     * @brief Turn backlight on/off
     * @param enable True to turn on, false to turn off
     */
    void setBacklight(bool enable);

protected:
private:
    static void Service_LCD1602_Start();
    static void Service_LCD1602_Execute();
    static void Service_LCD1602_End();
} atService_LCD1602;
/**
 * This function will be automaticaly called when a object is created by this class
 */
Service_LCD1602::Service_LCD1602(/* args */)
{
    _Start_User = *Service_LCD1602_Start;
    _Execute_User = *Service_LCD1602_Execute;
    _End_User = *Service_LCD1602_End;

    // change the ID of Service
    ID_Service = 1;
    // change the Service name
    Name_Service = (char *)"LCD1602 Service";
    // change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
Service_LCD1602::~Service_LCD1602()
{
}
/**
 * This start function will init some critical function
 */
void Service_LCD1602::Service_LCD1602_Start()
{
    atService_I2C.Run_Service();
    // Khởi động LCD
    atService_I2C.checkIn();
    lcd.init();
    atService_I2C.checkOut();
    atService_LCD1602.setBacklight(atObject_Param.lcd_backlight_enable);
}

/**
 * Execute fuction of SNM app
 */
void Service_LCD1602::Service_LCD1602_Execute()
{
    if (atService_LCD1602.User_Mode == SER_USER_MODE_DEBUG)
    {
    }
}

void Service_LCD1602::Service_LCD1602_End() {}

// ==================== Implementation of Display Functions ====================

/**
 * @brief Helper function to calculate cursor position for alignment
 */
uint8_t calculateCursorPosition(const String &text, LCD_Alignment align)
{
    uint8_t textLen = text.length();
    if (textLen > 16)
        textLen = 16; // LCD width limit

    switch (align)
    {
    case LCD_ALIGN_CENTER:
        return (16 - textLen) / 2;
    case LCD_ALIGN_RIGHT:
        return 16 - textLen;
    case LCD_ALIGN_LEFT:
    default:
        return 0;
    }
}

/**
 * @brief Display text on LCD with alignment and auto-scroll
 */
void Service_LCD1602::displayText(const String &text, LCD_Line line, LCD_Alignment align,
                                  bool autoScroll, uint16_t scrollSpeed)
{

    if (line == LCD_LINE_BOTH)
    {
        // Split text by '|' character
        int separatorPos = text.indexOf('|');
        if (separatorPos != -1)
        {
            String line1 = text.substring(0, separatorPos);
            String line2 = text.substring(separatorPos + 1);
            displayTwoLines(line1, line2, align, align, autoScroll, scrollSpeed);
        }
        else
        {
            // If no separator, display same text on both lines
            displayTwoLines(text, text, align, align, autoScroll, scrollSpeed);
        }
        return;
    }

    // Check if text is longer than 16 chars and auto-scroll is enabled
    if (autoScroll && text.length() > 16)
    {
        // Scroll the text
        atService_I2C.checkIn();

        // Add spaces at the end for smooth looping
        String scrollText = text + "   ";

        // Scroll from right to left
        for (int i = 0; i <= scrollText.length() - 16; i++)
        {
            lcd.setCursor(0, line);
            lcd.print(scrollText.substring(i, i + 16));
            atService_I2C.checkOut();
            vTaskDelay(scrollSpeed / portTICK_PERIOD_MS);
            atService_I2C.checkIn();
        }

        atService_I2C.checkOut();
    }
    else
    {
        // Normal display without scroll
        atService_I2C.checkIn();

        // Clear the line first
        lcd.setCursor(0, line);
        lcd.print("                "); // 16 spaces

        // Display text with alignment (truncate if > 16 chars)
        String displayStr = text;
        if (displayStr.length() > 16)
        {
            displayStr = displayStr.substring(0, 16);
        }

        uint8_t col = calculateCursorPosition(displayStr, align);
        lcd.setCursor(col, line);
        lcd.print(displayStr);

        atService_I2C.checkOut();
    }
}

/**
 * @brief Display two lines of text with individual alignment and auto-scroll
 */
void Service_LCD1602::displayTwoLines(const String &line1, const String &line2,
                                      LCD_Alignment align1, LCD_Alignment align2,
                                      bool autoScroll, uint16_t scrollSpeed)
{

    bool line1Long = line1.length() > 16;
    bool line2Long = line2.length() > 16;

    // If both lines are long and auto-scroll is enabled, scroll them together
    if (autoScroll && (line1Long || line2Long))
    {
        atService_I2C.checkIn();

        // Add spaces for smooth scrolling
        String scrollText1 = line1Long ? (line1 + "   ") : line1;
        String scrollText2 = line2Long ? (line2 + "   ") : line2;

        // Calculate max scroll length
        int maxLen = max(scrollText1.length(), scrollText2.length());
        int scrollLen = max(16, maxLen);

        // Pad shorter line with spaces if needed
        while (scrollText1.length() < scrollLen)
            scrollText1 += " ";
        while (scrollText2.length() < scrollLen)
            scrollText2 += " ";

        // Scroll both lines simultaneously
        for (int i = 0; i <= scrollLen - 16; i++)
        {
            // Display line 1
            lcd.setCursor(0, 0);
            if (line1Long)
            {
                lcd.print(scrollText1.substring(i, i + 16));
            }
            else
            {
                lcd.print("                "); // Clear
                uint8_t col1 = calculateCursorPosition(line1, align1);
                lcd.setCursor(col1, 0);
                lcd.print(line1.substring(0, 16));
            }

            // Display line 2
            lcd.setCursor(0, 1);
            if (line2Long)
            {
                lcd.print(scrollText2.substring(i, i + 16));
            }
            else
            {
                lcd.print("                "); // Clear
                uint8_t col2 = calculateCursorPosition(line2, align2);
                lcd.setCursor(col2, 1);
                lcd.print(line2.substring(0, 16));
            }

            atService_I2C.checkOut();
            vTaskDelay(scrollSpeed / portTICK_PERIOD_MS);
            atService_I2C.checkIn();
        }

        atService_I2C.checkOut();
    }
    else
    {
        // Normal display without scroll
        atService_I2C.checkIn();

        // Clear both lines
        lcd.setCursor(0, 0);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print("                ");

        // Display line 1 (truncate if > 16 chars)
        String displayStr1 = line1;
        if (displayStr1.length() > 16)
        {
            displayStr1 = displayStr1.substring(0, 16);
        }
        uint8_t col1 = calculateCursorPosition(displayStr1, align1);
        lcd.setCursor(col1, 0);
        lcd.print(displayStr1);

        // Display line 2 (truncate if > 16 chars)
        String displayStr2 = line2;
        if (displayStr2.length() > 16)
        {
            displayStr2 = displayStr2.substring(0, 16);
        }
        uint8_t col2 = calculateCursorPosition(displayStr2, align2);
        lcd.setCursor(col2, 1);
        lcd.print(displayStr2);

        atService_I2C.checkOut();
    }
}

/**
 * @brief Clear specific line or entire LCD
 */
void Service_LCD1602::clearLine(LCD_Line line)
{
    atService_I2C.checkIn();

    if (line == LCD_LINE_BOTH)
    {
        lcd.clear();
    }
    else
    {
        lcd.setCursor(0, line);
        lcd.print("                "); // 16 spaces
    }

    atService_I2C.checkOut();
}

/**
 * @brief Display text at specific x, y coordinates
 */
void Service_LCD1602::displayTextAt(const String &text, uint8_t x, uint8_t y)
{
    // Validate coordinates
    if (x >= 16 || y >= 2)
    {
        return; // Invalid coordinates
    }

    atService_I2C.checkIn();

    // Calculate maximum characters that can be displayed from position x
    uint8_t maxChars = 16 - x;
    String displayStr = text;
    
    // Truncate text if it exceeds LCD width
    if (displayStr.length() > maxChars)
    {
        displayStr = displayStr.substring(0, maxChars);
    }

    // Set cursor to specified position and display text
    lcd.setCursor(x, y);
    lcd.print(displayStr);

    atService_I2C.checkOut();
}

/**
 * @brief Scroll text horizontally on one line
 */
void Service_LCD1602::scrollText(const String &text, LCD_Line line, uint16_t speed)
{
    if (text.length() <= 16)
    {
        // No need to scroll if text fits
        displayText(text, line, LCD_ALIGN_LEFT);
        return;
    }

    atService_I2C.checkIn();

    // Scroll from right to left
    for (int i = 0; i <= text.length() - 16; i++)
    {
        lcd.setCursor(0, line);
        lcd.print(text.substring(i, i + 16));
        atService_I2C.checkOut();
        delay(speed);
        atService_I2C.checkIn();
    }

    atService_I2C.checkOut();
}

/**
 * @brief Display blinking text
 */
void Service_LCD1602::blinkText(const String &text, LCD_Line line, LCD_Alignment align,
                                uint8_t blinkCount, uint16_t blinkSpeed)
{
    for (uint8_t i = 0; i < blinkCount; i++)
    {
        displayText(text, line, align);
        delay(blinkSpeed);
        clearLine(line);
        delay(blinkSpeed / 2);
    }
    // Display final text
    displayText(text, line, align);
}

/**
 * @brief Display loading animation
 */
void Service_LCD1602::showLoading(const String &message, uint8_t dotCount)
{
    atService_I2C.checkIn();
    lcd.setCursor(0, 0);
    lcd.print("                ");

    uint8_t col = calculateCursorPosition(message, LCD_ALIGN_CENTER);
    lcd.setCursor(col, 0);
    lcd.print(message);

    // Animate dots on line 2
    lcd.setCursor(0, 1);
    lcd.print("                ");

    for (uint8_t i = 0; i < dotCount; i++)
    {
        lcd.setCursor(7 + i, 1);
        lcd.print(".");
        atService_I2C.checkOut();
        delay(300);
        atService_I2C.checkIn();
    }

    atService_I2C.checkOut();
}

/**
 * @brief Display progress bar
 */
void Service_LCD1602::showProgressBar(uint8_t progress, LCD_Line line, bool showPercent)
{
    if (progress > 100)
        progress = 100;

    atService_I2C.checkIn();

    lcd.setCursor(0, line);
    lcd.print("                ");
    lcd.setCursor(0, line);

    if (showPercent)
    {
        // Show [=====>    ] 50%
        uint8_t barWidth = 10;
        uint8_t filled = (progress * barWidth) / 100;

        lcd.print("[");
        for (uint8_t i = 0; i < barWidth; i++)
        {
            if (i < filled)
            {
                lcd.print("=");
            }
            else if (i == filled)
            {
                lcd.print(">");
            }
            else
            {
                lcd.print(" ");
            }
        }
        lcd.print("] ");
        lcd.print(progress);
        lcd.print("%");
    }
    else
    {
        // Full width progress bar
        uint8_t barWidth = 16;
        uint8_t filled = (progress * barWidth) / 100;

        for (uint8_t i = 0; i < barWidth; i++)
        {
            if (i < filled)
            {
                lcd.print((char)255); // Full block character
            }
            else
            {
                lcd.print(" ");
            }
        }
    }

    atService_I2C.checkOut();
}

/**
 * @brief Turn backlight on/off
 */
void Service_LCD1602::setBacklight(bool enable)
{
    atService_I2C.checkIn();
    if (enable)
    {
        lcd.backlight();
    }
    else
    {
        lcd.noBacklight();
    }
    atService_I2C.checkOut();
}

#endif
