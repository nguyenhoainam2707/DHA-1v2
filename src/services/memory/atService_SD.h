#ifndef _Service_atService_SD_
#define _Service_atService_SD_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "../src/services/Service.h"
#include "../src/obj/atObj_Param.h"
#include "../src/obj/atObj_Data.h"
#include <ArduinoJson.h>
#include <SD_MMC.h>
#include <FS.h>
/* _____DEFINATIONS__________________________________________________________ */
// Custom SDIO pin definitions for ESP32-S3
#define SD_CLK 12
#define SD_CMD 13
#define SD_D0 11
#define SD_D1 10
#define SD_D2 21
#define SD_D3 14
/* _____GLOBAL VARIABLES_____________________________________________________ */
SemaphoreHandle_t SD_mutex = NULL; // Mutex for SD bus access
/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */
/**
 * This Service class is the Service to manage the
 */
class Service_SD : public Service
{
public:
    Service_SD();
    ~Service_SD();

    void checkIn();
    void checkOut();
    // Constructor

    // Initialize and manage SD card with custom pins
    bool begin(uint8_t clk, uint8_t cmd, uint8_t d0,
               uint8_t d1 = -1, uint8_t d2 = -1, uint8_t d3 = -1,
               bool formatIfFailed = false);
    void end();
    bool isInitialized();

    // SD card information
    uint64_t getTotalSpace();
    uint64_t getUsedSpace();
    uint64_t getFreeSpace();
    String getCardType();
    void printCardInfo();

    // File management
    bool fileExists(const char *path);
    bool createFile(const char *path);
    bool deleteFile(const char *path);
    bool renameFile(const char *oldPath, const char *newPath);
    bool moveFile(const char *sourcePath, const char *destPath);
    bool copyFile(const char *sourcePath, const char *destPath);
    size_t getFileSize(const char *path);
    void listFiles(const char *path, String *result, uint16_t maxFiles);

    // Read/Write file
    String readFile(const char *path);
    bool writeFile(const char *path, const String &content, bool append = false);
    bool appendFile(const char *path, const String &content);

    // Read/Write binary file
    bool readBinaryFile(const char *path, uint8_t *buffer, size_t *size);
    bool writeBinaryFile(const char *path, const uint8_t *data, size_t size);

    // Directory management
    bool createDir(const char *path);
    bool removeDir(const char *path);
    bool dirExists(const char *path);
    void listDir(const char *dirname, uint8_t levels = 1);

    // Read file line by line
    bool readFileLines(const char *path, void (*callback)(const String &line));

    // Config JSON functions
    /**
     * @brief Save all configuration parameters from atObject_Param to JSON file
     * @param path Path to JSON file (e.g., "/CONFIG/Config.json")
     * @return true if save successful, false otherwise
     * @note This function will create the directory if it doesn't exist
     * @note All parameters including device info, sensors, RS485, MQTT, FTP, etc. will be saved
     */
    bool saveConfigToJSON(const char *path);

    /**
     * @brief Load configuration from JSON file and update atObject_Param variables
     * @param path Path to JSON file to read (e.g., "/CONFIG/Config.json")
     * @return true if load successful, false otherwise
     * @note After calling this function, all variables in atObject_Param will be updated
     * @note Only existing keys in JSON will be updated, missing keys keep their current values
     */
    bool loadConfigFromJSON(const char *path);

protected:
private:
    bool initialized;
    bool mode1Bit;
    static void Service_SD_Start();
    static void Service_SD_Execute();
    static void Service_SD_End();
} atService_SD;
/**
/**
 * This function will be automaticaly called when a object is created by this class
 */
Service_SD::Service_SD() : initialized(false), mode1Bit(false)
{
    _Start_User = *Service_SD_Start;
    _Execute_User = *Service_SD_Execute;
    _End_User = *Service_SD_End;

    // change the ID of Service
    ID_Service = 1;
    // change the Service name
    Name_Service = (char *)"SD Service";
}
/*
 * This function will be automaticaly called when the object of class is delete
 */
Service_SD::~Service_SD()
{
}
/**
 * This start function will init some critical function
 */
void Service_SD::Service_SD_Start()
{
    // Initialize SD bus
    if (SD_mutex == NULL)
    {
        SD_mutex = xSemaphoreCreateMutex();
    }
}

/**
 * Execute fuction of SNM app
 */
void Service_SD::Service_SD_Execute()
{
    // if(atService_SD.User_Mode == SER_USER_MODE_DEBUG)
    // {

    // }
}
void Service_SD::Service_SD_End() {}

void Service_SD::checkIn()
{
    xSemaphoreTake(SD_mutex, portMAX_DELAY); // Take the mutex to access SD bus
}

void Service_SD::checkOut()
{
    xSemaphoreGive(SD_mutex); // Release the mutex
}

// Initialize SD with custom pins
bool Service_SD::begin(uint8_t clk, uint8_t cmd, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, bool formatIfFailed)
{
    // Determine 1-bit or 4-bit mode
    this->mode1Bit = (d1 == 255 || d2 == 255 || d3 == 255);

    xSemaphoreTake(SD_mutex, portMAX_DELAY); // Take the mutex to access SD bus
    // Configure custom SDIO pins for ESP32-S3
    if (this->mode1Bit)
    {
        // 1-bit mode: only need CLK, CMD, D0
        Serial.println("[SD] Initializing SDIO 1-bit mode");
        if (!SD_MMC.setPins(clk, cmd, d0))
        {
            Serial.println("[SD] SDIO pin configuration error!");
            xSemaphoreGive(SD_mutex); // Release the mutex
            return false;
        }
    }
    else
    {
        // 4-bit mode: need full CLK, CMD, D0, D1, D2, D3
        Serial.println("[SD] Initializing SDIO 4-bit mode");
        if (!SD_MMC.setPins(clk, cmd, d0, d1, d2, d3))
        {
            Serial.println("[SD] SDIO pin configuration error!");
            xSemaphoreGive(SD_mutex); // Release the mutex
            return false;
        }
    }

    // Initialize SD_MMC
    if (!SD_MMC.begin("/sdcard", this->mode1Bit, formatIfFailed))
    {
        Serial.println("[SD] Initialization failed!");
        initialized = false;
        xSemaphoreGive(SD_mutex); // Release the mutex
        return false;
    }

    Serial.println("[SD] Initialization successful!");
    Serial.printf("[SD] Chế độ: %s\n", this->mode1Bit ? "1-bit" : "4-bit");
    Serial.printf("[SD] CLK: GPIO%d, CMD: GPIO%d, D0: GPIO%d\n", clk, cmd, d0);
    if (!this->mode1Bit)
    {
        Serial.printf("[SD] D1: GPIO%d, D2: GPIO%d, D3: GPIO%d\n", d1, d2, d3);
    }
    initialized = true;
    xSemaphoreGive(SD_mutex); // Release the mutex
    return true;
}

void Service_SD::end()
{
    xSemaphoreTake(SD_mutex, portMAX_DELAY); // Take the mutex to access SD bus
    SD_MMC.end();
    xSemaphoreGive(SD_mutex); // Release the mutex
    initialized = false;
}

bool Service_SD::isInitialized()
{
    return initialized;
}

uint64_t Service_SD::getTotalSpace()
{
    xSemaphoreTake(SD_mutex, portMAX_DELAY); // Take mutex before accessing SD
    uint64_t total = SD_MMC.totalBytes();
    xSemaphoreGive(SD_mutex); // Release mutex after accessing SD
    return total;
}

uint64_t Service_SD::getUsedSpace()
{
    xSemaphoreTake(SD_mutex, portMAX_DELAY); // Take mutex before accessing SD
    uint64_t used = SD_MMC.usedBytes();
    xSemaphoreGive(SD_mutex); // Release mutex after accessing SD
    return used;
}

uint64_t Service_SD::getFreeSpace()
{
    return getTotalSpace() - getUsedSpace();
}

String Service_SD::getCardType()
{
    xSemaphoreTake(SD_mutex, portMAX_DELAY); // Lấy mutex trước khi truy cập SD
    uint8_t cardType = SD_MMC.cardType();
    xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
    switch (cardType)
    {
    case CARD_MMC:
        return "MMC";
    case CARD_SD:
        return "SDSC";
    case CARD_SDHC:
        return "SDHC";
    default:
        return "UNKNOWN";
    }
}

void Service_SD::printCardInfo()
{
    if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.println("\n===== THÔNG TIN THẺ SD =====");
        Serial.printf("Loại thẻ: %s\n", getCardType().c_str());
        Serial.printf("Tổng dung lượng: %.2f MB\n", getTotalSpace() / (1024.0 * 1024.0));
        Serial.printf("Used: %.2f MB\n", getUsedSpace() / (1024.0 * 1024.0));
        Serial.printf("Còn trống: %.2f MB\n", getFreeSpace() / (1024.0 * 1024.0));
        Serial.println("============================\n");
    }
}

bool Service_SD::fileExists(const char *path)
{
    xSemaphoreTake(SD_mutex, portMAX_DELAY); // Lấy mutex trước khi truy cập SD
    bool exists = SD_MMC.exists(path);
    xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
    return exists;
}

bool Service_SD::createFile(const char *path)
{
    xSemaphoreTake(SD_mutex, portMAX_DELAY); // Lấy mutex trước khi truy cập SD
    File file = SD_MMC.open(path, FILE_WRITE);
    if (!file)
    {
        if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.printf("[SD] Cannot create file: %s\n", path);
        }
        xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
        return false;
    }
    file.close();
    xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
    return true;
}

bool Service_SD::deleteFile(const char *path)
{
    if (!fileExists(path))
    {
        if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.printf("[SD] File không tồn tại: %s\n", path);
        }
        return false;
    }

    xSemaphoreTake(SD_mutex, portMAX_DELAY); // Lấy mutex trước khi truy cập SD
    if (SD_MMC.remove(path))
    {
        if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.printf("[SD] Đã xóa file: %s\n", path);
        }
        xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
        return true;
    }

    if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.printf("[SD] Không thể xóa file: %s\n", path);
    }
    xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
    return false;
}

bool Service_SD::renameFile(const char *oldPath, const char *newPath)
{
    if (!fileExists(oldPath))
    {
        if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.printf("[SD] File không tồn tại: %s\n", oldPath);
        }
        return false;
    }

    xSemaphoreTake(SD_mutex, portMAX_DELAY); // Lấy mutex trước khi truy cập SD
    if (SD_MMC.rename(oldPath, newPath))
    {
        if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.printf("[SD] Đã đổi tên: %s -> %s\n", oldPath, newPath);
        }
        xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
        return true;
    }

    if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.printf("[SD] Không thể đổi tên file\n");
    }
    xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
    return false;
}

bool Service_SD::moveFile(const char *sourcePath, const char *destPath)
{
    if (!fileExists(sourcePath))
    {
        if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.printf("[SD] File nguồn không tồn tại: %s\n", sourcePath);
        }
        return false;
    }

    // Kiểm tra nếu file đích đã tồn tại
    if (fileExists(destPath))
    {
        if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.printf("[SD] File đích đã tồn tại: %s\n", destPath);
        }
        return false;
    }

    // Use rename to move file
    xSemaphoreTake(SD_mutex, portMAX_DELAY); // Lấy mutex trước khi truy cập SD
    if (SD_MMC.rename(sourcePath, destPath))
    {
        if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.printf("[SD] Đã di chuyển file: %s -> %s\n", sourcePath, destPath);
        }
        xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
        return true;
    }

    if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.printf("[SD] Không thể di chuyển file\n");
    }
    xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
    return false;
}

bool Service_SD::copyFile(const char *sourcePath, const char *destPath)
{
    if (!fileExists(sourcePath))
    {
        if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.printf("[SD] File nguồn không tồn tại: %s\n", sourcePath);
        }
        return false;
    }

    // Mở file nguồn để đọc
    xSemaphoreTake(SD_mutex, portMAX_DELAY); // Lấy mutex trước khi truy cập SD
    File sourceFile = SD_MMC.open(sourcePath, FILE_READ);
    if (!sourceFile)
    {
        if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.printf("[SD] Không thể mở file nguồn: %s\n", sourcePath);
        }
        xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
        return false;
    }

    // Mở file đích để ghi
    File destFile = SD_MMC.open(destPath, FILE_WRITE);
    if (!destFile)
    {
        if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.printf("[SD] Không thể tạo file đích: %s\n", destPath);
        }
        sourceFile.close();
        xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
        return false;
    }

    // Copy dữ liệu theo từng buffer để tiết kiệm RAM
    const size_t bufferSize = 512;
    uint8_t buffer[bufferSize];
    size_t totalBytes = 0;

    while (sourceFile.available())
    {
        size_t bytesRead = sourceFile.read(buffer, bufferSize);
        size_t bytesWritten = destFile.write(buffer, bytesRead);

        if (bytesRead != bytesWritten)
        {
            if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
            {
                Serial.printf("[SD] Lỗi khi copy dữ liệu\n");
            }
            sourceFile.close();
            destFile.close();
            SD_MMC.remove(destPath);  // Xóa file lỗi
            xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
            return false;
        }

        totalBytes += bytesWritten;
    }

    sourceFile.close();
    destFile.close();
    xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
    if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.printf("[SD] Đã copy file: %s -> %s (%d bytes)\n", sourcePath, destPath, totalBytes);
    }
    return true;
}

size_t Service_SD::getFileSize(const char *path)
{
    xSemaphoreTake(SD_mutex, portMAX_DELAY); // Lấy mutex trước khi truy cập SD
    File file = SD_MMC.open(path, FILE_READ);
    if (!file)
    {
        xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
        return 0;
    }

    size_t size = file.size();
    file.close();
    xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
    return size;
}

void Service_SD::listFiles(const char *path, String *result, uint16_t maxFiles)
{
    // String result;
    // result = new String[maxFiles];
    xSemaphoreTake(SD_mutex, portMAX_DELAY); // Lấy mutex trước khi truy cập SD
    File root = SD_MMC.open(path);
    if (!root)
    {
        if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.printf("[SD] Không thể mở thư mục: %s\n", path);
        }
        xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
    }

    if (!root.isDirectory())
    {
        if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.printf("[SD] Đây không phải là thư mục: %s\n", path);
        }
        root.close();
        xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
    }

    uint16_t fileCount = 0;
    File file = root.openNextFile();
    while (file)
    {
        if (!file.isDirectory())
        {
            // Kiểm tra giới hạn số file
            if (maxFiles > 0 && fileCount >= maxFiles)
            {
                file.close();
                break;
            }
            // Add element in to result
            result[fileCount] = String(file.name());
            fileCount++;
        }
        file.close();
        file = root.openNextFile();
    }
    xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
    root.close();
}

String Service_SD::readFile(const char *path)
{
    xSemaphoreTake(SD_mutex, portMAX_DELAY); // Lấy mutex trước khi truy cập SD
    File file = SD_MMC.open(path, FILE_READ);
    if (!file)
    {
        if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.printf("[SD] Không thể mở file để đọc: %s\n", path);
        }
        xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
        return "";
    }

    String content = "";
    while (file.available())
    {
        content += (char)file.read();
    }

    file.close();
    xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD

    if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.printf("[SD] Đã đọc file: %s (%d bytes)\n", path, content.length());
    }
    return content;
}

bool Service_SD::writeFile(const char *path, const String &content, bool append)
{
    // bool append = true -> ghi nối tiếp, false -> ghi đè
    xSemaphoreTake(SD_mutex, portMAX_DELAY); // Lấy mutex trước khi truy cập SD
    File file = SD_MMC.open(path, append ? FILE_APPEND : FILE_WRITE);
    if (!file)
    {
        if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.printf("[SD] Không thể mở file để ghi: %s\n", path);
        }
        xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
        return false;
    }

    size_t written = file.print(content);
    file.close();
    xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD

    if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.printf("[SD] Đã ghi file: %s (%d bytes)\n", path, written);
    }
    return written == content.length();
}

bool Service_SD::appendFile(const char *path, const String &content)
{
    return writeFile(path, content, true);
}

bool Service_SD::readBinaryFile(const char *path, uint8_t *buffer, size_t *size)
{
    xSemaphoreTake(SD_mutex, portMAX_DELAY); // Lấy mutex trước khi truy cập SD
    File file = SD_MMC.open(path, FILE_READ);
    if (!file)
    {
        if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.printf("[SD] Không thể mở file nhị phân: %s\n", path);
        }
        xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
        return false;
    }

    *size = file.size();
    file.read(buffer, *size);
    file.close();
    xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD

    if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.printf("[SD] Đã đọc file nhị phân: %s (%d bytes)\n", path, *size);
    }
    return true;
}

bool Service_SD::writeBinaryFile(const char *path, const uint8_t *data, size_t size)
{
    xSemaphoreTake(SD_mutex, portMAX_DELAY); // Lấy mutex trước khi truy cập SD
    File file = SD_MMC.open(path, FILE_WRITE);
    if (!file)
    {
        if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.printf("[SD] Không thể ghi file nhị phân: %s\n", path);
        }
        xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
        return false;
    }

    size_t written = file.write(data, size);
    file.close();
    xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD

    if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.printf("[SD] Đã ghi file nhị phân: %s (%d bytes)\n", path, written);
    }
    return written == size;
}

bool Service_SD::createDir(const char *path)
{
    xSemaphoreTake(SD_mutex, portMAX_DELAY); // Lấy mutex trước khi truy cập SD

    // Kiểm tra nếu thư mục đã tồn tại
    if (SD_MMC.exists(path))
    {
        if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.printf("[SD] Thư mục đã tồn tại: %s\n", path);
        }
        xSemaphoreGive(SD_mutex);
        return true;
    }

    // Tạo từng thư mục con trong đường dẫn
    String fullPath = String(path);
    String currentPath = "";

    // Xử lý đường dẫn bắt đầu bằng "/"
    if (fullPath.startsWith("/"))
    {
        currentPath = "/";
    }

    // Tách các phần của đường dẫn
    int startIndex = (fullPath.startsWith("/")) ? 1 : 0;
    int endIndex = 0;

    while (endIndex < fullPath.length())
    {
        endIndex = fullPath.indexOf('/', startIndex);

        if (endIndex == -1)
        {
            // Đây là phần cuối cùng của đường dẫn
            currentPath += fullPath.substring(startIndex);
        }
        else
        {
            // Thêm phần đường dẫn hiện tại
            currentPath += fullPath.substring(startIndex, endIndex);
        }

        // Tạo thư mục nếu chưa tồn tại
        if (!SD_MMC.exists(currentPath.c_str()))
        {
            if (!SD_MMC.mkdir(currentPath.c_str()))
            {
                if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
                {
                    Serial.printf("[SD] Không thể tạo thư mục: %s\n", currentPath.c_str());
                }
                xSemaphoreGive(SD_mutex);
                return false;
            }
            else
            {
                if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
                {
                    Serial.printf("[SD] Đã tạo thư mục: %s\n", currentPath.c_str());
                }
            }
        }

        // Thêm dấu "/" để tiếp tục (trừ phần cuối cùng)
        if (endIndex != -1)
        {
            currentPath += "/";
            startIndex = endIndex + 1;
        }
        else
        {
            break;
        }
    }

    xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
    return true;
}

bool Service_SD::removeDir(const char *path)
{
    xSemaphoreTake(SD_mutex, portMAX_DELAY); // Lấy mutex trước khi truy cập SD
    if (SD_MMC.rmdir(path))
    {
        if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.printf("[SD] Đã xóa thư mục: %s\n", path);
        }
        xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
        return true;
    }
    if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.printf("[SD] Không thể xóa thư mục: %s\n", path);
    }
    xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
    return false;
}

bool Service_SD::dirExists(const char *path)
{
    xSemaphoreTake(SD_mutex, portMAX_DELAY); // Lấy mutex trước khi truy cập SD
    File dir = SD_MMC.open(path);
    if (!dir)
    {
        xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
        return false;
    }

    bool isDir = dir.isDirectory();
    dir.close();
    xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
    return isDir;
}

void Service_SD::listDir(const char *dirname, uint8_t levels)
{
    xSemaphoreTake(SD_mutex, portMAX_DELAY); // Lấy mutex trước khi truy cập SD
    if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.printf("[SD] Liệt kê thư mục: %s\n", dirname);
    }

    File root = SD_MMC.open(dirname);
    if (!root)
    {
        if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("[SD] Không thể mở thư mục");
        }
        xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
        return;
    }

    if (!root.isDirectory())
    {
        if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("[SD] Không phải thư mục");
        }
        root.close();
        xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
        return;
    }

    File file = root.openNextFile();
    while (file)
    {
        for (uint8_t i = 0; i < levels; i++)
        {
            if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
            {
                Serial.print("  ");
            }
        }

        if (file.isDirectory())
        {
            if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
            {
                Serial.printf("[DIR]  %s\n", file.name());
            }
            if (levels)
            {
                xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
                listDir(file.path(), levels - 1);
            }
        }
        else
        {
            if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
            {
                Serial.printf("[FILE] %s (%d bytes)\n", file.name(), file.size());
            }
        }

        file = root.openNextFile();
    }
    root.close();
    xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
}

bool Service_SD::readFileLines(const char *path, void (*callback)(const String &line))
{
    xSemaphoreTake(SD_mutex, portMAX_DELAY); // Lấy mutex trước khi truy cập SD
    File file = SD_MMC.open(path, FILE_READ);
    if (!file)
    {
        if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.printf("[SD] Không thể mở file: %s\n", path);
        }
        xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
        return false;
    }

    String line = "";
    while (file.available())
    {
        char c = file.read();
        if (c == '\n')
        {
            callback(line);
            line = "";
        }
        else if (c != '\r')
        {
            line += c;
        }
    }

    if (line.length() > 0)
    {
        callback(line);
    }

    file.close();
    xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
    return true;
}

// Helper function to convert serial config to readable string
String serialConfigToString(uint32_t config)
{
    switch (config)
    {
    case SERIAL_5N1:
        return "SERIAL_5N1";
    case SERIAL_6N1:
        return "SERIAL_6N1";
    case SERIAL_7N1:
        return "SERIAL_7N1";
    case SERIAL_8N1:
        return "SERIAL_8N1";
    case SERIAL_5N2:
        return "SERIAL_5N2";
    case SERIAL_6N2:
        return "SERIAL_6N2";
    case SERIAL_7N2:
        return "SERIAL_7N2";
    case SERIAL_8N2:
        return "SERIAL_8N2";
    case SERIAL_5E1:
        return "SERIAL_5E1";
    case SERIAL_6E1:
        return "SERIAL_6E1";
    case SERIAL_7E1:
        return "SERIAL_7E1";
    case SERIAL_8E1:
        return "SERIAL_8E1";
    case SERIAL_5E2:
        return "SERIAL_5E2";
    case SERIAL_6E2:
        return "SERIAL_6E2";
    case SERIAL_7E2:
        return "SERIAL_7E2";
    case SERIAL_8E2:
        return "SERIAL_8E2";
    case SERIAL_5O1:
        return "SERIAL_5O1";
    case SERIAL_6O1:
        return "SERIAL_6O1";
    case SERIAL_7O1:
        return "SERIAL_7O1";
    case SERIAL_8O1:
        return "SERIAL_8O1";
    case SERIAL_5O2:
        return "SERIAL_5O2";
    case SERIAL_6O2:
        return "SERIAL_6O2";
    case SERIAL_7O2:
        return "SERIAL_7O2";
    case SERIAL_8O2:
        return "SERIAL_8O2";
    default:
        return "SERIAL_8N1";
    }
}

// Helper function to convert string back to serial config
uint32_t stringToSerialConfig(const String &configStr)
{
    if (configStr == "SERIAL_5N1")
        return SERIAL_5N1;
    if (configStr == "SERIAL_6N1")
        return SERIAL_6N1;
    if (configStr == "SERIAL_7N1")
        return SERIAL_7N1;
    if (configStr == "SERIAL_8N1")
        return SERIAL_8N1;
    if (configStr == "SERIAL_5N2")
        return SERIAL_5N2;
    if (configStr == "SERIAL_6N2")
        return SERIAL_6N2;
    if (configStr == "SERIAL_7N2")
        return SERIAL_7N2;
    if (configStr == "SERIAL_8N2")
        return SERIAL_8N2;
    if (configStr == "SERIAL_5E1")
        return SERIAL_5E1;
    if (configStr == "SERIAL_6E1")
        return SERIAL_6E1;
    if (configStr == "SERIAL_7E1")
        return SERIAL_7E1;
    if (configStr == "SERIAL_8E1")
        return SERIAL_8E1;
    if (configStr == "SERIAL_5E2")
        return SERIAL_5E2;
    if (configStr == "SERIAL_6E2")
        return SERIAL_6E2;
    if (configStr == "SERIAL_7E2")
        return SERIAL_7E2;
    if (configStr == "SERIAL_8E2")
        return SERIAL_8E2;
    if (configStr == "SERIAL_5O1")
        return SERIAL_5O1;
    if (configStr == "SERIAL_6O1")
        return SERIAL_6O1;
    if (configStr == "SERIAL_7O1")
        return SERIAL_7O1;
    if (configStr == "SERIAL_8O1")
        return SERIAL_8O1;
    if (configStr == "SERIAL_5O2")
        return SERIAL_5O2;
    if (configStr == "SERIAL_6O2")
        return SERIAL_6O2;
    if (configStr == "SERIAL_7O2")
        return SERIAL_7O2;
    if (configStr == "SERIAL_8O2")
        return SERIAL_8O2;
    return SERIAL_8N1; // default
}

bool Service_SD::saveConfigToJSON(const char *path)
{
    // Include atObj_Param
    extern Object_Param atObject_Param;
    if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.printf("[SD] saveConfigToJSON() called with path: %s\n", path);
        Serial.printf("[SD] SD initialized: %s\n", initialized ? "YES" : "NO");
    }
    if (!initialized)
    {
        if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.println("[SD] ERROR: SD card not initialized!");
        }
        return false;
    }

    if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.printf("[SD] Đang tạo file JSON config: %s\n", path);
    }

    // Tạo JSON document với kích thước đủ lớn (không cần mutex ở đây vì chỉ build JSON trong RAM)
    JsonDocument doc; // ArduinoJson v7 - auto memory management

    // Device Info
    JsonObject device = doc["device_info"].to<JsonObject>();
    device["device_name"] = atObject_Param.device_name;
    device["device_id"] = atObject_Param.device_id;
    device["sub_id"] = atObject_Param.sub_id;
    device["location_X"] = atObject_Param.location_X;
    device["location_Y"] = atObject_Param.location_Y;
    device["area"] = atObject_Param.area;
    device["device_type"] = atObject_Param.device_type;

    // LCD Config
    JsonObject lcd = doc["lcd_config"].to<JsonObject>();
    lcd["enable"] = atObject_Param.lcd_enable;
    lcd["backlight_enable"] = atObject_Param.lcd_backlight_enable;
    lcd["refresh_rate"] = atObject_Param.lcd_refresh_rate;

    // AI Config
    JsonObject ai = doc["ai_config"].to<JsonObject>();
    JsonArray ai_enable = ai["enable_list"].to<JsonArray>();
    for (int i = 0; i < MAX_AI_CHANNEL; i++)
    {
        ai_enable.add(atObject_Param.ai_enable_list[i]);
    }
    ai["refresh_rate"] = atObject_Param.ai_refresh_rate;

    // DO Config
    JsonObject doObj = doc["do_config"].to<JsonObject>();
    JsonArray do_enable = doObj["enable_list"].to<JsonArray>();
    for (int i = 0; i < MAX_DO_CHANNEL; i++)
    {
        do_enable.add(atObject_Param.do_enable_list[i]);
    }
    doObj["refresh_rate"] = atObject_Param.do_refresh_rate;
    
    // DO Protocol list
    JsonArray do_protocol = doObj["protocol_list"].to<JsonArray>();
    for (int i = 0; i < MAX_DO_CHANNEL; i++)
    {
        do_protocol.add(atObject_Param.do_protocol_list[i]);
    }
    
    // DO Schmitt Trigger Config
    JsonArray do_schmitt_sensor = doObj["schmitt_trigger_sensor_list"].to<JsonArray>();
    JsonArray do_schmitt_valuetype = doObj["schmitt_trigger_valuetype_list"].to<JsonArray>();
    JsonArray do_schmitt_upper = doObj["schmitt_trigger_upper_threshold_list"].to<JsonArray>();
    JsonArray do_schmitt_lower = doObj["schmitt_trigger_lower_threshold_list"].to<JsonArray>();
    for (int i = 0; i < MAX_DO_CHANNEL; i++)
    {
        do_schmitt_sensor.add(atObject_Param.do_control_schmitt_trigger_sensor_list[i]);
        do_schmitt_valuetype.add(atObject_Param.do_control_schmitt_trigger_sensor_valuetype_list[i]);
        do_schmitt_upper.add(atObject_Param.do_schmitt_trigger_upper_threshold_list[i]);
        do_schmitt_lower.add(atObject_Param.do_schmitt_trigger_lower_threshold_list[i]);
    }
    
    // DO Control by Sensor State Config
    JsonArray do_control_sensor = doObj["control_sensor_list"].to<JsonArray>();
    JsonArray do_control_state = doObj["control_sensor_state_list"].to<JsonArray>();
    for (int i = 0; i < MAX_DO_CHANNEL; i++)
    {
        do_control_sensor.add(atObject_Param.do_control_sensor_list[i]);
        do_control_state.add(atObject_Param.do_control_sensor_state_list[i]);
    }
    
    // DO Control by Digital Input
    JsonArray do_control_di = doObj["control_input_digital_list"].to<JsonArray>();
    for (int i = 0; i < MAX_DO_CHANNEL; i++)
    {
        do_control_di.add(atObject_Param.do_control_input_digital_list[i]);
    }

    // DI Config
    JsonObject di = doc["di_config"].to<JsonObject>();
    JsonArray di_enable = di["enable_list"].to<JsonArray>();
    for (int i = 0; i < MAX_DI_CHANNEL; i++)
    {
        di_enable.add(atObject_Param.di_enable_list[i]);
    }
    di["refresh_rate"] = atObject_Param.di_refresh_rate;
    
    // DI Control Sensor State Config
    JsonArray di_control_enable = di["control_sensor_state_enable_list"].to<JsonArray>();
    JsonArray di_control_sensor = di["control_sensor_list"].to<JsonArray>();
    JsonArray di_control_state = di["control_sensor_state_list"].to<JsonArray>();
    for (int i = 0; i < MAX_DI_CHANNEL; i++)
    {
        di_control_enable.add(atObject_Param.di_control_sensor_state_enable_list[i]);
        di_control_sensor.add(atObject_Param.di_control_sensor_list[i]);
        di_control_state.add(atObject_Param.di_control_sensor_state_list[i]);
    }

    // RS485 Config
    JsonObject rs485 = doc["rs485_config"].to<JsonObject>();
    JsonArray rs485_enable = rs485["enable"].to<JsonArray>();
    for (int i = 0; i < MAX_RS485_CHANNEL; i++)
    {
        rs485_enable.add(atObject_Param.rs485_enable[i]);
    }

    // RS485 Slave Config cho từng channel
    JsonArray rs485_slaves = rs485["slaves"].to<JsonArray>();
    for (int ch = 0; ch < MAX_RS485_CHANNEL; ch++)
    {
        JsonObject channel = rs485_slaves.add<JsonObject>();
        channel["channel"] = ch;

        JsonArray enable_list = channel["enable_list"].to<JsonArray>();
        JsonArray baudrate_list = channel["baudrate_list"].to<JsonArray>();
        JsonArray config_list = channel["serial_config_list"].to<JsonArray>();
        JsonArray id_list = channel["id_list"].to<JsonArray>();
        JsonArray timeout_list = channel["timeout_list"].to<JsonArray>();
        JsonArray functioncode_list = channel["functioncode_list"].to<JsonArray>();
        JsonArray address_list = channel["starting_address_list"].to<JsonArray>();
        JsonArray quantity_list = channel["quantity_list"].to<JsonArray>();
        JsonArray calib_enable_list = channel["calib_state_reading_enable"].to<JsonArray>();
        JsonArray calib_fc_list = channel["calib_state_function_code"].to<JsonArray>();
        JsonArray calib_addr_list = channel["calib_state_address"].to<JsonArray>();
        JsonArray calib_value_list = channel["calib_state_value"].to<JsonArray>();
        JsonArray error_enable_list = channel["error_state_reading_enable"].to<JsonArray>();
        JsonArray error_fc_list = channel["error_state_function_code"].to<JsonArray>();
        JsonArray error_addr_list = channel["error_state_address"].to<JsonArray>();
        JsonArray error_value_list = channel["error_state_value"].to<JsonArray>();

        for (int i = 0; i < MAX_SENSORS; i++)
        {
            enable_list.add(atObject_Param.rs485_slave_enable_list[ch][i]);
            baudrate_list.add(atObject_Param.rs485_slave_baudrate_list[ch][i]);
            config_list.add(serialConfigToString(atObject_Param.rs485_slave_serial_config_list[ch][i]));
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

    // Sensor Config
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
        sensor["state_protocol"] = atObject_Param.sensor_state_protocol_list[i];
        sensor["data_type"] = atObject_Param.sensor_data_type_list[i];
        sensor["read_interval"] = atObject_Param.sensor_read_interval_list[i];

        // RS485 index cho từng channel
        JsonArray rs485_idx = sensor["rs485_index"].to<JsonArray>();
        for (int ch = 0; ch < MAX_RS485_CHANNEL; ch++)
        {
            rs485_idx.add(atObject_Param.sensor_rs485_index[ch][i]);
        }

        // Calibration config
        JsonObject calib = sensor["calibration"].to<JsonObject>();
        calib["function_order"] = atObject_Param.sensor_calib_function_order_list[i];
        calib["index_A"] = atObject_Param.sensor_calib_function_index_list[i].index_A;
        calib["index_B"] = atObject_Param.sensor_calib_function_index_list[i].index_B;
        calib["index_C"] = atObject_Param.sensor_calib_function_index_list[i].index_C;
        calib["index_D"] = atObject_Param.sensor_calib_function_index_list[i].index_D;

        // Calib table points
        JsonArray table_points = calib["table_points"].to<JsonArray>();
        for (int j = 0; j < MAX_CALIB_TABLE_POINTS; j++)
        {
            JsonObject point = table_points.add<JsonObject>();
            point["raw_value"] = atObject_Param.sensor_calib_table_list[i][j].raw_value;
            point["calib_value"] = atObject_Param.sensor_calib_table_list[i][j].calib_value;
            point["active"] = atObject_Param.sensor_calib_table_point_active_list[i][j];
        }

        // Error config
        JsonObject error = sensor["error"].to<JsonObject>();
        error["enable"] = atObject_Param.sensor_error_enable_list[i];
        error["raw_lower"] = atObject_Param.sensor_raw_error_lower_threshold_list[i];
        error["raw_upper"] = atObject_Param.sensor_raw_error_upper_threshold_list[i];
        error["calib_lower"] = atObject_Param.sensor_calib_error_lower_threshold_list[i];
        error["calib_upper"] = atObject_Param.sensor_calib_error_upper_threshold_list[i];

        // Limit config
        JsonObject limit = sensor["limit"].to<JsonObject>();
        limit["enable"] = atObject_Param.sensor_limit_enable_list[i];
        limit["calib_lower"] = atObject_Param.sensor_calib_lower_limit_threshold_list[i];
        limit["calib_upper"] = atObject_Param.sensor_calib_upper_limit_threshold_list[i];
        limit["lower_noise"] = atObject_Param.sensor_lower_limit_noise_list[i];
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

        // Sensor config for this FTP server
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

    // Tạo thư mục nếu chưa có (sử dụng mutex)
    String pathStr = String(path);
    int lastSlash = pathStr.lastIndexOf('/');
    if (lastSlash > 0)
    {
        String dirPath = pathStr.substring(0, lastSlash);
        createDir(dirPath.c_str()); // createDir đã có mutex bên trong
    }

    // Serialize JSON to file với mutex
    xSemaphoreTake(SD_mutex, portMAX_DELAY); // Lấy mutex trước khi truy cập SD
    File file = SD_MMC.open(path, FILE_WRITE);
    if (!file)
    {
        if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.printf("[SD] Không thể tạo file JSON: %s\n", path);
        }
        xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
        return false;
    }

    size_t bytesWritten = serializeJsonPretty(doc, file);
    file.close();
    xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD

    if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.printf("[SD] Đã lưu config JSON: %s (%d bytes)\n", path, bytesWritten);
    }

    return bytesWritten > 0;
}

bool Service_SD::loadConfigFromJSON(const char *path)
{
    // Include atObj_Param
    extern Object_Param atObject_Param;

    if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.printf("[SD] Đang đọc file JSON config: %s\n", path);
    }

    // Kiểm tra file có tồn tại không trước
    if (!fileExists(path))
    {
        if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.printf("[SD] File JSON không tồn tại: %s\n", path);
        }
        return false;
    }

    // Đọc file JSON với mutex
    xSemaphoreTake(SD_mutex, portMAX_DELAY); // Lấy mutex trước khi truy cập SD
    File file = SD_MMC.open(path, FILE_READ);
    if (!file)
    {
        if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.printf("[SD] Không thể mở file JSON: %s\n", path);
        }
        xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD
        return false;
    }

    // Đọc toàn bộ nội dung file
    size_t fileSize = file.size();
    if (fileSize == 0)
    {
        if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.printf("[SD] File JSON rỗng: %s\n", path);
        }
        file.close();
        xSemaphoreGive(SD_mutex);
        return false;
    }

    String jsonContent = "";
    jsonContent.reserve(fileSize + 1); // Pre-allocate memory

    while (file.available())
    {
        jsonContent += (char)file.read();
    }
    file.close();
    xSemaphoreGive(SD_mutex); // Trả mutex sau khi truy cập SD

    if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.printf("[SD] Đã đọc file JSON: %d bytes\n", jsonContent.length());
    }

    // Parse JSON (không cần mutex vì thao tác trong RAM)
    JsonDocument doc; // ArduinoJson v7 - auto memory management
    DeserializationError error = deserializeJson(doc, jsonContent);

    if (error)
    {
        if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
        {
            Serial.printf("[SD] Lỗi parse JSON: %s\n", error.c_str());
        }
        return false;
    }

    // Device Info
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
    }

    // LCD Config
    if (!doc["lcd_config"].isNull())
    {
        JsonObject lcd = doc["lcd_config"];
        if (!lcd["enable"].isNull())
            atObject_Param.lcd_enable = lcd["enable"];
        if (!lcd["backlight_enable"].isNull())
            atObject_Param.lcd_backlight_enable = lcd["backlight_enable"];
        if (!lcd["refresh_rate"].isNull())
            atObject_Param.lcd_refresh_rate = lcd["refresh_rate"];
    }

    // AI Config
    if (!doc["ai_config"].isNull())
    {
        JsonObject ai = doc["ai_config"];
        if (!ai["enable_list"].isNull())
        {
            JsonArray ai_enable = ai["enable_list"];
            for (int i = 0; i < MAX_AI_CHANNEL && i < ai_enable.size(); i++)
            {
                atObject_Param.ai_enable_list[i] = ai_enable[i];
            }
        }
        if (!ai["refresh_rate"].isNull())
            atObject_Param.ai_refresh_rate = ai["refresh_rate"];
    }

    // DO Config
    if (!doc["do_config"].isNull())
    {
        JsonObject doObj = doc["do_config"];
        if (!doObj["enable_list"].isNull())
        {
            JsonArray do_enable = doObj["enable_list"];
            for (int i = 0; i < MAX_DO_CHANNEL && i < do_enable.size(); i++)
            {
                atObject_Param.do_enable_list[i] = do_enable[i];
            }
        }
        if (!doObj["refresh_rate"].isNull())
            atObject_Param.do_refresh_rate = doObj["refresh_rate"];
        
        // DO Protocol list
        if (!doObj["protocol_list"].isNull())
        {
            JsonArray do_protocol = doObj["protocol_list"];
            for (int i = 0; i < MAX_DO_CHANNEL && i < do_protocol.size(); i++)
            {
                atObject_Param.do_protocol_list[i] = do_protocol[i];
            }
        }
        
        // DO Schmitt Trigger Config
        if (!doObj["schmitt_trigger_sensor_list"].isNull())
        {
            JsonArray do_schmitt_sensor = doObj["schmitt_trigger_sensor_list"];
            for (int i = 0; i < MAX_DO_CHANNEL && i < do_schmitt_sensor.size(); i++)
            {
                atObject_Param.do_control_schmitt_trigger_sensor_list[i] = do_schmitt_sensor[i];
            }
        }
        if (!doObj["schmitt_trigger_valuetype_list"].isNull())
        {
            JsonArray do_schmitt_valuetype = doObj["schmitt_trigger_valuetype_list"];
            for (int i = 0; i < MAX_DO_CHANNEL && i < do_schmitt_valuetype.size(); i++)
            {
                atObject_Param.do_control_schmitt_trigger_sensor_valuetype_list[i] = do_schmitt_valuetype[i];
            }
        }
        if (!doObj["schmitt_trigger_upper_threshold_list"].isNull())
        {
            JsonArray do_schmitt_upper = doObj["schmitt_trigger_upper_threshold_list"];
            for (int i = 0; i < MAX_DO_CHANNEL && i < do_schmitt_upper.size(); i++)
            {
                atObject_Param.do_schmitt_trigger_upper_threshold_list[i] = do_schmitt_upper[i];
            }
        }
        if (!doObj["schmitt_trigger_lower_threshold_list"].isNull())
        {
            JsonArray do_schmitt_lower = doObj["schmitt_trigger_lower_threshold_list"];
            for (int i = 0; i < MAX_DO_CHANNEL && i < do_schmitt_lower.size(); i++)
            {
                atObject_Param.do_schmitt_trigger_lower_threshold_list[i] = do_schmitt_lower[i];
            }
        }
        
        // DO Control by Sensor State Config
        if (!doObj["control_sensor_list"].isNull())
        {
            JsonArray do_control_sensor = doObj["control_sensor_list"];
            for (int i = 0; i < MAX_DO_CHANNEL && i < do_control_sensor.size(); i++)
            {
                atObject_Param.do_control_sensor_list[i] = do_control_sensor[i];
            }
        }
        if (!doObj["control_sensor_state_list"].isNull())
        {
            JsonArray do_control_state = doObj["control_sensor_state_list"];
            for (int i = 0; i < MAX_DO_CHANNEL && i < do_control_state.size(); i++)
            {
                atObject_Param.do_control_sensor_state_list[i] = do_control_state[i];
            }
        }
        
        // DO Control by Digital Input
        if (!doObj["control_input_digital_list"].isNull())
        {
            JsonArray do_control_di = doObj["control_input_digital_list"];
            for (int i = 0; i < MAX_DO_CHANNEL && i < do_control_di.size(); i++)
            {
                atObject_Param.do_control_input_digital_list[i] = do_control_di[i];
            }
        }
    }

    // DI Config
    if (!doc["di_config"].isNull())
    {
        JsonObject di = doc["di_config"];
        if (!di["enable_list"].isNull())
        {
            JsonArray di_enable = di["enable_list"];
            for (int i = 0; i < MAX_DI_CHANNEL && i < di_enable.size(); i++)
            {
                atObject_Param.di_enable_list[i] = di_enable[i];
            }
        }
        if (!di["refresh_rate"].isNull())
            atObject_Param.di_refresh_rate = di["refresh_rate"];
        
        // DI Control Sensor State Config
        if (!di["control_sensor_state_enable_list"].isNull())
        {
            JsonArray di_control_enable = di["control_sensor_state_enable_list"];
            for (int i = 0; i < MAX_DI_CHANNEL && i < di_control_enable.size(); i++)
            {
                atObject_Param.di_control_sensor_state_enable_list[i] = di_control_enable[i];
            }
        }
        if (!di["control_sensor_list"].isNull())
        {
            JsonArray di_control_sensor = di["control_sensor_list"];
            for (int i = 0; i < MAX_DI_CHANNEL && i < di_control_sensor.size(); i++)
            {
                atObject_Param.di_control_sensor_list[i] = di_control_sensor[i];
            }
        }
        if (!di["control_sensor_state_list"].isNull())
        {
            JsonArray di_control_state = di["control_sensor_state_list"];
            for (int i = 0; i < MAX_DI_CHANNEL && i < di_control_state.size(); i++)
            {
                atObject_Param.di_control_sensor_state_list[i] = di_control_state[i];
            }
        }
    }

    // RS485 Config
    if (!doc["rs485_config"].isNull())
    {
        JsonObject rs485 = doc["rs485_config"];

        if (!rs485["enable"].isNull())
        {
            JsonArray rs485_enable = rs485["enable"];
            for (int i = 0; i < MAX_RS485_CHANNEL && i < rs485_enable.size(); i++)
            {
                atObject_Param.rs485_enable[i] = rs485_enable[i];
            }
        }

        if (!rs485["slaves"].isNull())
        {
            JsonArray rs485_slaves = rs485["slaves"];
            for (int ch = 0; ch < MAX_RS485_CHANNEL && ch < rs485_slaves.size(); ch++)
            {
                JsonObject channel = rs485_slaves[ch];

                if (!channel["enable_list"].isNull())
                {
                    JsonArray enable_list = channel["enable_list"];
                    for (int i = 0; i < MAX_SENSORS && i < enable_list.size(); i++)
                    {
                        atObject_Param.rs485_slave_enable_list[ch][i] = enable_list[i];
                    }
                }

                if (!channel["baudrate_list"].isNull())
                {
                    JsonArray baudrate_list = channel["baudrate_list"];
                    for (int i = 0; i < MAX_SENSORS && i < baudrate_list.size(); i++)
                    {
                        atObject_Param.rs485_slave_baudrate_list[ch][i] = baudrate_list[i];
                    }
                }

                if (!channel["serial_config_list"].isNull())
                {
                    JsonArray config_list = channel["serial_config_list"];
                    for (int i = 0; i < MAX_SENSORS && i < config_list.size(); i++)
                    {
                        atObject_Param.rs485_slave_serial_config_list[ch][i] = stringToSerialConfig(config_list[i].as<String>());
                    }
                }

                if (!channel["id_list"].isNull())
                {
                    JsonArray id_list = channel["id_list"];
                    for (int i = 0; i < MAX_SENSORS && i < id_list.size(); i++)
                    {
                        atObject_Param.rs485_slave_id_list[ch][i] = id_list[i];
                    }
                }

                if (!channel["timeout_list"].isNull())
                {
                    JsonArray timeout_list = channel["timeout_list"];
                    for (int i = 0; i < MAX_SENSORS && i < timeout_list.size(); i++)
                    {
                        atObject_Param.rs485_slave_timeout_list[ch][i] = timeout_list[i];
                    }
                }

                if (!channel["functioncode_list"].isNull())
                {
                    JsonArray functioncode_list = channel["functioncode_list"];
                    for (int i = 0; i < MAX_SENSORS && i < functioncode_list.size(); i++)
                    {
                        atObject_Param.rs485_slave_function_code_list[ch][i] = functioncode_list[i];
                    }
                }

                if (!channel["starting_address_list"].isNull())
                {
                    JsonArray address_list = channel["starting_address_list"];
                    for (int i = 0; i < MAX_SENSORS && i < address_list.size(); i++)
                    {
                        atObject_Param.rs485_slave_starting_address_list[ch][i] = address_list[i];
                    }
                }

                if (!channel["quantity_list"].isNull())
                {
                    JsonArray quantity_list = channel["quantity_list"];
                    for (int i = 0; i < MAX_SENSORS && i < quantity_list.size(); i++)
                    {
                        atObject_Param.rs485_slave_quantity_list[ch][i] = quantity_list[i];
                    }
                }

                if (!channel["calib_state_reading_enable"].isNull())
                {
                    JsonArray calib_enable_list = channel["calib_state_reading_enable"];
                    for (int i = 0; i < MAX_SENSORS && i < calib_enable_list.size(); i++)
                    {
                        atObject_Param.rs485_slave_calib_state_reading_enable_list[ch][i] = calib_enable_list[i];
                    }
                }

                if (!channel["calib_state_function_code"].isNull())
                {
                    JsonArray calib_fc_list = channel["calib_state_function_code"];
                    for (int i = 0; i < MAX_SENSORS && i < calib_fc_list.size(); i++)
                    {
                        atObject_Param.rs485_slave_calib_state_function_code_list[ch][i] = calib_fc_list[i];
                    }
                }

                if (!channel["calib_state_address"].isNull())
                {
                    JsonArray calib_addr_list = channel["calib_state_address"];
                    for (int i = 0; i < MAX_SENSORS && i < calib_addr_list.size(); i++)
                    {
                        atObject_Param.rs485_slave_state_calib_address_list[ch][i] = calib_addr_list[i];
                    }
                }

                if (!channel["calib_state_value"].isNull())
                {
                    JsonArray calib_value_list = channel["calib_state_value"];
                    for (int i = 0; i < MAX_SENSORS && i < calib_value_list.size(); i++)
                    {
                        atObject_Param.rs485_slave_state_calib_value_list[ch][i] = calib_value_list[i];
                    }
                }

                if (!channel["error_state_reading_enable"].isNull())
                {
                    JsonArray error_enable_list = channel["error_state_reading_enable"];
                    for (int i = 0; i < MAX_SENSORS && i < error_enable_list.size(); i++)
                    {
                        atObject_Param.rs485_slave_error_state_reading_enable_list[ch][i] = error_enable_list[i];
                    }
                }

                if (!channel["error_state_function_code"].isNull())
                {
                    JsonArray error_fc_list = channel["error_state_function_code"];
                    for (int i = 0; i < MAX_SENSORS && i < error_fc_list.size(); i++)
                    {
                        atObject_Param.rs485_slave_error_state_function_code_list[ch][i] = error_fc_list[i];
                    }
                }

                if (!channel["error_state_address"].isNull())
                {
                    JsonArray error_addr_list = channel["error_state_address"];
                    for (int i = 0; i < MAX_SENSORS && i < error_addr_list.size(); i++)
                    {
                        atObject_Param.rs485_slave_state_error_address_list[ch][i] = error_addr_list[i];
                    }
                }

                if (!channel["error_state_value"].isNull())
                {
                    JsonArray error_value_list = channel["error_state_value"];
                    for (int i = 0; i < MAX_SENSORS && i < error_value_list.size(); i++)
                    {
                        atObject_Param.rs485_slave_state_error_value_list[ch][i] = error_value_list[i];
                    }
                }
            }
        }
    }

    // Sensor Config
    if (!doc["sensor_config"].isNull())
    {
        JsonObject sensors = doc["sensor_config"];
        if (!sensors["sensors"].isNull())
        {
            JsonArray sensor_list = sensors["sensors"];
            for (int i = 0; i < MAX_SENSORS && i < sensor_list.size(); i++)
            {
                JsonObject sensor = sensor_list[i];

                if (!sensor["enable"].isNull())
                    atObject_Param.sensor_enable_list[i] = sensor["enable"];
                if (!sensor["name"].isNull())
                    atObject_Param.sensor_name_list[i] = sensor["name"].as<String>();
                if (!sensor["unit"].isNull())
                    atObject_Param.sensor_unit_list[i] = sensor["unit"].as<String>();
                if (!sensor["protocol"].isNull())
                    atObject_Param.sensor_protocol_list[i] = sensor["protocol"];
                if (!sensor["state_protocol"].isNull())
                    atObject_Param.sensor_state_protocol_list[i] = sensor["state_protocol"];
                if (!sensor["data_type"].isNull())
                    atObject_Param.sensor_data_type_list[i] = sensor["data_type"];
                if (!sensor["read_interval"].isNull())
                    atObject_Param.sensor_read_interval_list[i] = sensor["read_interval"];

                if (!sensor["rs485_index"].isNull())
                {
                    JsonArray rs485_idx = sensor["rs485_index"];
                    for (int ch = 0; ch < MAX_RS485_CHANNEL && ch < rs485_idx.size(); ch++)
                    {
                        atObject_Param.sensor_rs485_index[ch][i] = rs485_idx[ch];
                    }
                }

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

                    if (!calib["table_points"].isNull())
                    {
                        JsonArray table_points = calib["table_points"];
                        for (int j = 0; j < MAX_CALIB_TABLE_POINTS && j < table_points.size(); j++)
                        {
                            JsonObject point = table_points[j];
                            if (!point["raw_value"].isNull())
                                atObject_Param.sensor_calib_table_list[i][j].raw_value = point["raw_value"];
                            if (!point["calib_value"].isNull())
                                atObject_Param.sensor_calib_table_list[i][j].calib_value = point["calib_value"];
                            if (!point["active"].isNull())
                                atObject_Param.sensor_calib_table_point_active_list[i][j] = point["active"];
                        }
                    }
                }

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

                if (!sensor["limit"].isNull())
                {
                    JsonObject limit = sensor["limit"];
                    if (!limit["enable"].isNull())
                        atObject_Param.sensor_limit_enable_list[i] = limit["enable"];
                    if (!limit["calib_lower"].isNull())
                        atObject_Param.sensor_calib_lower_limit_threshold_list[i] = limit["calib_lower"];
                    if (!limit["calib_upper"].isNull())
                        atObject_Param.sensor_calib_upper_limit_threshold_list[i] = limit["calib_upper"];
                    if (!limit["lower_noise"].isNull())
                        atObject_Param.sensor_lower_limit_noise_list[i] = limit["lower_noise"];
                    if (!limit["upper_noise"].isNull())
                        atObject_Param.sensor_upper_limit_noise_list[i] = limit["upper_noise"];
                }
            }
        }
    }

    // MQTT Config
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
    }

    // FTP Config
    if (!doc["ftp_config"].isNull())
    {
        JsonObject ftp = doc["ftp_config"];
        if (!ftp["servers"].isNull())
        {
            JsonArray ftp_servers = ftp["servers"];
            for (int i = 0; i < MAX_FTP_SERVER && i < ftp_servers.size(); i++)
            {
                JsonObject server = ftp_servers[i];

                if (!server["active"].isNull())
                    atObject_Param.ftp_active_list[i] = server["active"];
                if (!server["host"].isNull())
                    atObject_Param.ftp_host_list[i] = server["host"].as<String>();
                if (!server["port"].isNull())
                    atObject_Param.ftp_port_list[i] = server["port"];
                if (!server["timeout"].isNull())
                    atObject_Param.ftp_timeout_list[i] = server["timeout"];
                if (!server["user"].isNull())
                    atObject_Param.ftp_user_list[i] = server["user"].as<String>();
                if (!server["pass"].isNull())
                    atObject_Param.ftp_pass_list[i] = server["pass"].as<String>();
                if (!server["path"].isNull())
                    atObject_Param.ftp_path_list[i] = server["path"].as<String>();
                if (!server["file_name"].isNull())
                    atObject_Param.ftp_file_name_list[i] = server["file_name"].as<String>();
                if (!server["retry_times"].isNull())
                    atObject_Param.ftp_retry_times_list[i] = server["retry_times"];
                if (!server["send_time"].isNull())
                    atObject_Param.ftp_send_time_list[i] = server["send_time"];
                if (!server["max_file_number"].isNull())
                    atObject_Param.ftp_max_file_number_per_one_send_time[i] = server["max_file_number"];

                if (!server["sensor_config"].isNull())
                {
                    JsonArray sensor_cfg = server["sensor_config"];
                    for (int j = 0; j < MAX_SENSORS && j < sensor_cfg.size(); j++)
                    {
                        atObject_Param.ftp_sensor_config[i][j] = sensor_cfg[j];
                    }
                }
            }
        }
    }

    // Logger Config
    if (!doc["logger_config"].isNull())
    {
        JsonObject logger = doc["logger_config"];
        if (!logger["save_paths"].isNull())
        {
            JsonArray save_paths = logger["save_paths"];
            for (int i = 0; i < MAX_FTP_SERVER && i < save_paths.size(); i++)
            {
                atObject_Param.logger_save_path[i] = save_paths[i].as<String>();
            }
        }
        if (!logger["sent_paths"].isNull())
        {
            JsonArray sent_paths = logger["sent_paths"];
            for (int i = 0; i < MAX_FTP_SERVER && i < sent_paths.size(); i++)
            {
                atObject_Param.logger_save_sent_file_path[i] = sent_paths[i].as<String>();
            }
        }
    }

    // RTC Config
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
    }

    if (atService_SD.User_Mode == SER_USER_MODE_DEBUG)
    {
        Serial.printf("[SD] Đã load config từ JSON: %s\n", path);
    }

    return true;
}

#endif
