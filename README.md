# 🌟 DHA-1 Industrial IoT Data Acquisition System

<div align="center">

![ESP32-S3](https://img.shields.io/badge/ESP32--S3-Development-blue?style=for-the-badge&logo=espressif)
![Platform](https://img.shields.io/badge/PlatformIO-orange?style=for-the-badge&logo=platformio)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)
![Version](https://img.shields.io/badge/Version-1.0-red?style=for-the-badge)

**Professional Industrial IoT Data Acquisition System**  
*Powered by ESP32-S3 with Web-based Configuration Portal*

</div>

---

## 📋 Mục Lục

- [🔍 Giới Thiệu](#-giới-thiệu)
- [✨ Tính Năng Chính](#-tính-năng-chính)
- [🏗️ Cấu Trúc Dự Án](#️-cấu-trúc-dự-án)
- [🛠️ Cài Đặt và Build](#️-cài-đặt-và-build)
- [🌐 Web Configuration Portal](#-web-configuration-portal)
- [📡 API Documentation](#-api-documentation)
- [🔧 Hardware Requirements](#-hardware-requirements)
- [📊 Protocols Supported](#-protocols-supported)
- [🚀 Quick Start](#-quick-start)
- [📖 Hướng Dẫn Sử Dụng](#-hướng-dẫn-sử-dụng)
- [🤝 Contributing](#-contributing)

---

## 🔍 Giới Thiệu

**DHA-1** là hệ thống thu thập dữ liệu công nghiệp IoT chuyên nghiệp được xây dựng trên nền tảng **ESP32-S3**. Hệ thống cung cấp giao diện web hiện đại để cấu hình và giám sát đến **10 sensor** với nhiều giao thức khác nhau, bao gồm **ADC 4-20mA** và **RS485 Modbus**, đồng thời cung cấp cho người dùng các cảnh báo, để điều khiển các relay hoặc đọc trạng thái của cảm biến thông qua các input đầu vào. 

### 🎯 Mục Tiêu

- **Đơn giản hóa** việc cấu hình hệ thống công nghiệp
- **Chuẩn hóa** giao diện web cho các thiết bị IoT
- **Tối ưu hóa** hiệu suất và độ tin cậy
- **Mở rộng** khả năng tích hợp với các hệ thống SCADA/MES

---

## ✨ Tính Năng Chính

### 🌐 **Web Configuration Portal**
- ✅ Giao diện web responsive hiện đại
- ✅ Dashboard real-time với cập nhật tự động 
- ✅ Cấu hình sensor với Point Table Calibration
- ✅ Cấu hình truyền file FTP theo thông tư 10 và 17 BTNMT
- ✅ Authentication với mật khẩu và password
- ✅ Export/Import cấu hình dạng JSON

### 📊 **Multi-Protocol Sensor Support**
- ✅ **ADC 4-20mA**: 4 cổng dọc tín hiệu analog chuẩn công nghiệp
- ✅ **RS485 Modbus RTU**: 2 channels, mỗi channel tối đa 10 slaves
- ✅ **Digital Input**: 4 channels
- ✅ **Digital Output**: 4 channels


### 🔧 **Advanced Features**
- ✅ **Real-time Clock (RTC)**: DS3231 với battery backup
- ✅ **LCD Display**: 16x2 I2C với backlight control
- ✅ **SD Card Logging**: Automatic data logging
- ✅ **MQTT Integration**: Publish/Subscribe capabilities
- ✅ **FTP Server**: Remote file management

### 📱 **User Experience**
- ✅ **Tabbed Interface**: Organized configuration sections
- ✅ **Live Data Updates**: 2-second refresh rate
- ✅ **Mobile Responsive**: Works on phones and tablets
- ✅ **Error Handling**: Comprehensive error action
- ✅ **Auto-restart**: Configuration changes trigger restart

---

## 🏗️ Cấu Trúc Dự Án

```
DHA-1/
├── 📁 src/
│   ├── 📄 main.cpp                    # Entry point
│   ├── 📁 apps/                       # Application modules
│   │   ├── 📄 App.h                   # Base application class
│   │   ├── 📄 atApp_AP.h              # Access Point & Web Server
│   │   ├── 📄 atApp_Sensor.h          # Sensor management
│   │   ├── 📄 atApp_MQTT_PUB.h        # MQTT Publisher
│   │   ├── 📄 atApp_RTC_v2.h          # Real-time Clock
│   │   ├── 📄 atApp_LCD.h             # LCD Display
│   │   ├── 📄 webApp.h                # Web interface (auto-generated)
│   │   └── 📄 ...                     # Other application modules
│   ├── 📁 obj/                        # Data objects
│   │   ├── 📄 Object.h                # Base object class
│   │   ├── 📄 atObj_Data.h            # Runtime data structure
│   │   └── 📄 atObj_Param.h           # Configuration parameters
│   └── 📁 services/                   # Hardware abstraction
│       ├── 📄 Service.h               # Base service class
│       ├── 📄 atService_ADS1115.h     # ADC service
│       ├── 📄 atService_Modbus.h      # Modbus RTU
│       ├── 📄 atService_DS3231.h      # RTC service
│       └── 📄 ...                     # Other hardware services
├── 📁 data/                           # Web assets
│   ├── 📄 webApp.html                 # Main web interface
│   └── 📄 sensor.html                 # Sensor calibration table
├── 📁 lib/                            # External libraries (ZIP format)
│   ├── 📦 ArduinoJson.zip
│   ├── 📦 RTClib.zip
│   ├── 📦 ModbusMaster.zip
│   └── 📦 ...
├── 📄 platformio.ini                  # PlatformIO configuration
├── 📄 embed_html.py                   # Build script (HTML embedding)
└── 📄 README.md                       # This file
```


---

## 🛠️ Cài Đặt và Build

### 📋 Prerequisites

- **PlatformIO IDE** hoặc **PlatformIO Core**
- **ESP32-S3** DHA-1v1
- **USB-B cable** cho lập trình

### 🚀 Build Instructions

1. **Clone repository:**
```bash
git clone https://github.com/MINHDUONGJSC/DHA-1.git
cd DHA-1
```

2. **Install dependencies:**
```bash
pio lib install
```

3. **Build firmware:**
```bash
pio run
```

4. **Upload to ESP32-S3:**
```bash
pio run --target upload
```

5. **Monitor serial output:**
```bash
pio device monitor
```

### ⚙️ Configuration Files

| File | Purpose | Format |
|------|---------|--------|
| `platformio.ini` | Build configuration | INI |
| `embed_html.py` | HTML embedding script | Python |

---

## 🌐 Web Configuration Portal

### 🔐 Access Information

- **SSID**: `DHA-1`
- **Password**: `12345678`
- **IP Address**: `192.168.4.1`
- **Login**: `admin` / `admin123`

### 📱 Interface Features

#### 🏠 **Dashboard Tab**
- **Real-time sensor readings** (update every 2 seconds)
- **System status indicators**
- **Alarm and warning displays**
- **Uptime and memory usage**

#### ⚙️ **Configuration Tabs**

| Tab | Description | Features |
|-----|-------------|----------|
| **General** | System settings | Device name, time zone, network |
| **Sensors** | Sensor configuration | Protocol, calibration, limits |
| **Digital I/O** | I/O configuration | Input/output mapping, control logic |
| **RS485** | Modbus settings | 2 channels, 10 slaves per channel |
| **MQTT** | MQTT integration | Broker settings, topics |
| **RTC** | Time settings | Auto-sync, timezone |
| **LCD** | Display settings | Backlight, refresh rate |

#### 🔧 **Advanced Features**

- **Point Table Calibration**: 10-point linearization
- **Error Thresholds**: Configurable alarm levels
- **Data Export**: Download configuration as JSON
- **Firmware Update**: OTA capability
- **Log Viewer**: Real-time log display

---

## 📡 API Documentation

### 🔑 Authentication

All API calls require JWT authentication header:
```http
Authorization: Bearer <token>
```

### 🌐 Endpoints

#### **Authentication**
```http
POST /api/login
Content-Type: application/json

{
  "username": "admin",
  "password": "admin123"
}
```

#### **Configuration**
```http
GET /api/config          # Get full configuration
POST /api/save           # Save configuration
GET /api/dashboard       # Get real-time data
GET /api/rtc            # Get RTC information
```

#### **Response Format**
```json
{
  "success": true,
  "data": {
    "sensors": [
      {
        "id": 0,
        "name": "Temperature",
        "raw": 2048.5,
        "calib": 25.3,
        "status": 0,
        "unit": "°C"
      }
    ],
    "system": {
      "uptime": 3600,
      "free_heap": 245760,
      "rtc_temp": 24.5
    }
  }
}
```

---

## 🔧 Hardware Requirements

### 💾 **ESP32-S3 Specifications**

| Component | Requirement | Notes |
|-----------|-------------|-------|
| **MCU** | ESP32-S3 | Dual-core, 240MHz |
| **Flash** | 16MB | For firmware + web assets |
| **PSRAM** | 8MB | For buffering and processing |
| **WiFi** | 802.11 b/g/n | Access Point mode |

### 🔌 **External Components**

| Component | Interface | Purpose |
|-----------|-----------|---------|
| **ADS1115** | I2C | 16-bit ADC for 4-20mA |
| **DS3231** | I2C | Real-time clock |
| **LCD 16x2** | I2C | Local display |
| **SD Card** | SPI | Data logging |
| **RS485 Transceiver x2** | UART | 2-channel Modbus communication |
| **PCF8574** | I2C | I/O expansion |
| **74HC595** | SPI | Output expansion |

---

## 📊 Protocols Supported

### 🔧 **ADC 4-20mA**
- **Resolution**: 16-bit (ADS1115)
- **Range**: 4-20mA industrial standard
- **Accuracy**: ±0.1% FSR
- **Calibration**: Point table or polynomial

### 📡 **RS485 Modbus RTU**
- **Channels**: 2 independent channels
- **Slaves per Channel**: Up to 10 devices
- **Total Slaves**: Maximum 20 devices
- **Baud Rate**: 9600, 19200, 38400, 115200
- **Data Bits**: 8
- **Parity**: None, Even, Odd
- **Stop Bits**: 1, 2
- **Functions**: 03 (Read Holding), 04 (Read Input)

### 🌐 **MQTT**
- **Version**: 3.1.1
- **QoS**: 0, 1, 2
- **Topics**: Configurable publish/subscribe
- **Payload**: JSON format
- **SSL/TLS**: Supported

---

## 🚀 Quick Start

### 1️⃣ **Hardware Setup**
```
1. Connect ESP32-S3 to power
2. Wire I2C devices (SDA=21, SCL=22)
3. Connect RS485 transceiver
4. Insert SD card
5. Power on system
```

### 2️⃣ **Network Connection**
```
1. Connect to WiFi "DHA-1"
2. Password: "12345678"
3. Open browser: http://192.168.4.1
4. Login: admin / admin123
```

### 3️⃣ **Sensor Configuration**
```
1. Go to "Sensors" tab
2. Enable desired sensors
3. Select protocol (ADC/RS485)
4. Configure calibration
5. Set alarm thresholds
6. Save configuration
```

### 4️⃣ **Monitoring**
```
1. Switch to "Dashboard" tab
2. View real-time data
3. Monitor system status
4. Check alarm conditions
```

---

## 📖 Hướng Dẫn Sử Dụng

### 🔧 **Cấu Hình Sensor**

#### **ADC 4-20mA Setup:**
1. Chọn **Protocol**: ADC
2. Cấu hình **Calibration**:
   - Linear: `y = A + Bx`
   - Point Table: Nhập 10 điểm chuẩn
3. Thiết lập **Error Thresholds**
4. Cấu hình **Output Limits**

#### **RS485 Modbus Setup:**
1. Chọn **Protocol**: RS485
2. Chọn **Channel** (1 hoặc 2)
3. Chọn **Slave Device** (1-10 per channel)
4. Cấu hình **Register Address**
5. Thiết lập **Data Type** (INT16, FLOAT, etc.)
6. Áp dụng **Calibration Function**

### 📊 **Dashboard Usage**

- **Sensor Cards**: Hiển thị real-time data
- **Status Colors**:
  - 🟢 **Green**: Normal operation
  - 🟡 **Orange**: Calibration mode
  - 🔴 **Red**: Error condition
- **Auto-refresh**: Cập nhật mỗi 2 giây

### 💾 **Data Management**

- **Save Config**: Lưu cấu hình vào SD card
- **Export**: Download file JSON
- **Backup**: Tự động backup định kỳ
- **Restore**: Import từ file JSON

---

## 🤝 Contributing

### 📋 **Development Guidelines**

1. **Fork** repository
2. **Create** feature branch
3. **Follow** coding standards
4. **Test** thoroughly
5. **Submit** pull request

### 🐛 **Bug Reports**

Sử dụng GitHub Issues với template:
- **Environment**: Hardware, firmware version
- **Steps**: Cách tái hiện lỗi
- **Expected**: Kết quả mong đợi
- **Actual**: Kết quả thực tế

### 💡 **Feature Requests**

- Mô tả chi tiết tính năng
- Giải thích use case
- Đề xuất implementation

---

## 📞 Contact & Support

<div align="center">

**🏢 MASSOFT**  
*Industrial IoT & Automation Solutions*

📧 **Email**: support@massoft.vn  
🌐 **Website**: https://massoft.vn  
📱 **Phone**: +84 XXX XXX XXX

---

**© 2025 MASSOFT. All rights reserved.**

</div>
