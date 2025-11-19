#ifndef _Application_atApp_AP_
#define _Application_atApp_AP_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "App.h"
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "../src/obj/atObj_Param.h"
#include "../src/obj/atObj_Data.h"
#include "../src/services/memory/atService_SD.h"
#include "webApp.h"
/* _____DEFINITIONS__________________________________________________________ */
#define AP_SSID "DHA-1"
#define AP_PASSWORD "12345678"
#define AP_CHANNEL 1
#define AP_MAX_CONNECTIONS 4

/* _____GLOBAL VARIABLES_____________________________________________________ */
TaskHandle_t Task_atApp_AP;
void atApp_AP_Task_Func(void *parameter);
WebServer server(80);
String sessionToken = ""; // Simple session management
bool isAuthenticated = false;
unsigned long lastAuthTime = 0;
const unsigned long AUTH_TIMEOUT = 3600000; // 1 hour timeout

/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */

/**
 * This Application class is the application to manage the Access Point and Web Server
 */
class App_AP : public Application
{
public:
	App_AP();
	~App_AP();

	// API Endpoints
	static void handleRoot();
	static void handleLogin();
	static void handleLogout();
	static void handleGetConfig();
	static void handleGetDashboard();
	static void handleGetRTC();
	static void handleSaveConfig();
	static void handleNotFound();

	// Helper functions
	static bool checkAuth();
	static void sendCORS();

protected:
private:
	static void App_AP_Pend();
	static void App_AP_Start();
	static void App_AP_Restart();
	static void App_AP_Execute();
	static void App_AP_Suspend();
	static void App_AP_Resume();
	static void App_AP_End();
} atApp_AP;
/**
 * This function will be automaticaly called when a object is created by this class
 */
App_AP::App_AP(/* args */)
{
	_Pend_User = *App_AP_Pend;
	_Start_User = *App_AP_Start;
	_Restart_User = *App_AP_Restart;
	_Execute_User = *App_AP_Execute;
	_Suspend_User = *App_AP_Suspend;
	_Resume_User = *App_AP_Resume;
	_End_User = *App_AP_End;

	// change the ID of application
	ID_Application = 1;
	// change the application name
	Name_Application = (char *)"AP Application	";
	// change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
App_AP::~App_AP()
{
}

/**
 * Pend to start is the first task of this application it will do prerequisite condition. In the debit mode, task will send information of application to terminal to start the application.
 */
void App_AP::App_AP_Pend()
{
	// atService_XYZ.Debug();
	// suspend this task
	// vTaskSuspend(NULL);
}
/**
 * This start function will init some critical function
 */
void App_AP::App_AP_Start()
{
	if (atApp_AP.User_Mode == APP_USER_MODE_DEBUG)
	{
		Serial.println("[AP] Initializing Access Point Application...");
	}

	atService_SD.Run_Service();
	if (!atService_SD.isInitialized())
	{
		if (!atService_SD.begin(SD_CLK, SD_CMD, SD_D0, SD_D1, SD_D2, SD_D3, false))
		{
			if (atApp_AP.User_Mode == SER_USER_MODE_DEBUG)
			{
				Serial.println("\n✗ CANNOT INITIALIZE SD CARD!");
				Serial.println("Please check:");
				Serial.println("  - SD card is properly inserted");
				Serial.println("  - Correct SDIO pin connections:");
				Serial.printf("    CLK: GPIO%d\n", SD_CLK);
				Serial.printf("    CMD: GPIO%d\n", SD_CMD);
				Serial.printf("    D0:  GPIO%d\n", SD_D0);
				Serial.printf("    D1:  GPIO%d\n", SD_D1);
				Serial.printf("    D2:  GPIO%d\n", SD_D2);
				Serial.printf("    D3:  GPIO%d\n", SD_D3);
				Serial.println("  - SD card working properly");
				Serial.println("  - Stable 3.3V power supply");

				// Try 1-bit mode if 4-bit mode fails
				Serial.println("\n→ Retrying with 1-bit mode...");
			}
			if (!atService_SD.begin(SD_CLK, SD_CMD, SD_D0, 255, 255, 255, false))
			{
				if (atApp_AP.User_Mode == SER_USER_MODE_DEBUG)
				{
					Serial.println("✗ 1-bit mode also failed!");
				}
				return;
			}
		}
		if (atApp_AP.User_Mode == APP_USER_MODE_DEBUG)
		{
			Serial.println("✓ SD Card initialized successfully.");
			atService_SD.Debug();
			atService_SD.printCardInfo();
		}
	}

	// Configure Access Point
	// Serial.println("[AP] 1");
	WiFi.mode(WIFI_AP);
	// Serial.println("[AP] 2");
	WiFi.softAP(AP_SSID, AP_PASSWORD, AP_CHANNEL, 0, AP_MAX_CONNECTIONS);
	// Serial.println("[AP] 3");
	IPAddress IP = WiFi.softAPIP();
	// Serial.println("[AP] 4");
	if (atApp_AP.User_Mode == APP_USER_MODE_DEBUG)
	{
		Serial.print("[AP] AP IP address: ");
		Serial.println(IP);
		Serial.printf("[AP] SSID: %s\n", AP_SSID);
		Serial.printf("[AP] Password: %s\n", AP_PASSWORD);
	}

	// Serial.println("[AP] 5");
	// Setup web server routes
	server.on("/", HTTP_GET, handleRoot);
	server.on("/api/login", HTTP_POST, handleLogin);
	server.on("/api/logout", HTTP_POST, handleLogout);
	server.on("/api/config", HTTP_GET, handleGetConfig);
	server.on("/api/dashboard", HTTP_GET, handleGetDashboard);
	server.on("/api/rtc", HTTP_GET, handleGetRTC);
	server.on("/api/save", HTTP_POST, handleSaveConfig);
	server.onNotFound(handleNotFound);
	// Serial.println("[AP] 6");
	// Enable CORS for all routes
	server.enableCORS(true);
	// Serial.println("[AP] 7");
	vTaskDelay(2000 / portTICK_PERIOD_MS);
	// Start server
	// Serial.println("[AP] 8");
	server.begin();
	Serial.println("[AP] Web server started");
	Serial.println("[AP] Access portal at: http://192.168.4.1");
	vTaskDelay(100 / portTICK_PERIOD_MS);
}
/**
 * Restart function of SNM  app
 */
void App_AP::App_AP_Restart()
{
}
/**
 * Execute fuction of SNM app - handles client requests
 */
void App_AP::App_AP_Execute()
{
	server.handleClient();

	// Check auth timeout
	if (isAuthenticated && (millis() - lastAuthTime > AUTH_TIMEOUT))
	{
		isAuthenticated = false;
		sessionToken = "";
		Serial.println("[AP] Session expired");
	}
}
void App_AP::App_AP_Suspend() {}
void App_AP::App_AP_Resume() {}
void App_AP::App_AP_End() {}

// ==================== Helper Functions ====================

bool App_AP::checkAuth()
{
	if (!isAuthenticated)
	{
		return false;
	}

	// Check if session is still valid
	if (millis() - lastAuthTime > AUTH_TIMEOUT)
	{
		isAuthenticated = false;
		sessionToken = "";
		return false;
	}

	// Update last auth time
	lastAuthTime = millis();
	return true;
}

void App_AP::sendCORS()
{
	server.sendHeader("Access-Control-Allow-Origin", "*");
	server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
	server.sendHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
}

// ==================== API Endpoints ====================

void App_AP::handleRoot()
{
	sendCORS();
	server.send_P(200, "text/html", html);
}

void App_AP::handleLogin()
{
	sendCORS();

	if (server.method() == HTTP_OPTIONS)
	{
		server.send(200);
		return;
	}

	// Parse JSON request
	String body = server.arg("plain");
	JsonDocument doc;
	DeserializationError error = deserializeJson(doc, body);

	if (error)
	{
		server.send(400, "application/json", "{\"success\":false,\"message\":\"Invalid JSON\"}");
		return;
	}

	String username = doc["username"] | "";
	String password = doc["password"] | "";

	// Simple authentication (you should use more secure method in production)
	if (username == "admin" && password == "admin123")
	{
		isAuthenticated = true;
		lastAuthTime = millis();
		sessionToken = String(random(100000, 999999)); // Simple random token

		String response = "{\"success\":true,\"message\":\"Login successful\",\"token\":\"" + sessionToken + "\"}";
		server.send(200, "application/json", response);
		Serial.println("[AP] User logged in successfully");
	}
	else
	{
		server.send(401, "application/json", "{\"success\":false,\"message\":\"Invalid credentials\"}");
		Serial.println("[AP] Failed login attempt");
	}
}

void App_AP::handleLogout()
{
	sendCORS();
	isAuthenticated = false;
	sessionToken = "";
	lastAuthTime = 0;
	server.send(200, "application/json", "{\"success\":true,\"message\":\"Logged out\"}");
	Serial.println("[AP] User logged out");
}

void App_AP::handleGetConfig()
{
	sendCORS();

	if (!checkAuth())
	{
		server.send(401, "application/json", "{\"success\":false,\"message\":\"Unauthorized\"}");
		return;
	}

	// // Load config from SD card first (if available)
	// if (atService_SD.isInitialized())
	// {
	// 	if (atService_SD.fileExists("/CONFIG/Config.json"))
	// 	{
	// 		atService_SD.loadConfigFromJSON("/CONFIG/Config.json");
	// 		Serial.println("[AP] Loaded config from SD card");
	// 	}
	// }

	// Create JSON response with all configuration (matching saveConfigToJSON structure)
	JsonDocument doc;

	// ------------------------------ > Device Info < ------------------------------
	JsonObject device = doc["device_info"].to<JsonObject>();
	device["device_name"] = atObject_Param.device_name;
	device["device_id"] = atObject_Param.device_id;
	device["sub_id"] = atObject_Param.sub_id;
	device["location_X"] = atObject_Param.location_X;
	device["location_Y"] = atObject_Param.location_Y;
	device["area"] = atObject_Param.area;
	device["device_type"] = atObject_Param.device_type;

	// ----------------------- LCD Config Parameters ------------------------
	JsonObject lcd = doc["lcd_config"].to<JsonObject>();
	lcd["enable"] = atObject_Param.lcd_enable;
	lcd["backlight_enable"] = atObject_Param.lcd_backlight_enable;
	lcd["refresh_rate"] = atObject_Param.lcd_refresh_rate;

	// ------------------------ AI Config Parameters ------------------------
	JsonObject ai_config = doc["ai_config"].to<JsonObject>();
	JsonArray ai_enable = ai_config["enable_list"].to<JsonArray>();
	for (int i = 0; i < MAX_AI_CHANNEL; i++)
	{
		ai_enable.add(atObject_Param.ai_enable_list[i]);
	}
	ai_config["refresh_rate"] = atObject_Param.ai_refresh_rate;

	// ------------------------ DO Config Parameters ------------------------
	JsonObject do_config = doc["do_config"].to<JsonObject>();
	JsonArray do_enable_list = do_config["enable_list"].to<JsonArray>();
	do_config["refresh_rate"] = atObject_Param.do_refresh_rate;
	JsonArray do_protocol = do_config["protocol_list"].to<JsonArray>();
	JsonArray do_schmitt_sensor = do_config["schmitt_trigger_sensor_list"].to<JsonArray>();
	JsonArray do_schmitt_valuetype = do_config["schmitt_trigger_valuetype_list"].to<JsonArray>();
	JsonArray do_schmitt_upper = do_config["schmitt_trigger_upper_threshold_list"].to<JsonArray>();
	JsonArray do_schmitt_lower = do_config["schmitt_trigger_lower_threshold_list"].to<JsonArray>();
	JsonArray do_control_sensor = do_config["control_sensor_list"].to<JsonArray>();
	JsonArray do_control_state = do_config["control_sensor_state_list"].to<JsonArray>();
	JsonArray do_control_di = do_config["control_input_digital_list"].to<JsonArray>();
	for (int i = 0; i < MAX_DO_CHANNEL; i++)
	{
		do_enable_list.add(atObject_Param.do_enable_list[i]);
		do_protocol.add(atObject_Param.do_protocol_list[i]);
		do_schmitt_sensor.add(atObject_Param.do_control_schmitt_trigger_sensor_list[i]);
		do_schmitt_valuetype.add(atObject_Param.do_control_schmitt_trigger_sensor_valuetype_list[i]);
		do_schmitt_upper.add(atObject_Param.do_schmitt_trigger_upper_threshold_list[i]);
		do_schmitt_lower.add(atObject_Param.do_schmitt_trigger_lower_threshold_list[i]);
		do_control_sensor.add(atObject_Param.do_control_sensor_list[i]);
		do_control_state.add(atObject_Param.do_control_sensor_state_list[i]);
		do_control_di.add(atObject_Param.do_control_input_digital_list[i]);
	}

	// ------------------------- DI Config Parameters ------------------------
	JsonObject di = doc["di_config"].to<JsonObject>();
	JsonArray di_enable = di["enable_list"].to<JsonArray>();
	JsonArray di_control_enable = di["control_sensor_state_enable_list"].to<JsonArray>();
	JsonArray di_control_sensor = di["control_sensor_list"].to<JsonArray>();
	JsonArray di_control_state = di["control_sensor_state_list"].to<JsonArray>();
	di["refresh_rate"] = atObject_Param.di_refresh_rate;
	for (int i = 0; i < MAX_DI_CHANNEL; i++)
	{
		di_enable.add(atObject_Param.di_enable_list[i]);
		di_control_enable.add(atObject_Param.di_control_sensor_state_enable_list[i]);
		di_control_sensor.add(atObject_Param.di_control_sensor_list[i]);
		di_control_state.add(atObject_Param.di_control_sensor_state_list[i]);
	}

	// ------------------------------ > RS485s Config  < ------------------------------
	JsonObject rs485 = doc["rs485_config"].to<JsonObject>();
	JsonArray rs485_enable = rs485["enable"].to<JsonArray>();
	for (int i = 0; i < MAX_RS485_CHANNEL; i++)
	{
		rs485_enable.add(atObject_Param.rs485_enable[i]);
	}

	JsonArray rs485_slaves = rs485["slaves"].to<JsonArray>();
	for (int ch = 0; ch < MAX_RS485_CHANNEL; ch++)
	{
		JsonObject channel = rs485_slaves.add<JsonObject>();
		channel["channel"] = ch;

		JsonArray enable_list = channel["enable_list"].to<JsonArray>();
		JsonArray id_list = channel["id_list"].to<JsonArray>();
		JsonArray baudrate_list = channel["baudrate_list"].to<JsonArray>();
		JsonArray serial_config_list = channel["serial_config_list"].to<JsonArray>();
		JsonArray timeout_list = channel["timeout_list"].to<JsonArray>();
		JsonArray functioncode_list = channel["function_code_list"].to<JsonArray>();
		JsonArray address_list = channel["starting_address_list"].to<JsonArray>();
		JsonArray quantity_list = channel["quantity_list"].to<JsonArray>();

		JsonArray calib_enable_list = channel["calib_status_enable"].to<JsonArray>();
		JsonArray calib_fc_list = channel["calib_state_function_code"].to<JsonArray>();
		JsonArray calib_addr_list = channel["calib_state_address"].to<JsonArray>();
		JsonArray calib_value_list = channel["calib_state_value"].to<JsonArray>();

		JsonArray error_enable_list = channel["error_status_enable"].to<JsonArray>();
		JsonArray error_fc_list = channel["error_state_function_code"].to<JsonArray>();
		JsonArray error_addr_list = channel["error_state_address"].to<JsonArray>();
		JsonArray error_value_list = channel["error_state_value"].to<JsonArray>();

		for (int i = 0; i < MAX_SENSORS; i++)
		{
			enable_list.add(atObject_Param.rs485_slave_enable_list[ch][i]);
			baudrate_list.add(atObject_Param.rs485_slave_baudrate_list[ch][i]);
			serial_config_list.add(atObject_Param.rs485_slave_serial_config_list[ch][i]);
			id_list.add(atObject_Param.rs485_slave_id_list[ch][i]);
			timeout_list.add(atObject_Param.rs485_slave_timeout_list[ch][i]);
			functioncode_list.add(atObject_Param.rs485_slave_function_code_list[ch][i]);
			address_list.add(atObject_Param.rs485_slave_starting_address_list[ch][i]);
			quantity_list.add(atObject_Param.rs485_slave_quantity_list[ch][i]);
			calib_enable_list.add(atObject_Param.rs485_slave_calib_state_reading_enable_list[ch][i]);
			calib_fc_list.add(atObject_Param.rs485_slave_calib_state_function_code_list[ch][i]);
			calib_addr_list.add(atObject_Param.rs485_slave_state_calib_address_list[ch][i]);
			calib_value_list.add(atObject_Param.rs485_slave_state_calib_value_list[ch][i]);
			error_enable_list.add(atObject_Param.rs485_slave_error_state_reading_enable_list[ch][i]);
			error_fc_list.add(atObject_Param.rs485_slave_error_state_function_code_list[ch][i]);
			error_addr_list.add(atObject_Param.rs485_slave_state_error_address_list[ch][i]);
			error_value_list.add(atObject_Param.rs485_slave_state_error_value_list[ch][i]);
		}
	}

	// ------------------------------ > Sensor Config < ------------------------------
	JsonObject sensors = doc["sensor_config"].to<JsonObject>();
	JsonArray sensor_list = sensors["sensors"].to<JsonArray>();

	for (int i = 0; i < MAX_SENSORS; i++)
	{
		JsonObject sensor = sensor_list.add<JsonObject>();
		sensor["id"] = atObject_Param.sensor_id_list[i];
		sensor["enable"] = atObject_Param.sensor_enable_list[i];
		sensor["name"] = atObject_Param.sensor_name_list[i];
		sensor["unit"] = atObject_Param.sensor_unit_list[i];
		sensor["protocol"] = atObject_Param.sensor_protocol_list[i];
		sensor["state_reading_protocol"] = atObject_Param.sensor_state_protocol_list[i];

		JsonArray rs485_idx = sensor["rs485_index"].to<JsonArray>();
		for (int ch = 0; ch < MAX_RS485_CHANNEL; ch++)
		{
			rs485_idx.add(atObject_Param.sensor_rs485_index[ch][i]);
		}
		sensor["data_type"] = atObject_Param.sensor_data_type_list[i];
		sensor["read_interval"] = atObject_Param.sensor_read_interval_list[i];

		// Calibration Config
		JsonObject calib = sensor["calibration"].to<JsonObject>();
		calib["function_order"] = atObject_Param.sensor_calib_function_order_list[i];
		calib["index_A"] = atObject_Param.sensor_calib_function_index_list[i].index_A;
		calib["index_B"] = atObject_Param.sensor_calib_function_index_list[i].index_B;
		calib["index_C"] = atObject_Param.sensor_calib_function_index_list[i].index_C;
		calib["index_D"] = atObject_Param.sensor_calib_function_index_list[i].index_D;

		JsonArray table_points = calib["point_table"].to<JsonArray>();
		for (int j = 0; j < MAX_CALIB_TABLE_POINTS; j++)
		{
			JsonObject point = table_points.add<JsonObject>();
			point["raw_value"] = atObject_Param.sensor_calib_table_list[i][j].raw_value;
			point["calib_value"] = atObject_Param.sensor_calib_table_list[i][j].calib_value;
			point["enable"] = atObject_Param.sensor_calib_table_point_active_list[i][j];
		}

		JsonObject error = sensor["error"].to<JsonObject>();
		error["enable"] = atObject_Param.sensor_error_enable_list[i];
		error["raw_lower"] = atObject_Param.sensor_raw_error_lower_threshold_list[i];
		error["raw_upper"] = atObject_Param.sensor_raw_error_upper_threshold_list[i];
		error["calib_lower"] = atObject_Param.sensor_calib_error_lower_threshold_list[i];
		error["calib_upper"] = atObject_Param.sensor_calib_error_upper_threshold_list[i];

		JsonObject limit = sensor["limit"].to<JsonObject>();
		limit["enable"] = atObject_Param.sensor_limit_enable_list[i];
		limit["lower_threshold"] = atObject_Param.sensor_calib_lower_limit_threshold_list[i];
		limit["lower_noise"] = atObject_Param.sensor_lower_limit_noise_list[i];
		limit["upper_threshold"] = atObject_Param.sensor_calib_upper_limit_threshold_list[i];
		limit["upper_noise"] = atObject_Param.sensor_upper_limit_noise_list[i];
	}

	// MQTT Config
	JsonObject mqtt = doc["mqtt_config"].to<JsonObject>();
	mqtt["public_enable"] = atObject_Param.mqtt_public_enable;
	mqtt["subscribe_enable"] = atObject_Param.mqtt_subscribe_enable;
	mqtt["sending_interval"] = atObject_Param.mqtt_sending_interval;
	mqtt["port"] = atObject_Param.mqtt_port;
	mqtt["pub_topic"] = atObject_Param.mqtt_pub_topic;
	mqtt["sub_topic"] = atObject_Param.mqtt_sub_topic;
	mqtt["broker"] = atObject_Param.mqtt_broker;
	mqtt["client_id"] = atObject_Param.mqtt_client_id;
	mqtt["username"] = atObject_Param.mqtt_username;
	mqtt["password"] = atObject_Param.mqtt_password;
	mqtt["max_retry_times"] = atObject_Param.mqtt_max_retry_times;

	// FTP Config
	JsonObject ftp = doc["ftp_config"].to<JsonObject>();
	JsonArray ftp_servers = ftp["servers"].to<JsonArray>();

	for (int i = 0; i < MAX_FTP_SERVER; i++)
	{
		JsonObject server = ftp_servers.add<JsonObject>();
		server["index"] = i;
		server["active"] = atObject_Param.ftp_active_list[i];
		server["host"] = atObject_Param.ftp_host_list[i];
		server["port"] = atObject_Param.ftp_port_list[i];
		server["timeout"] = atObject_Param.ftp_timeout_list[i];
		server["user"] = atObject_Param.ftp_user_list[i];
		server["pass"] = atObject_Param.ftp_pass_list[i];
		server["path"] = atObject_Param.ftp_path_list[i];
		server["file_name"] = atObject_Param.ftp_file_name_list[i];
		server["retry_times"] = atObject_Param.ftp_retry_times_list[i];
		server["send_time"] = atObject_Param.ftp_send_time_list[i];
		server["max_file_number"] = atObject_Param.ftp_max_file_number_per_one_send_time[i];

		JsonArray sensor_cfg = server["sensor_config"].to<JsonArray>();
		for (int j = 0; j < MAX_SENSORS; j++)
		{
			sensor_cfg.add(atObject_Param.ftp_sensor_config[i][j]);
		}
	}

	// Logger Config
	JsonObject logger = doc["logger_config"].to<JsonObject>();
	JsonArray save_paths = logger["save_paths"].to<JsonArray>();
	JsonArray sent_paths = logger["sent_paths"].to<JsonArray>();
	for (int i = 0; i < MAX_FTP_SERVER; i++)
	{
		save_paths.add(atObject_Param.logger_save_path[i]);
		sent_paths.add(atObject_Param.logger_save_sent_file_path[i]);
	}

	// RTC Config
	JsonObject rtc = doc["rtc_config"].to<JsonObject>();
	rtc["setting_request"] = atObject_Param.rtc_setting_request;
	rtc["hour"] = atObject_Param.rtc_setting_hour;
	rtc["minute"] = atObject_Param.rtc_setting_minute;
	rtc["second"] = atObject_Param.rtc_setting_second;
	rtc["day"] = atObject_Param.rtc_setting_day;
	rtc["month"] = atObject_Param.rtc_setting_month;
	rtc["year"] = atObject_Param.rtc_setting_year;
	rtc["auto_update_from_ntp"] = atObject_Param.rtc_auto_update_from_ntp;
	rtc["ntp_server"] = atObject_Param.ntp_server;

	// Serialize and send
	String response;
	serializeJson(doc, response);
	server.send(200, "application/json", response);
	Serial.printf("[AP] Config sent (%d bytes)\n", response.length());

	// Free memory
	doc.clear();
}

void App_AP::handleGetDashboard()
{
	sendCORS();

	if (!checkAuth())
	{
		server.send(401, "application/json", "{\"success\":false,\"message\":\"Unauthorized\"}");
		return;
	}

	// Create lightweight dashboard JSON (only sensor values)
	JsonDocument doc;

	JsonArray sensors = doc["sensors"].to<JsonArray>();
	for (int i = 0; i < MAX_SENSORS; i++)
	{

		JsonObject sensor = sensors.add<JsonObject>();
		sensor["id"] = i;
		sensor["name"] = atObject_Param.sensor_name_list[i];
		sensor["unit"] = atObject_Param.sensor_unit_list[i];
		sensor["raw"] = atObject_Data.sensor_raw_value_list[i];
		sensor["calib"] = atObject_Data.sensor_calib_value_list[i];
		sensor["sum"] = atObject_Data.sensor_sum_value_list[i];
		sensor["status"] = atObject_Data.sensor_status_list[i];
		sensor["alarm"] = atObject_Data.sensor_alarm_status_list[i];
		sensor["warning"] = atObject_Data.sensor_warning_status_list[i];
	}
	JsonObject system = doc["system"].to<JsonObject>();
	// Add system info
	system["uptime"] = millis() / 1000;
	system["free_heap"] = ESP.getFreeHeap();
	system["client_number"] = WiFi.softAPgetStationNum();
	system["ip_address"] = WiFi.softAPIP().toString();
	system["sim_ready"] = atObject_Data.simReady;
	system["mqtt_connected"] = atObject_Data.mqttConnected;
	system["ftp_connected"] = atObject_Data.ftpConnected;
	system["sd_card_mounted"] = atObject_Data.sdCardMounted;

	// Add rtc jsonobject
	JsonObject rtc = doc["rtc"].to<JsonObject>();
	rtc["year"] = atObject_Data.rtc_year;
	rtc["month"] = atObject_Data.rtc_month;
	rtc["day"] = atObject_Data.rtc_day;
	rtc["hour"] = atObject_Data.rtc_hour;
	rtc["minute"] = atObject_Data.rtc_minute;
	rtc["second"] = atObject_Data.rtc_second;
	rtc["day_of_week"] = atObject_Data.rtc_day_of_week;
	rtc["temperature"] = atObject_Data.rtc_temperature;

	// 4-20mA channels
	JsonArray ai_channels = doc["ai_channels"].to<JsonArray>();
	for (int i = 0; i < MAX_AI_CHANNEL; i++)
	{
		JsonObject channel = ai_channels.add<JsonObject>();
		channel["id"] = i;
		channel["enabled"] = atObject_Param.ai_enable_list[i];
		channel["raw_value"] = atObject_Data.ai_raw[i];
		channel["ai_voltage"] = atObject_Data.ai_voltage[i];
		channel["ai_current"] = atObject_Data.ai_current[i];
	}
	// DO channels
	JsonArray do_channels = doc["do_channels"].to<JsonArray>();
	for (int i = 0; i < MAX_DO_CHANNEL; i++)
	{
		JsonObject channel = do_channels.add<JsonObject>();
		channel["id"] = i;
		channel["enabled"] = atObject_Param.do_enable_list[i];
		channel["state"] = atObject_Data.do_state[i];
	}
	// DI channels
	JsonArray di_channels = doc["di_channels"].to<JsonArray>();
	for (int i = 0; i < MAX_DI_CHANNEL; i++)
	{
		JsonObject channel = di_channels.add<JsonObject>();
		channel["id"] = i;
		channel["enabled"] = atObject_Param.di_enable_list[i];
		channel["state"] = atObject_Data.di_state[i];
	}
	// RS485 Channel 1
	JsonArray rs485_channel_1 = doc["rs485_channel_1"].to<JsonArray>();
	for (int i = 0; i < MAX_SENSORS; i++)
	{
		JsonObject slave = rs485_channel_1.add<JsonObject>();
		slave["id"] = i;
		slave["enabled"] = atObject_Param.rs485_slave_enable_list[0][i];
		slave["high_16bits"] = atObject_Data.rs485_data_list[0][i].high16bitByte;
		slave["low_16bits"] = atObject_Data.rs485_data_list[0][i].low16bitByte;
		slave["response"] = atObject_Data.rs485_response_list[0][i];
		slave["calib_reg"] = atObject_Data.rs485_calib_status_reg_list[0][i];
		slave["error_reg"] = atObject_Data.rs485_error_status_reg_list[0][i];
	}
	// RS485 Channel 2
	JsonArray rs485_channel_2 = doc["rs485_channel_2"].to<JsonArray>();
	for (int i = 0; i < MAX_SENSORS; i++)
	{
		JsonObject slave = rs485_channel_2.add<JsonObject>();
		slave["id"] = i;
		slave["enabled"] = atObject_Param.rs485_slave_enable_list[1][i];
		slave["high 16bits"] = atObject_Data.rs485_data_list[1][i].high16bitByte;
		slave["low 16bits"] = atObject_Data.rs485_data_list[1][i].low16bitByte;
		slave["response"] = atObject_Data.rs485_response_list[1][i];
		slave["calib_reg"] = atObject_Data.rs485_calib_status_reg_list[1][i];
		slave["error_reg"] = atObject_Data.rs485_error_status_reg_list[1][i];
	}
	// Serialize and send
	String response;
	serializeJson(doc, response);
	server.send(200, "application/json", response);
	Serial.printf("[AP] Dashboard data sent (%d bytes)\n", response.length());
	// Serial.println(response);
}

void App_AP::handleGetRTC()
{
	sendCORS();

	if (!checkAuth())
	{
		server.send(401, "application/json", "{\"success\":false,\"message\":\"Unauthorized\"}");
		return;
	}

	// Create RTC JSON
	JsonDocument doc;
	JsonObject rtc = doc["rtc"].to<JsonObject>();
	rtc["year"] = atObject_Data.rtc_year;
	rtc["month"] = atObject_Data.rtc_month;
	rtc["day"] = atObject_Data.rtc_day;
	rtc["hour"] = atObject_Data.rtc_hour;
	rtc["minute"] = atObject_Data.rtc_minute;
	rtc["second"] = atObject_Data.rtc_second;

	// Serialize and send
	String response;
	serializeJson(doc, response);
	server.send(200, "application/json", response);
	Serial.printf("[AP] RTC data sent (%d bytes)\n", response.length());
}

void App_AP::handleSaveConfig()
{
	sendCORS();

	if (!checkAuth())
	{
		server.send(401, "application/json", "{\"success\":false,\"message\":\"Unauthorized\"}");
		return;
	}

	Serial.println("[AP] Receiving configuration from web...");

	// Parse JSON request
	String body = server.arg("plain");
	Serial.printf("[AP] Received %d bytes of JSON data\n", body.length());

	// Debug: Print first 500 chars of JSON
	if (body.length() < 500)
	{
		Serial.println("[AP] JSON body: " + body);
	}
	else
	{
		Serial.println("[AP] JSON preview: " + body.substring(0, 500) + "...");
	}

	JsonDocument doc;
	DeserializationError error = deserializeJson(doc, body);

	if (error)
	{
		Serial.printf("[AP] JSON parse error: %s\n", error.c_str());
		String errorMsg = "{\"success\":false,\"message\":\"JSON parse error: ";
		errorMsg += error.c_str();
		errorMsg += "\"}";
		server.send(400, "application/json", errorMsg);
		return;
	}

	Serial.println("[AP] JSON parsed successfully");
	Serial.println("[AP] ========================================");
	Serial.println("[AP] Starting configuration update...");

	// ========== 1. DEVICE INFO ==========
	if (!doc["device_info"].isNull())
	{
		JsonObject device = doc["device_info"];
		if (!device["device_name"].isNull())
			atObject_Param.device_name = device["device_name"].as<String>();
		if (!device["device_id"].isNull())
			atObject_Param.device_id = device["device_id"].as<String>();
		if (!device["sub_id"].isNull())
			atObject_Param.sub_id = device["sub_id"].as<String>();
		if (!device["location_X"].isNull())
			atObject_Param.location_X = device["location_X"].as<String>();
		if (!device["location_Y"].isNull())
			atObject_Param.location_Y = device["location_Y"].as<String>();
		if (!device["area"].isNull())
			atObject_Param.area = device["area"].as<String>();
		if (!device["device_type"].isNull())
			atObject_Param.device_type = device["device_type"].as<String>();
		Serial.println("[AP] ✓ Device info updated");
	}

	// ========== 2. LCD CONFIG ==========
	if (!doc["lcd_config"].isNull())
	{
		JsonObject lcd = doc["lcd_config"];
		if (!lcd["enable"].isNull())
			atObject_Param.lcd_enable = lcd["enable"];
		if (!lcd["backlight_enable"].isNull())
			atObject_Param.lcd_backlight_enable = lcd["backlight_enable"];
		if (!lcd["refresh_rate"].isNull())
			atObject_Param.lcd_refresh_rate = lcd["refresh_rate"];
		Serial.println("[AP] ✓ LCD config updated");
	}

	// ========== 3. AI CONFIG ==========
	if (!doc["ai_config"].isNull())
	{
		JsonObject ai_config = doc["ai_config"];
		if (!ai_config["refresh_rate"].isNull())
			atObject_Param.ai_refresh_rate = ai_config["refresh_rate"];

		if (!ai_config["enable_list"].isNull())
		{
			JsonArray ai_enable = ai_config["enable_list"];
			for (int i = 0; i < ai_enable.size() && i < MAX_AI_CHANNEL; i++)
			{
				atObject_Param.ai_enable_list[i] = ai_enable[i];
			}
		}
		Serial.println("[AP] ✓ AI config updated");
	}

	// ========== 4. DO CONFIG ==========
	if (!doc["do_config"].isNull())
	{
		JsonObject do_config = doc["do_config"];
		if (!do_config["refresh_rate"].isNull())
			atObject_Param.do_refresh_rate = do_config["refresh_rate"];

		if (!do_config["enable_list"].isNull())
		{
			JsonArray do_enable = do_config["enable_list"];
			for (int i = 0; i < do_enable.size() && i < MAX_DO_CHANNEL; i++)
			{
				atObject_Param.do_enable_list[i] = do_enable[i];
			}
		}

		if (!do_config["protocol_list"].isNull())
		{
			JsonArray do_protocol = do_config["protocol_list"];
			for (int i = 0; i < do_protocol.size() && i < MAX_DO_CHANNEL; i++)
			{
				atObject_Param.do_protocol_list[i] = do_protocol[i];
			}
		}

		if (!do_config["schmitt_trigger_sensor_list"].isNull())
		{
			JsonArray do_schmitt_sensor = do_config["schmitt_trigger_sensor_list"];
			for (int i = 0; i < do_schmitt_sensor.size() && i < MAX_DO_CHANNEL; i++)
			{
				atObject_Param.do_control_schmitt_trigger_sensor_list[i] = do_schmitt_sensor[i];
			}
		}

		if (!do_config["schmitt_trigger_valuetype_list"].isNull())
		{
			JsonArray do_schmitt_valuetype = do_config["schmitt_trigger_valuetype_list"];
			for (int i = 0; i < do_schmitt_valuetype.size() && i < MAX_DO_CHANNEL; i++)
			{
				atObject_Param.do_control_schmitt_trigger_sensor_valuetype_list[i] = do_schmitt_valuetype[i];
			}
		}

		if (!do_config["schmitt_trigger_upper_threshold_list"].isNull())
		{
			JsonArray do_schmitt_upper = do_config["schmitt_trigger_upper_threshold_list"];
			for (int i = 0; i < do_schmitt_upper.size() && i < MAX_DO_CHANNEL; i++)
			{
				atObject_Param.do_schmitt_trigger_upper_threshold_list[i] = do_schmitt_upper[i];
			}
		}

		if (!do_config["schmitt_trigger_lower_threshold_list"].isNull())
		{
			JsonArray do_schmitt_lower = do_config["schmitt_trigger_lower_threshold_list"];
			for (int i = 0; i < do_schmitt_lower.size() && i < MAX_DO_CHANNEL; i++)
			{
				atObject_Param.do_schmitt_trigger_lower_threshold_list[i] = do_schmitt_lower[i];
			}
		}

		if (!do_config["control_sensor_list"].isNull())
		{
			JsonArray do_control_sensor = do_config["control_sensor_list"];
			for (int i = 0; i < do_control_sensor.size() && i < MAX_DO_CHANNEL; i++)
			{
				atObject_Param.do_control_sensor_list[i] = do_control_sensor[i];
			}
		}

		if (!do_config["control_sensor_state_list"].isNull())
		{
			JsonArray do_control_state = do_config["control_sensor_state_list"];
			for (int i = 0; i < do_control_state.size() && i < MAX_DO_CHANNEL; i++)
			{
				atObject_Param.do_control_sensor_state_list[i] = do_control_state[i];
			}
		}

		if (!do_config["control_input_digital_list"].isNull())
		{
			JsonArray do_control_di = do_config["control_input_digital_list"];
			for (int i = 0; i < do_control_di.size() && i < MAX_DO_CHANNEL; i++)
			{
				atObject_Param.do_control_input_digital_list[i] = do_control_di[i];
			}
		}
		Serial.println("[AP] ✓ DO config updated");
	}

	// ========== 5. DI CONFIG ==========
	if (!doc["di_config"].isNull())
	{
		JsonObject di_config = doc["di_config"];
		if (!di_config["refresh_rate"].isNull())
			atObject_Param.di_refresh_rate = di_config["refresh_rate"];

		if (!di_config["enable_list"].isNull())
		{
			JsonArray di_enable = di_config["enable_list"];
			for (int i = 0; i < di_enable.size() && i < MAX_DI_CHANNEL; i++)
			{
				atObject_Param.di_enable_list[i] = di_enable[i];
			}
		}

		if (!di_config["control_sensor_state_enable_list"].isNull())
		{
			JsonArray di_control_enable = di_config["control_sensor_state_enable_list"];
			for (int i = 0; i < di_control_enable.size() && i < MAX_DI_CHANNEL; i++)
			{
				atObject_Param.di_control_sensor_state_enable_list[i] = di_control_enable[i];
			}
		}

		if (!di_config["control_sensor_list"].isNull())
		{
			JsonArray di_control_sensor = di_config["control_sensor_list"];
			for (int i = 0; i < di_control_sensor.size() && i < MAX_DI_CHANNEL; i++)
			{
				atObject_Param.di_control_sensor_list[i] = di_control_sensor[i];
			}
		}

		if (!di_config["control_sensor_state_list"].isNull())
		{
			JsonArray di_control_state = di_config["control_sensor_state_list"];
			for (int i = 0; i < di_control_state.size() && i < MAX_DI_CHANNEL; i++)
			{
				atObject_Param.di_control_sensor_state_list[i] = di_control_state[i];
			}
		}
		Serial.println("[AP] ✓ DI config updated");
	}

	// ========== 6. RS485 CONFIG ==========
	if (!doc["rs485_config"].isNull())
	{
		JsonObject rs485_config = doc["rs485_config"];

		if (!rs485_config["enable"].isNull())
		{
			JsonArray rs485_enable = rs485_config["enable"];
			for (int i = 0; i < rs485_enable.size() && i < MAX_RS485_CHANNEL; i++)
			{
				atObject_Param.rs485_enable[i] = rs485_enable[i];
			}
		}

		if (!rs485_config["slaves"].isNull())
		{
			JsonArray slaves = rs485_config["slaves"];
			for (int ch = 0; ch < slaves.size() && ch < MAX_RS485_CHANNEL; ch++)
			{
				JsonObject channel = slaves[ch];

				if (!channel["enable_list"].isNull())
				{
					JsonArray enable_list = channel["enable_list"];
					for (int i = 0; i < enable_list.size() && i < MAX_SENSORS; i++)
					{
						atObject_Param.rs485_slave_enable_list[ch][i] = enable_list[i];
					}
				}

				if (!channel["id_list"].isNull())
				{
					JsonArray id_list = channel["id_list"];
					for (int i = 0; i < id_list.size() && i < MAX_SENSORS; i++)
					{
						atObject_Param.rs485_slave_id_list[ch][i] = id_list[i];
					}
				}

				if (!channel["baudrate_list"].isNull())
				{
					JsonArray baudrate_list = channel["baudrate_list"];
					for (int i = 0; i < baudrate_list.size() && i < MAX_SENSORS; i++)
					{
						atObject_Param.rs485_slave_baudrate_list[ch][i] = baudrate_list[i];
					}
				}

				if (!channel["serial_config_list"].isNull())
				{
					JsonArray serial_config_list = channel["serial_config_list"];
					for (int i = 0; i < serial_config_list.size() && i < MAX_SENSORS; i++)
					{
						atObject_Param.rs485_slave_serial_config_list[ch][i] = serial_config_list[i];
					}
				}

				if (!channel["timeout_list"].isNull())
				{
					JsonArray timeout_list = channel["timeout_list"];
					for (int i = 0; i < timeout_list.size() && i < MAX_SENSORS; i++)
					{
						atObject_Param.rs485_slave_timeout_list[ch][i] = timeout_list[i];
					}
				}

				if (!channel["function_code_list"].isNull())
				{
					JsonArray function_code_list = channel["function_code_list"];
					for (int i = 0; i < function_code_list.size() && i < MAX_SENSORS; i++)
					{
						atObject_Param.rs485_slave_function_code_list[ch][i] = function_code_list[i];
					}
				}

				if (!channel["starting_address_list"].isNull())
				{
					JsonArray starting_address_list = channel["starting_address_list"];
					for (int i = 0; i < starting_address_list.size() && i < MAX_SENSORS; i++)
					{
						atObject_Param.rs485_slave_starting_address_list[ch][i] = starting_address_list[i];
					}
				}

				if (!channel["quantity_list"].isNull())
				{
					JsonArray quantity_list = channel["quantity_list"];
					for (int i = 0; i < quantity_list.size() && i < MAX_SENSORS; i++)
					{
						atObject_Param.rs485_slave_quantity_list[ch][i] = quantity_list[i];
					}
				}

				// Calibration status
				if (!channel["calib_status_enable"].isNull())
				{
					JsonArray calib_enable = channel["calib_status_enable"];
					for (int i = 0; i < calib_enable.size() && i < MAX_SENSORS; i++)
					{
						atObject_Param.rs485_slave_calib_state_reading_enable_list[ch][i] = calib_enable[i];
					}
				}

				if (!channel["calib_state_function_code"].isNull())
				{
					JsonArray calib_fc = channel["calib_state_function_code"];
					for (int i = 0; i < calib_fc.size() && i < MAX_SENSORS; i++)
					{
						atObject_Param.rs485_slave_calib_state_function_code_list[ch][i] = calib_fc[i];
					}
				}

				if (!channel["calib_state_address"].isNull())
				{
					JsonArray calib_addr = channel["calib_state_address"];
					for (int i = 0; i < calib_addr.size() && i < MAX_SENSORS; i++)
					{
						atObject_Param.rs485_slave_state_calib_address_list[ch][i] = calib_addr[i];
					}
				}

				if (!channel["calib_state_value"].isNull())
				{
					JsonArray calib_value = channel["calib_state_value"];
					for (int i = 0; i < calib_value.size() && i < MAX_SENSORS; i++)
					{
						atObject_Param.rs485_slave_state_calib_value_list[ch][i] = calib_value[i];
					}
				}

				// Error status
				if (!channel["error_status_enable"].isNull())
				{
					JsonArray error_enable = channel["error_status_enable"];
					for (int i = 0; i < error_enable.size() && i < MAX_SENSORS; i++)
					{
						atObject_Param.rs485_slave_error_state_reading_enable_list[ch][i] = error_enable[i];
					}
				}

				if (!channel["error_state_function_code"].isNull())
				{
					JsonArray error_fc = channel["error_state_function_code"];
					for (int i = 0; i < error_fc.size() && i < MAX_SENSORS; i++)
					{
						atObject_Param.rs485_slave_error_state_function_code_list[ch][i] = error_fc[i];
					}
				}

				if (!channel["error_state_address"].isNull())
				{
					JsonArray error_addr = channel["error_state_address"];
					for (int i = 0; i < error_addr.size() && i < MAX_SENSORS; i++)
					{
						atObject_Param.rs485_slave_state_error_address_list[ch][i] = error_addr[i];
					}
				}

				if (!channel["error_state_value"].isNull())
				{
					JsonArray error_value = channel["error_state_value"];
					for (int i = 0; i < error_value.size() && i < MAX_SENSORS; i++)
					{
						atObject_Param.rs485_slave_state_error_value_list[ch][i] = error_value[i];
					}
				}
			}
		}
		Serial.println("[AP] ✓ RS485 config updated");
	}

	// ========== 7. SENSOR CONFIG ==========
	if (!doc["sensor_config"].isNull())
	{
		JsonObject sensor_config = doc["sensor_config"];
		if (!sensor_config["sensors"].isNull())
		{
			JsonArray sensors = sensor_config["sensors"];
			for (int i = 0; i < sensors.size() && i < MAX_SENSORS; i++)
			{
				JsonObject sensor = sensors[i];

				if (!sensor["enable"].isNull())
					atObject_Param.sensor_enable_list[i] = sensor["enable"];
				if (!sensor["name"].isNull())
					atObject_Param.sensor_name_list[i] = sensor["name"].as<String>();
				if (!sensor["unit"].isNull())
					atObject_Param.sensor_unit_list[i] = sensor["unit"].as<String>();
				if (!sensor["read_interval"].isNull())
					atObject_Param.sensor_read_interval_list[i] = sensor["read_interval"];
				if (!sensor["protocol"].isNull())
					atObject_Param.sensor_protocol_list[i] = sensor["protocol"];
				if (!sensor["state_reading_protocol"].isNull())
					atObject_Param.sensor_state_protocol_list[i] = sensor["state_reading_protocol"];

				if (atObject_Param.sensor_protocol_list[i] == PROTOCOL_RS485_CH1)
				{
					if (!sensor["rs485_slave_index"].isNull())
					{
						atObject_Param.sensor_rs485_index[0][i] = sensor["rs485_slave_index"];
					}
				}
				else if (atObject_Param.sensor_protocol_list[i] == PROTOCOL_RS485_CH2)
				{
					if (!sensor["rs485_slave_index"].isNull())
					{
						atObject_Param.sensor_rs485_index[1][i] = sensor["rs485_slave_index"];
					}
				}
				if (!sensor["data_type"].isNull())
					atObject_Param.sensor_data_type_list[i] = sensor["data_type"];
				if (!sensor["id"].isNull())
					atObject_Param.sensor_id_list[i] = sensor["id"];

				// Calibration
				if (!sensor["calibration"].isNull())
				{
					JsonObject calib = sensor["calibration"];
					if (!calib["function_order"].isNull())
						atObject_Param.sensor_calib_function_order_list[i] = calib["function_order"];
					if (!calib["index_A"].isNull())
						atObject_Param.sensor_calib_function_index_list[i].index_A = calib["index_A"];
					if (!calib["index_B"].isNull())
						atObject_Param.sensor_calib_function_index_list[i].index_B = calib["index_B"];
					if (!calib["index_C"].isNull())
						atObject_Param.sensor_calib_function_index_list[i].index_C = calib["index_C"];
					if (!calib["index_D"].isNull())
						atObject_Param.sensor_calib_function_index_list[i].index_D = calib["index_D"];

					// Point table
					if (!calib["point_table"].isNull())
					{
						JsonArray points = calib["point_table"];
						for (int p = 0; p < points.size() && p < MAX_CALIB_TABLE_POINTS; p++)
						{
							JsonObject point = points[p];
							if (!point["enable"].isNull())
								atObject_Param.sensor_calib_table_point_active_list[i][p] = point["enable"];
							if (!point["raw_value"].isNull())
								atObject_Param.sensor_calib_table_list[i][p].raw_value = point["raw_value"];
							if (!point["calib_value"].isNull())
								atObject_Param.sensor_calib_table_list[i][p].calib_value = point["calib_value"];
						}
					}
				}

				// Error thresholds
				if (!sensor["error"].isNull())
				{
					JsonObject error = sensor["error"];
					if (!error["enable"].isNull())
						atObject_Param.sensor_error_enable_list[i] = error["enable"];
					if (!error["raw_lower"].isNull())
						atObject_Param.sensor_raw_error_lower_threshold_list[i] = error["raw_lower"];
					if (!error["raw_upper"].isNull())
						atObject_Param.sensor_raw_error_upper_threshold_list[i] = error["raw_upper"];
					if (!error["calib_lower"].isNull())
						atObject_Param.sensor_calib_error_lower_threshold_list[i] = error["calib_lower"];
					if (!error["calib_upper"].isNull())
						atObject_Param.sensor_calib_error_upper_threshold_list[i] = error["calib_upper"];
				}

				// Limit (noise) thresholds
				if (!sensor["limit"].isNull())
				{
					JsonObject limit = sensor["limit"];
					if (!limit["enable"].isNull())
						atObject_Param.sensor_limit_enable_list[i] = limit["enable"];
					if (!limit["lower_threshold"].isNull())
						atObject_Param.sensor_calib_lower_limit_threshold_list[i] = limit["lower_threshold"];
					if (!limit["lower_noise"].isNull())
						atObject_Param.sensor_lower_limit_noise_list[i] = limit["lower_noise"];
					if (!limit["upper_threshold"].isNull())
						atObject_Param.sensor_calib_upper_limit_threshold_list[i] = limit["upper_threshold"];
					if (!limit["upper_noise"].isNull())
						atObject_Param.sensor_upper_limit_noise_list[i] = limit["upper_noise"];
				}
			}
		}
		Serial.println("[AP] ✓ Sensor config updated");
	}

	// ========== 8. MQTT CONFIG ==========
	if (!doc["mqtt_config"].isNull())
	{
		JsonObject mqtt = doc["mqtt_config"];
		if (!mqtt["public_enable"].isNull())
			atObject_Param.mqtt_public_enable = mqtt["public_enable"];
		if (!mqtt["subscribe_enable"].isNull())
			atObject_Param.mqtt_subscribe_enable = mqtt["subscribe_enable"];
		if (!mqtt["sending_interval"].isNull())
			atObject_Param.mqtt_sending_interval = mqtt["sending_interval"];
		if (!mqtt["port"].isNull())
			atObject_Param.mqtt_port = mqtt["port"];
		if (!mqtt["pub_topic"].isNull())
			atObject_Param.mqtt_pub_topic = mqtt["pub_topic"].as<String>();
		if (!mqtt["sub_topic"].isNull())
			atObject_Param.mqtt_sub_topic = mqtt["sub_topic"].as<String>();
		if (!mqtt["broker"].isNull())
			atObject_Param.mqtt_broker = mqtt["broker"].as<String>();
		if (!mqtt["client_id"].isNull())
			atObject_Param.mqtt_client_id = mqtt["client_id"].as<String>();
		if (!mqtt["username"].isNull())
			atObject_Param.mqtt_username = mqtt["username"].as<String>();
		if (!mqtt["password"].isNull())
			atObject_Param.mqtt_password = mqtt["password"].as<String>();
		if (!mqtt["max_retry_times"].isNull())
			atObject_Param.mqtt_max_retry_times = mqtt["max_retry_times"];
		Serial.println("[AP] ✓ MQTT config updated");
	}

	// ========== 9. FTP CONFIG ==========
	if (!doc["ftp_config"].isNull())
	{
		JsonObject ftp_config = doc["ftp_config"];
		if (!ftp_config["servers"].isNull())
		{
			JsonArray servers = ftp_config["servers"];
			for (int i = 0; i < servers.size() && i < MAX_FTP_SERVER; i++)
			{
				JsonObject server = servers[i];

				if (!server["is_active"].isNull())
					atObject_Param.ftp_active_list[i] = server["is_active"];
				if (!server["host"].isNull())
					atObject_Param.ftp_host_list[i] = server["host"].as<String>();
				if (!server["port"].isNull())
					atObject_Param.ftp_port_list[i] = server["port"];
				if (!server["username"].isNull())
					atObject_Param.ftp_user_list[i] = server["username"].as<String>();
				if (!server["password"].isNull())
					atObject_Param.ftp_pass_list[i] = server["password"].as<String>();
				if (!server["timeout"].isNull())
					atObject_Param.ftp_timeout_list[i] = server["timeout"];
				if (!server["max_retry"].isNull())
					atObject_Param.ftp_retry_times_list[i] = server["max_retry"];
				if (!server["path"].isNull())
					atObject_Param.ftp_path_list[i] = server["path"].as<String>();
				if (!server["file_name_pattern"].isNull())
					atObject_Param.ftp_file_name_list[i] = server["file_name_pattern"].as<String>();
				if (!server["send_time_at_minute"].isNull())
					atObject_Param.ftp_send_time_list[i] = server["send_time_at_minute"];
				if (!server["max_file_send_number_one_time"].isNull())
					atObject_Param.ftp_max_file_number_per_one_send_time[i] = server["max_file_send_number_one_time"];

				// Handle sensor_config array
				if (!server["sensor_config"].isNull())
				{
					JsonArray sensor_config = server["sensor_config"];
					for (int j = 0; j < sensor_config.size() && j < MAX_SENSORS; j++)
					{
						atObject_Param.ftp_sensor_config[i][j] = sensor_config[j];
					}
				}
			}
		}
		Serial.println("[AP] ✓ FTP config updated");
	}

	// ========== 10. LOGGER CONFIG ==========
	if (!doc["logger_config"].isNull())
	{
		JsonObject logger_config = doc["logger_config"];
		if (!logger_config["save_paths"].isNull())
		{
			JsonArray save_paths = logger_config["save_paths"];
			for (int i = 0; i < save_paths.size() && i < MAX_FTP_SERVER; i++)
			{
				atObject_Param.logger_save_path[i] = save_paths[i].as<String>();
			}
		}
		if (!logger_config["sent_paths"].isNull())
		{
			JsonArray sent_paths = logger_config["sent_paths"];
			for (int i = 0; i < sent_paths.size() && i < MAX_FTP_SERVER; i++)
			{
				atObject_Param.logger_save_sent_file_path[i] = sent_paths[i].as<String>();
			}
		}
		Serial.println("[AP] ✓ Logger config updated");
	}

	// ========== 11. RTC CONFIG ==========
	if (!doc["rtc_config"].isNull())
	{
		JsonObject rtc = doc["rtc_config"];
		if (!rtc["setting_request"].isNull())
			atObject_Param.rtc_setting_request = rtc["setting_request"];
		if (!rtc["hour"].isNull())
			atObject_Param.rtc_setting_hour = rtc["hour"];
		if (!rtc["minute"].isNull())
			atObject_Param.rtc_setting_minute = rtc["minute"];
		if (!rtc["second"].isNull())
			atObject_Param.rtc_setting_second = rtc["second"];
		if (!rtc["day"].isNull())
			atObject_Param.rtc_setting_day = rtc["day"];
		if (!rtc["month"].isNull())
			atObject_Param.rtc_setting_month = rtc["month"];
		if (!rtc["year"].isNull())
			atObject_Param.rtc_setting_year = rtc["year"];
		if (!rtc["auto_update_from_ntp"].isNull())
			atObject_Param.rtc_auto_update_from_ntp = rtc["auto_update_from_ntp"];
		if (!rtc["ntp_server"].isNull())
			atObject_Param.ntp_server = rtc["ntp_server"].as<String>();
		Serial.println("[AP] ✓ RTC config updated");
	}

	// Free memory
	doc.clear();

	Serial.println("[AP] ========================================");
	Serial.println("[AP] ✅\tALL CONFIGURATION UPDATED IN RAM");
	Serial.println("[AP] ========================================");

	// Save to SD card
	bool saveSuccess = false;
	if (atService_SD.isInitialized())
	{
		// Ensure /CONFIG directory exists
		if (!atService_SD.dirExists("/CONFIG"))
		{
			Serial.println("[AP] Creating /CONFIG directory...");
			if (!atService_SD.createDir("/CONFIG"))
			{
				Serial.println("[AP] Failed to create /CONFIG directory");
				server.send(500, "application/json", "{\"success\":false,\"message\":\"Failed to create config directory\"}");
				return;
			}
		}

		Serial.println("[AP] Saving configuration to SD card...");
		saveSuccess = atService_SD.saveConfigToJSON("/CONFIG/Config.json");

		if (saveSuccess)
		{
			Serial.println("[AP] Configuration saved successfully");
			server.send(200, "application/json", "{\"success\":true,\"message\":\"Configuration saved. Device will restart in 3 seconds...\"}");

			// Wait for response to be sent
			delay(500);

			// Restart ESP32
			Serial.println("[AP] Restarting ESP32...");
			delay(2000);
			ESP.restart();
		}
		else
		{
			Serial.println("[AP] Failed to save configuration to SD card");
			server.send(500, "application/json", "{\"success\":false,\"message\":\"Failed to save to SD card\"}");
		}
	}
	else
	{
		Serial.println("[AP] SD card not initialized");
		server.send(500, "application/json", "{\"success\":false,\"message\":\"SD card not available\"}");
	}
}

void App_AP::handleNotFound()
{
	sendCORS();
	String message = "{\"success\":false,\"message\":\"Endpoint not found\"}";
	server.send(404, "application/json", message);
}
void atApp_AP_Task_Func(void *parameter)
{
	while (1)
	{
		atApp_AP.Run_Application(APP_RUN_MODE_AUTO);
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}
#endif