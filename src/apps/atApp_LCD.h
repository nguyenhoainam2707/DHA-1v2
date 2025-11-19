#ifndef _Application_atApp_LCD_
#define _Application_atApp_LCD_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "App.h"
#include "../services/atService_LCD1602.h"
#include "../src/obj/atObj_Param.h"
#include "../src/obj/atObj_Data.h"
// #include "../src/apps/atApp_Database.h"
/* _____DEFINITIONS__________________________________________________________ */
#define MAX_COLUMN 2
#define MAX_ROW 16
#define MENU_ITEM_COUNT 5
#define MENU_TITLE_TIME_SHOW 3000 // 3 seconds to show menu title

/* _____GLOBAL VARIABLES_____________________________________________________ */
TaskHandle_t Task_atApp_LCD;
void atApp_LCD_Task_Func(void *parameter);

/* _____CLASS DEFINITION_____________________________________________________ */
class App_LCD : public Application
{
public:
	App_LCD();
	~App_LCD();

protected:
private:
	static void App_LCD_Pend();
	static void App_LCD_Start();
	static void App_LCD_Restart();
	static void App_LCD_Execute();
	static void App_LCD_Suspend();
	static void App_LCD_Resume();
	static void App_LCD_End();
	String lcd_line1_string_buffer = "1234567890123456";
	String lcd_line2_string_buffer = "ABCDEFGHIJKLMNOP";
	String old_string_line1_buffer = "";
	String old_string_line2_buffer = "";
	char lcd_buffer[MAX_COLUMN][MAX_ROW] = {
		{'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6'}, // Line 1
		{'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P'}  // Line 2
	};
	static void update_buffer();
	String Menu_Name[MENU_ITEM_COUNT] = {
		"Sensor Value",
		"FTP Status",
		"SIM Status",
		"RS85 Status",
		"RTC Time"};
	// design functions
	// Menu callback function type
	typedef void (*MenuCallback)();

	// Menu structure
	struct MenuItem
	{
		String name;
		MenuCallback callback;
	};

	static MenuItem menu_items[MENU_ITEM_COUNT];
	static uint8_t current_menu_index;
	static bool menu_mode_active;
	static uint32_t menu_timeout_start;
	static const uint32_t MENU_TIMEOUT_MS = 15000; // 15 seconds timeout

	// Add new variables for menu title display
	static bool menu_title_showing;
	static uint32_t menu_title_start_time;
	static uint8_t menu_title_index;

	// Menu functions
	static uint8_t create_menu(String menu_name, MenuCallback callback_function);
	static void display_current_menu();
	static void display_menu_title(uint8_t menu_index);
	static void show_menu_content(uint8_t menu_index);
	static void navigate_menu(bool next); // true = next, false = previous
	static void execute_current_menu();
	static void exit_menu_mode();
	static bool is_menu_timeout();
	static bool is_menu_title_timeout();

	// Example callback functions
	static void menu_sensor_callback();
	static void menu_ftp_callback();
	static void menu_sim_callback();
	static void menu_rs485_callback();
	static void menu_rtc_callback();

	// Add these variables for button press counting
	static uint8_t last_button_press_count;
	static bool menu_navigation_by_count_enabled;

	// New functions for button press navigation
	static void handle_button_press_navigation();
	static void set_button_count_navigation(bool enabled);

	// Normal display variables for sensor cycling
	static uint8_t sensor_display_index;
	static uint32_t sensor_display_start_time;
	static const uint32_t SENSOR_DISPLAY_TIME = 2000; // 2 seconds per sensor
	static bool sensor_cycling_enabled;

	// Variables for cycle mode notification
	static bool show_cycle_notification;
	static uint32_t cycle_notification_start_time;
	static String cycle_notification_message;
	static const uint32_t CYCLE_NOTIFICATION_TIME = 2000; // 2 seconds notification

	// Normal display functions
	static void update_sensor_normal_display(bool show_enabled_sensor_only);
	static void show_specific_sensor(uint8_t sensor_index);
	static void show_next_sensor();
	static void show_next_sensor_enabled(); // Only show enabled sensors
	static bool is_sensor_display_timeout();
	static void set_sensor_cycling(bool enabled);
	static bool has_enabled_sensors(); // Check if any sensor is enabled

	// Cycle notification functions
	static void show_cycle_notification_message(String message);
	static bool is_cycle_notification_timeout();
	// Check button states for menu navigation and execution
	bool enter_menu = false;
	bool execute_menu = false;
	
	bool enter_cycle_sensor = false;
	bool cycle_sensor_active = false;
	bool cycle_sensor = false;

	bool enter_cycle_enabled_sensor = true;
	bool cycle_enabled_sensor_active = false;
	bool cycle_enabled_sensor = false;

	bool enter_wifi_config = false;
} atApp_LCD;

// Initialize static variables
App_LCD::MenuItem App_LCD::menu_items[MENU_ITEM_COUNT];

uint8_t App_LCD::current_menu_index = 0;

bool App_LCD::menu_mode_active = false;

uint32_t App_LCD::menu_timeout_start = 0;

uint8_t App_LCD::last_button_press_count = 0;

bool App_LCD::menu_navigation_by_count_enabled = true;

bool App_LCD::menu_title_showing = false;

uint32_t App_LCD::menu_title_start_time = 0;

uint8_t App_LCD::menu_title_index = 0;

// Initialize sensor display static variables
uint8_t App_LCD::sensor_display_index = 0;

uint32_t App_LCD::sensor_display_start_time = 0;

bool App_LCD::sensor_cycling_enabled = true;

// Initialize cycle notification static variables
bool App_LCD::show_cycle_notification = false;

uint32_t App_LCD::cycle_notification_start_time = 0;

String App_LCD::cycle_notification_message = "";

/**
 * Create menu item with name and callback function
 * @param menu_name: Name of the menu item
 * @param callback_function: Function to call when menu is selected
 * @return: Index of created menu item, or 255 if failed
 */
uint8_t App_LCD::create_menu(String menu_name, MenuCallback callback_function)
{
	// Find first available slot
	for (uint8_t i = 0; i < MENU_ITEM_COUNT; i++)
	{
		if (menu_items[i].name == "" || menu_items[i].name.length() == 0)
		{
			menu_items[i].name = menu_name;
			menu_items[i].callback = callback_function;

			if (atApp_LCD.User_Mode == APP_USER_MODE_DEBUG)
			{
				Serial.printf("Menu created [%d]: %s\n", i, menu_name.c_str());
			}

			return i;
		}
	}

	// No available slot
	if (atApp_LCD.User_Mode == APP_USER_MODE_DEBUG)
	{
		Serial.println("Error: No available menu slot");
	}
	return 255;
}

/**
 * Display menu title for specified time
 * @param menu_index: Index of menu to show title for
 */
void App_LCD::display_menu_title(uint8_t menu_index)
{
	if (menu_index >= MENU_ITEM_COUNT)
		return;

	menu_title_showing = true;
	menu_title_start_time = millis();
	menu_title_index = menu_index;

	// Display menu title
	String line1 = ">> MENU " + String(menu_index + 1) + " <<";
	String line2 = menu_items[menu_index].name;

	// Center align and truncate if needed
	if (line1.length() > MAX_ROW)
	{
		line1 = line1.substring(0, MAX_ROW);
	}
	if (line2.length() > MAX_ROW)
	{
		line2 = line2.substring(0, MAX_ROW);
	}

	atApp_LCD.lcd_line1_string_buffer = line1;
	atApp_LCD.lcd_line2_string_buffer = line2;

	if (atApp_LCD.User_Mode == APP_USER_MODE_DEBUG)
	{
		Serial.printf("Showing menu title [%d]: %s\n", menu_index, line2.c_str());
	}
}

/**
 * Show menu content by calling its callback function
 * @param menu_index: Index of menu to show content for
 */
void App_LCD::show_menu_content(uint8_t menu_index)
{
	if (menu_index >= MENU_ITEM_COUNT)
		return;

	if (menu_items[menu_index].callback != nullptr)
	{
		if (atApp_LCD.User_Mode == APP_USER_MODE_DEBUG)
		{
			Serial.printf("Showing menu content [%d]: %s\n", menu_index, menu_items[menu_index].name.c_str());
		}

		// Call the callback function to set content
		menu_items[menu_index].callback();

		// Don't exit menu mode when showing content, just stop showing title
		menu_title_showing = false;
		menu_mode_active = true; // Keep in menu mode
	}
}

/**
 * Check if menu title display time has expired
 */
bool App_LCD::is_menu_title_timeout()
{
	return menu_title_showing && (millis() - menu_title_start_time > MENU_TITLE_TIME_SHOW);
}

/**
 * Display current menu on LCD
 */
void App_LCD::display_current_menu()
{
	if (!menu_mode_active || current_menu_index >= MENU_ITEM_COUNT)
		return;

	// If we're not showing title, start showing it
	if (!menu_title_showing)
	{
		display_menu_title(current_menu_index);
	}
	else
	{
		// Update title display (in case of navigation during title show)
		if (menu_title_index != current_menu_index)
		{
			display_menu_title(current_menu_index);
		}
	}
}

/**
 * Navigate through menu items
 * @param next: true for next item, false for previous
 */
void App_LCD::navigate_menu(bool next)
{
	if (!menu_mode_active)
		return;

	if (next)
	{
		current_menu_index = (current_menu_index + 1) % MENU_ITEM_COUNT;
	}
	else
	{
		current_menu_index = (current_menu_index == 0) ? (MENU_ITEM_COUNT - 1) : (current_menu_index - 1);
	}

	// Reset timeout when navigating
	menu_timeout_start = millis();

	// Show title for new menu
	display_menu_title(current_menu_index);

	if (atApp_LCD.User_Mode == APP_USER_MODE_DEBUG)
	{
		Serial.printf("Menu navigate to index: %d\n", current_menu_index);
	}
}

/**
 * Handle menu navigation based on buttonPressedTimeNumber
 */
void App_LCD::handle_button_press_navigation()
{
	// Check if button press count has changed
	if (atObject_Data.buttonPressedTimeNumber != last_button_press_count)
	{
		uint8_t press_difference = atObject_Data.buttonPressedTimeNumber - last_button_press_count;

		// Only process if there's an increase and we're in menu mode
		if (press_difference > 0 && menu_mode_active)
		{
			// Navigate menu by the number of button presses
			for (uint8_t i = 0; i < press_difference; i++)
			{
				navigate_menu(true); // Move to next menu
			}

			// Reset timeout when navigating
			menu_timeout_start = millis();

			if (atApp_LCD.User_Mode == APP_USER_MODE_DEBUG)
			{
				Serial.printf("Menu navigation by button count: +%d steps, current index: %d\n",
							  press_difference, current_menu_index);
			}
		}
		else if (press_difference > 0 && !menu_mode_active)
		{
			// If not in menu mode but button pressed, enter menu mode
			menu_mode_active = true;
			menu_timeout_start = millis();
			current_menu_index = 0;

			// Navigate by button press count - 1 (since we start at index 0)
			for (uint8_t i = 1; i < press_difference; i++)
			{
				navigate_menu(true);
			}

			// Show title for current menu
			display_menu_title(current_menu_index);

			if (atApp_LCD.User_Mode == APP_USER_MODE_DEBUG)
			{
				Serial.printf("Entered menu mode with %d button presses, index: %d\n",
							  press_difference, current_menu_index);
			}
		}

		// Update last count
		last_button_press_count = atObject_Data.buttonPressedTimeNumber;
	}
}

/**
 * Execute current menu callback function
 */
void App_LCD::execute_current_menu()
{
	if (!menu_mode_active || current_menu_index >= MENU_ITEM_COUNT)
		return;

	// Force show content immediately when executed
	show_menu_content(current_menu_index);
}

/**
 * Check if menu has timed out
 */
bool App_LCD::is_menu_timeout()
{
	return menu_mode_active && (millis() - menu_timeout_start > MENU_TIMEOUT_MS);
}

// Example callback functions
void App_LCD::menu_sensor_callback()
{
	// Display all sensor and cycling through them by pressing button
	for (size_t index = 0; index < MAX_SENSORS; index++)
	{
	}
}

void App_LCD::menu_ftp_callback()
{
	atApp_LCD.lcd_line1_string_buffer = "FTP Status";
	atApp_LCD.lcd_line2_string_buffer = atObject_Data.ftpConnected ? "Connected" : "Disconnected";
	// Don't exit menu mode - let timeout handle it
}

void App_LCD::menu_sim_callback()
{
	atApp_LCD.lcd_line1_string_buffer = "SIM Status";
	atApp_LCD.lcd_line2_string_buffer = atObject_Data.simReady ? "Ready" : "Not Ready";
	// Don't exit menu mode - let timeout handle it
}

void App_LCD::menu_rs485_callback()
{
	atApp_LCD.lcd_line1_string_buffer = "RS485 Status";
	atApp_LCD.lcd_line2_string_buffer = "CH1:" + String(atObject_Data.rs485_response_list[0][0]);
	// Don't exit menu mode - let timeout handle it
}

void App_LCD::menu_rtc_callback()
{
	atApp_LCD.lcd_line1_string_buffer = "RTC Time";
	atApp_LCD.lcd_line2_string_buffer = String(atObject_Data.rtc_day) + "/" +
										String(atObject_Data.rtc_month) + "/" +
										String(atObject_Data.rtc_year) + " " +
										String(atObject_Data.rtc_hour) + ":" +
										String(atObject_Data.rtc_minute) + ":" +
										String(atObject_Data.rtc_second);
	// Don't exit menu mode - let timeout handle it
}

/**
 * This function will be automaticaly called when a object is created by this class
 */
App_LCD::App_LCD(/* args */)
{
	_Pend_User = *App_LCD_Pend;
	_Start_User = *App_LCD_Start;
	_Restart_User = *App_LCD_Restart;
	_Execute_User = *App_LCD_Execute;
	_Suspend_User = *App_LCD_Suspend;
	_Resume_User = *App_LCD_Resume;
	_End_User = *App_LCD_End;

	// change the ID of application
	ID_Application = 1;
	// change the application name
	Name_Application = (char *)"LCD Application	";
	// change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
App_LCD::~App_LCD()
{
}
/**
 * Pend to start is the first task of this application it will do prerequisite condition. In the debit mode, task will send information of application to terminal to start the application.
 */
void App_LCD::App_LCD_Pend()
{
	// atService_LCD1602.Debug();
}
/**
 * This start function will init some critical function
 */
void App_LCD::App_LCD_Start()
{
	// init atLCD1602 Service in the fist running time
	if (atApp_LCD.User_Mode == APP_USER_MODE_DEBUG)
	{
		atService_LCD1602.Debug();
	}
	atService_LCD1602.Run_Service();
	atService_LCD1602.displayTwoLines("DHA-1", "Initializing...", LCD_ALIGN_CENTER, LCD_ALIGN_CENTER, true, 250);

	// atService_LCD1602.displayTextAt(atApp_LCD.lcd_buffer[0], 0, 0);
	// atService_LCD1602.displayTextAt(atApp_LCD.lcd_buffer[1], 0, 1);
	vTaskDelay(1500 / portTICK_PERIOD_MS);

	// Initialize menu items
	create_menu("Sensor Value", menu_sensor_callback);
	create_menu("FTP Status", menu_ftp_callback);
	create_menu("SIM Status", menu_sim_callback);
	create_menu("RS485 Status", menu_rs485_callback);
	create_menu("RTC Time", menu_rtc_callback);

	// Initialize sensor display
	sensor_display_index = 0;
	sensor_display_start_time = millis();
	sensor_cycling_enabled = true;

	// Find first enabled sensor to start with
	bool found_enabled_sensor = false;
	for (uint8_t i = 0; i < MAX_SENSORS; i++)
	{
		if (atObject_Param.sensor_enable_list[i])
		{
			sensor_display_index = i;
			found_enabled_sensor = true;
			break;
		}
	}

	if (atApp_LCD.User_Mode == APP_USER_MODE_DEBUG)
	{
		if (found_enabled_sensor)
		{
			Serial.printf("Sensor normal display initialized, starting with sensor %d\n", sensor_display_index + 1);
		}
		else
		{
			Serial.println("Warning: No enabled sensors found during initialization");
		}
	}
}
/**
 * Restart function of SNM  app
 */
void App_LCD::App_LCD_Restart()
{
}
/**
 * Execute function of LCD app
 */
void App_LCD::App_LCD_Execute()
{
	// Check if lcd is not enabled, skip
	if (!atObject_Param.lcd_enable)
	{
		atService_LCD1602.clearLine(LCD_LINE_BOTH);
		atService_LCD1602.setBacklight(false);
		return;
	}

	// Check for menu timeout
	if (is_menu_timeout())
	{
		exit_menu_mode();
		// Turn on cycle enabled sensor mode after exiting menu
		atApp_LCD.enter_cycle_enabled_sensor = true;
	}

	// Check for menu title timeout
	if (is_menu_title_timeout())
	{
		show_menu_content(menu_title_index);
	}

	

	

	// Check button held states
	// Handle menu actions
	if (false){}
	else if (atObject_Data.flagButtonHeldSecond[12]) // 7 seconds to enter menu
	{
		atApp_LCD.enter_wifi_config = true;
		atApp_LCD.enter_menu = false;

		atApp_LCD.enter_cycle_enabled_sensor = false;
		atApp_LCD.cycle_enabled_sensor_active = false; // Reset active state
		atApp_LCD.cycle_enabled_sensor = false;
		
		atApp_LCD.enter_cycle_sensor = false;
		atApp_LCD.cycle_sensor_active = false; // Reset active state
		atApp_LCD.cycle_sensor = false;

		Serial.println("---> Enter Wifi Config");
		atApp_LCD.lcd_line1_string_buffer = "Enter Wifi Config";
		atApp_LCD.lcd_line2_string_buffer = "192.168.4.1";
	}
	else if (atObject_Data.flagButtonHeldSecond[9] && !menu_mode_active) // 7 seconds to enter menu
	{
		atApp_LCD.enter_menu = true;

		atApp_LCD.enter_cycle_enabled_sensor = false;
		atApp_LCD.cycle_enabled_sensor_active = false; // Reset active state
		atApp_LCD.cycle_enabled_sensor = false;
		
		atApp_LCD.enter_cycle_sensor = false;
		atApp_LCD.cycle_sensor_active = false; // Reset active state
		atApp_LCD.cycle_sensor = false;

		Serial.println("---> Enter Menu Triggered");
		atApp_LCD.lcd_line2_string_buffer = "Enter Menu Triggered";
	}
	else if (atObject_Data.flagButtonHeldSecond[6]) // 5 second to cycle enabled sensor only
	{
		atApp_LCD.enter_cycle_enabled_sensor = true;
		atApp_LCD.cycle_enabled_sensor_active = false; // Reset active state
		atApp_LCD.cycle_enabled_sensor = false;
		
		atApp_LCD.enter_cycle_sensor = false;
		atApp_LCD.cycle_sensor_active = false; // Reset active state
		atApp_LCD.cycle_sensor = false;
		atApp_LCD.enter_menu = false;

		Serial.println("---> Cycle Enabled Sensor Triggered");
		atApp_LCD.lcd_line1_string_buffer = "Change Mode";
		atApp_LCD.lcd_line2_string_buffer = "Cy-En-Sen";
	}
	else if (atObject_Data.flagButtonHeldSecond[3]) // Short press to cycle all sensors
	{
		atApp_LCD.enter_cycle_sensor = true;
		atApp_LCD.cycle_sensor_active = false; // Reset active state
		atApp_LCD.cycle_sensor = false;

		atApp_LCD.enter_cycle_enabled_sensor = false;
		atApp_LCD.cycle_enabled_sensor_active = false; // Reset active state
		atApp_LCD.cycle_enabled_sensor = false;
		atApp_LCD.enter_menu = false;

		Serial.println("---> Cycle Sensor Triggered");
		atApp_LCD.lcd_line1_string_buffer = "Change Mode";
		atApp_LCD.lcd_line2_string_buffer = "Cy-Sen";
	}
	
	// --------------------------------- Cycle Enabled Sensor Mode ---------------------------------
	if (false){}
	else if (atApp_LCD.enter_wifi_config)
	{
		// Stay in wifi config display until reset
		atApp_LCD.lcd_line1_string_buffer = "Wifi: DHA-1";
		atApp_LCD.lcd_line2_string_buffer = "192.168.4.1";
	}
	else if (atApp_LCD.enter_cycle_enabled_sensor && !atApp_LCD.cycle_enabled_sensor_active)
	{
		// Manually cycle to next enabled sensor only
		// Serial.println("---> Cycle Enabled Sensor Triggered");
		cycle_notification_start_time = millis();
		atApp_LCD.cycle_enabled_sensor_active = true;
		show_cycle_notification_message("Cy-En-Sen");
		// show_next_sensor_enabled();
	}
	else if (atApp_LCD.enter_cycle_enabled_sensor && atApp_LCD.cycle_enabled_sensor_active)
	{
		// check for cycle notification timeout
		if (is_cycle_notification_timeout())
		{
			show_cycle_notification = false;
		}
	}
	else if (atApp_LCD.cycle_enabled_sensor && atApp_LCD.cycle_enabled_sensor_active)
	{
		// Normal display mode - Show cycling sensors
		// Serial.println("---> Normal Display: Cycle Enabled Sensor");
		update_sensor_normal_display(true);
	}
	// --------------------------------- Cycle All Sensor Mode ---------------------------------
	else if (atApp_LCD.enter_cycle_sensor && !atApp_LCD.cycle_sensor_active)
	{
		// Manually cycle to next sensor (including disabled)
		// Serial.println("---> Cycle Sensor Triggered");
		cycle_notification_start_time = millis();
		atApp_LCD.cycle_sensor_active = true;
		show_cycle_notification_message("Cy-Sen");
		// show_next_sensor();
	}
	else if (atApp_LCD.enter_cycle_sensor && atApp_LCD.cycle_sensor_active)
	{
		// check for cycle notification timeout
		if (is_cycle_notification_timeout())
		{
			show_cycle_notification = false;
		}
	}
	else if (atApp_LCD.cycle_sensor && atApp_LCD.cycle_sensor_active)
	{
		// Normal display mode - Show cycling sensors
		// Serial.println("---> Normal Display: Cycle Sensor");
		update_sensor_normal_display(false);
	}
	// --------------------------------- Menu Mode ---------------------------------
	else if (atApp_LCD.enter_menu && !menu_mode_active)
	{
		// Show menu to enter menu mode
		// Serial.println("---> Enter Menu Triggered");
		menu_mode_active = true;
		menu_timeout_start = millis();
		current_menu_index = 0;
		last_button_press_count = atObject_Data.buttonPressedTimeNumber; // Reset counter
		display_menu_title(current_menu_index);
	}
	else if (atApp_LCD.execute_menu)
	{
		// Execute current menu action
		// Serial.println("---> Execute Menu Triggered");
		execute_current_menu();
	}
	
	else if (menu_mode_active && !menu_title_showing)
	{

		// If in menu mode but not showing title, we're showing content
		// Content is already set by callback, just maintain it
		// Serial.println("---> Maintaining Menu Content Display");
	}
	else if (menu_mode_active && menu_title_showing)
	{
		// Serial.println("---> Maintaining Menu Title Display");
		// Display current menu title
		handle_button_press_navigation();
		display_current_menu();
	}
	
	
	

	// Update buffer to LCD if there is any change
	atApp_LCD.update_buffer();

	// show debug info
	if (atApp_LCD.User_Mode == APP_USER_MODE_DEBUG)
	{
		// cycle_enabled_sensor, cycle_enabled_sensor
		Serial.printf("Cycle Enabled Sensor: %d\n", atApp_LCD.cycle_enabled_sensor);
		Serial.printf("Enter Cycle Enabled Sensor: %d\n", atApp_LCD.enter_cycle_enabled_sensor);
		Serial.printf("Enter Cycle Enabled Sensor Active: %d\n", atApp_LCD.cycle_enabled_sensor_active);
		
		Serial.printf("Cycle Sensor: %d\n", atApp_LCD.cycle_sensor);
		Serial.printf("Enter Cycle Sensor: %d\n", atApp_LCD.enter_cycle_sensor);
		Serial.printf("Enter Cycle Sensor Active: %d\n", atApp_LCD.cycle_sensor_active);

		Serial.printf("Enter Menu: %d\n", atApp_LCD.enter_menu);
		Serial.printf("Execute Menu: %d\n", atApp_LCD.execute_menu);
		Serial.printf("Menu Mode Active: %d\n", menu_mode_active);
		Serial.printf("Menu Title Showing: %d\n", menu_title_showing);
		Serial.printf("Current Menu Index: %d\n", current_menu_index);
		Serial.printf("Last Button Press Count: %d\n", last_button_press_count);
	}
}

/**
 * Exit menu mode and return to normal display
 */
void App_LCD::exit_menu_mode()
{
	menu_mode_active = false;
	menu_title_showing = false;
	current_menu_index = 0;
	last_button_press_count = atObject_Data.buttonPressedTimeNumber; // Sync counter

	if (atApp_LCD.User_Mode == APP_USER_MODE_DEBUG)
	{
		Serial.println("Exiting menu mode");
	}
}

void App_LCD::App_LCD_Suspend() {}
void App_LCD::App_LCD_Resume() {}
void App_LCD::App_LCD_End() {}
void App_LCD::update_buffer()
{
	// Split line 1
	for (int i = 0; i < MAX_ROW; i++)
	{
		if (i < atApp_LCD.lcd_line1_string_buffer.length())
		{
			// compare and assign char by char
			if (atApp_LCD.lcd_buffer[0][i] != atApp_LCD.lcd_line1_string_buffer.charAt(i))
			{
				atApp_LCD.lcd_buffer[0][i] = atApp_LCD.lcd_line1_string_buffer.charAt(i);
				atService_LCD1602.displayTextAt(String(atApp_LCD.lcd_buffer[0][i]), i, 0);
			}
		}
		else
		{
			if (atApp_LCD.lcd_buffer[0][i] != ' ')
			{
				atApp_LCD.lcd_buffer[0][i] = ' ';
				atService_LCD1602.displayTextAt(String(atApp_LCD.lcd_buffer[0][i]), i, 0);
			}
		}
	}
	// Split line 2
	for (int i = 0; i < MAX_ROW; i++)
	{
		if (i < atApp_LCD.lcd_line2_string_buffer.length())
		{
			// compare and assign char by char
			if (atApp_LCD.lcd_buffer[1][i] != atApp_LCD.lcd_line2_string_buffer.charAt(i))
			{
				atApp_LCD.lcd_buffer[1][i] = atApp_LCD.lcd_line2_string_buffer.charAt(i);
				atService_LCD1602.displayTextAt(String(atApp_LCD.lcd_buffer[1][i]), i, 1);
			}
		}
		else
		{
			if (atApp_LCD.lcd_buffer[1][i] != ' ')
			{
				atApp_LCD.lcd_buffer[1][i] = ' ';
				atService_LCD1602.displayTextAt(String(atApp_LCD.lcd_buffer[1][i]), i, 1);
			}
		}
	}
}

/**
 * Update normal display to show current sensor value
 */
void App_LCD::update_sensor_normal_display(bool show_enabled_sensor_only)
{
	// Check if showing cycle notification
	if (show_cycle_notification)
	{
		// Check if notification time has expired
		if (is_cycle_notification_timeout())
		{
			show_cycle_notification = false;
		}
		else
		{
			// Show notification message
			atApp_LCD.lcd_line1_string_buffer = "Cycle Mode";
			atApp_LCD.lcd_line2_string_buffer = cycle_notification_message;
			return;
		}
	}

	// Check if any sensor is enabled
	if (!has_enabled_sensors())
	{
		// Display warning message
		atApp_LCD.lcd_line1_string_buffer = "No sensor enable";
		atApp_LCD.lcd_line2_string_buffer = "Please config";
		return;
	}

	// Check if we need to cycle to next sensor
	if (sensor_cycling_enabled && is_sensor_display_timeout())
	{
		if (show_enabled_sensor_only)
			show_next_sensor_enabled();
		else
			show_next_sensor();
	}

	// Display current sensor
	if (sensor_display_index < MAX_SENSORS)
	{
		atApp_LCD.lcd_line1_string_buffer = String(sensor_display_index + 1) +
											":" +
											atObject_Param.sensor_name_list[sensor_display_index];

		// fill " " into atApp_LCD.lcd_line1_string_buffer to make it MAX_ROW length
		if (atApp_LCD.lcd_line1_string_buffer.length() < MAX_ROW - 4)
		{
			int spaces_to_add = MAX_ROW - atApp_LCD.lcd_line1_string_buffer.length();
			for (int i = 0; i < spaces_to_add; i++)
			{
				atApp_LCD.lcd_line1_string_buffer += " ";
			}
		}
		else if (atApp_LCD.lcd_line1_string_buffer.length() > MAX_ROW - 4)
		{
			atApp_LCD.lcd_line1_string_buffer = atApp_LCD.lcd_line1_string_buffer.substring(0, MAX_ROW - 4) + "..";
		}

		atApp_LCD.lcd_line1_string_buffer = atApp_LCD.lcd_line1_string_buffer.substring(0, MAX_ROW - 2);

		// Add status code at the end of line 1
		uint8_t status = atObject_Data.sensor_status_list[sensor_display_index];
		switch (status)
		{
		case SENSOR_STATE_NORMAL:
			atApp_LCD.lcd_line1_string_buffer += "00";
			break;
		case SENSOR_STATE_CALIB:
			atApp_LCD.lcd_line1_string_buffer += "01";
			break;
		case SENSOR_STATE_ERROR:
			atApp_LCD.lcd_line1_string_buffer += "02";
			break;
		default:
			atApp_LCD.lcd_line1_string_buffer += "??";
			break;
		}

		// Write sensor value to line 2
		atApp_LCD.lcd_line2_string_buffer = String(atObject_Data.sensor_raw_value_list[sensor_display_index]);
		// and write unit into the line 2 from right to left
		atApp_LCD.lcd_line2_string_buffer = atApp_LCD.lcd_line2_string_buffer.substring(0, MAX_ROW - atObject_Param.sensor_unit_list[sensor_display_index].length() - 1) +
											" " + atObject_Param.sensor_unit_list[sensor_display_index];
	}
	else
	{
		// Fallback display
		atApp_LCD.lcd_line1_string_buffer = "Sensor Display";
		atApp_LCD.lcd_line2_string_buffer = "No Data";
	}
}

/**
 * Move to next sensor in the cycle
 */
void App_LCD::show_next_sensor()
{
	sensor_display_index = (sensor_display_index + 1) % MAX_SENSORS;
	sensor_display_start_time = millis();

	if (atApp_LCD.User_Mode == APP_USER_MODE_DEBUG)
	{
		Serial.printf("Showing sensor %d: %.2f\n",
					  sensor_display_index + 1,
					  atObject_Data.sensor_raw_value_list[sensor_display_index]);
	}
}

/**
 * Move to next enabled sensor in the cycle (skip disabled sensors)
 */
void App_LCD::show_next_sensor_enabled()
{
	// Check if any sensor is enabled first
	if (!has_enabled_sensors())
	{
		if (atApp_LCD.User_Mode == APP_USER_MODE_DEBUG)
		{
			Serial.println("No enabled sensors found");
		}
		return;
	}

	uint8_t start_index = sensor_display_index;
	uint8_t attempts = 0;

	do
	{
		sensor_display_index = (sensor_display_index + 1) % MAX_SENSORS;
		attempts++;

		// Check if this sensor is enabled
		if (atObject_Param.sensor_enable_list[sensor_display_index])
		{
			sensor_display_start_time = millis();

			if (atApp_LCD.User_Mode == APP_USER_MODE_DEBUG)
			{
				Serial.printf("Showing next enabled sensor %d: %.2f\n",
							  sensor_display_index + 1,
							  atObject_Data.sensor_calib_value_list[sensor_display_index]);
			}
			return;
		}

		// Prevent infinite loop if no sensors are enabled
		if (attempts >= MAX_SENSORS)
		{
			if (atApp_LCD.User_Mode == APP_USER_MODE_DEBUG)
			{
				Serial.println("Warning: No enabled sensors found in loop");
			}
			return;
		}

	} while (sensor_display_index != start_index);
}

/**
 * Check if current sensor display time has expired
 */
bool App_LCD::is_sensor_display_timeout()
{
	return (millis() - sensor_display_start_time > SENSOR_DISPLAY_TIME);
}

/**
 * Enable or disable sensor cycling
 */
void App_LCD::set_sensor_cycling(bool enabled)
{
	sensor_cycling_enabled = enabled;

	if (enabled)
	{
		sensor_display_start_time = millis();
	}

	if (atApp_LCD.User_Mode == APP_USER_MODE_DEBUG)
	{
		Serial.printf("Sensor cycling: %s\n", enabled ? "Enabled" : "Disabled");
	}
}

/**
 * Show specific sensor by index
 * @param sensor_index: Index of sensor to show (0-9)
 */
void App_LCD::show_specific_sensor(uint8_t sensor_index)
{
	if (sensor_index < MAX_SENSORS)
	{
		sensor_display_index = sensor_index;
		sensor_display_start_time = millis();

		if (atApp_LCD.User_Mode == APP_USER_MODE_DEBUG)
		{
			Serial.printf("Manually showing sensor %d\n", sensor_index + 1);
		}
	}
}

/**
 * Show cycle mode notification message
 * @param message: The message to show
 */
void App_LCD::show_cycle_notification_message(String message)
{
	show_cycle_notification = true;
	cycle_notification_start_time = millis();
	cycle_notification_message = message;

	atApp_LCD.lcd_line1_string_buffer = "Cycle Mode";
	atApp_LCD.lcd_line2_string_buffer = message;

	if (atApp_LCD.User_Mode == APP_USER_MODE_DEBUG)
	{
		Serial.printf("Showing cycle notification: %s\n", message.c_str());
	}
}

/**
 * Check if cycle notification time has expired
 */
bool App_LCD::is_cycle_notification_timeout()
{
	if (millis() - cycle_notification_start_time > CYCLE_NOTIFICATION_TIME)
	{
		if (atApp_LCD.enter_cycle_sensor)
		{
			atApp_LCD.enter_cycle_sensor = false;
			atApp_LCD.cycle_sensor = true;
		}
		if( atApp_LCD.enter_cycle_enabled_sensor)
		{
			atApp_LCD.enter_cycle_enabled_sensor = false;
			atApp_LCD.cycle_enabled_sensor = true;
		}
		return true;
	}
	return false;
}

/**
 * Check if any sensor is enabled
 * @return: true if at least one sensor is enabled, false otherwise
 */
bool App_LCD::has_enabled_sensors()
{
	for (uint8_t i = 0; i < MAX_SENSORS; i++)
	{
		if (atObject_Param.sensor_enable_list[i])
		{
			return true;
		}
	}
	return false;
}

void atApp_LCD_Task_Func(void *parameter)
{
	while (1)
	{
		atApp_LCD.Run_Application(APP_RUN_MODE_AUTO);
		vTaskDelay(200 / portTICK_PERIOD_MS);
	}
}
#endif