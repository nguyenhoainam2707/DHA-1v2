# Web API Integration Guide

## Đã hoàn thành trong atApp_AP.h

### 1. Access Point Configuration
- **SSID**: `DHA-1_Config`
- **Password**: `12345678`
- **IP Address**: `192.168.4.1`
- **Max Connections**: 4 clients

### 2. API Endpoints

#### Authentication
- **POST** `/api/login`
  - Body: `{"username":"admin", "password":"admin123"}`
  - Response: `{"success":true, "token":"xxxxx"}`

- **POST** `/api/logout`
  - Response: `{"success":true, "message":"Logged out"}`

#### Configuration
- **GET** `/api/config` (requires auth)
  - Returns full configuration from atObj_Param

- **POST** `/api/save` (requires auth)
  - Body: Full configuration JSON
  - Saves to SD card and restarts ESP32

#### Dashboard
- **GET** `/api/dashboard` (requires auth)
  - Returns real-time sensor data from atObj_Data

### 3. Cần cập nhật trong webApp.html

#### Function: handleLogin()
```javascript
function handleLogin(event) {
    event.preventDefault();
    
    const username = document.getElementById('loginUsername').value;
    const password = document.getElementById('loginPassword').value;
    
    fetch('http://192.168.4.1/api/login', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ username, password })
    })
    .then(response => response.json())
    .then(data => {
        if (data.success) {
            localStorage.setItem('authToken', data.token);
            hideLogin();
            loadConfiguration(); // Load config after login
        } else {
            document.getElementById('loginError').style.display = 'block';
        }
    })
    .catch(error => console.error('Login error:', error));
}
```

#### Function: loadConfiguration()
```javascript
function loadConfiguration() {
    fetch('http://192.168.4.1/api/config', {
        method: 'GET',
        headers: { 'Authorization': 'Bearer ' + localStorage.getItem('authToken') }
    })
    .then(response => response.json())
    .then(data => {
        // Populate form fields with data
        if (data.device_info) {
            document.getElementById('deviceName').value = data.device_info.device_name;
            document.getElementById('deviceId').value = data.device_info.device_id;
            // ... populate other fields
        }
        
        if (data.sensors) {
            data.sensors.forEach((sensor, index) => {
                // Populate sensor configuration
            });
        }
        
        // ... populate MQTT, RTC, etc.
    })
    .catch(error => console.error('Load config error:', error));
}
```

#### Function: saveConfig()
```javascript
function saveConfig() {
    // Collect all form data
    const config = {
        device_info: {
            device_name: document.getElementById('deviceName').value,
            device_id: document.getElementById('deviceId').value,
            // ... collect other fields
        },
        lcd_config: {
            enable: document.getElementById('lcdEnable').checked,
            backlight_enable: document.getElementById('lcdBacklight').checked,
            refresh_rate: parseInt(document.getElementById('lcdRefreshRate').value)
        },
        sensors: [],
        mqtt: {},
        rtc: {}
        // ... collect all configurations
    };
    
    fetch('http://192.168.4.1/api/save', {
        method: 'POST',
        headers: { 
            'Content-Type': 'application/json',
            'Authorization': 'Bearer ' + localStorage.getItem('authToken')
        },
        body: JSON.stringify(config)
    })
    .then(response => response.json())
    .then(data => {
        if (data.success) {
            showMessage('Configuration saved! Device restarting...', 'success');
        } else {
            showMessage('Failed to save: ' + data.message, 'error');
        }
    })
    .catch(error => {
        console.error('Save error:', error);
        showMessage('Network error: ' + error, 'error');
    });
}
```

#### Function: updateDashboard()
```javascript
function updateDashboard() {
    fetch('http://192.168.4.1/api/dashboard', {
        method: 'GET',
        headers: { 'Authorization': 'Bearer ' + localStorage.getItem('authToken') }
    })
    .then(response => response.json())
    .then(data => {
        if (data.sensors) {
            data.sensors.forEach(sensor => {
                // Update dashboard display
                document.getElementById('sensor' + sensor.id + 'Raw').value = sensor.raw.toFixed(2);
                document.getElementById('sensor' + sensor.id + 'Calib').value = sensor.calib.toFixed(2);
                document.getElementById('sensor' + sensor.id + 'Status').textContent = getStatusText(sensor.status);
            });
        }
        
        // Update system info
        document.getElementById('uptime').textContent = formatUptime(data.uptime);
        document.getElementById('freeHeap').textContent = formatBytes(data.free_heap);
        document.getElementById('clients').textContent = data.clients;
    })
    .catch(error => console.error('Dashboard update error:', error));
}

// Auto-refresh dashboard every 2 seconds
setInterval(updateDashboard, 2000);
```

### 4. Security Notes

⚠️ **Important**: Current implementation uses simple authentication
- Username: `admin`
- Password: `admin123`
- Session timeout: 1 hour

**Khuyến nghị cho production:**
1. Thay đổi mật khẩu mặc định
2. Sử dụng HTTPS (ESP32 hỗ trợ)
3. Implement proper JWT tokens
4. Hash passwords với bcrypt
5. Add CSRF protection

### 5. Memory Management

Code đã được tối ưu hóa để tránh tràn RAM:
- Sử dụng `JsonDocument` với auto memory management (ArduinoJson v7)
- Gửi JSON theo chunks nhỏ thay vì full config
- Clear JSON document sau khi sử dụng: `doc.clear()`
- Delay 500ms trước khi restart để đảm bảo response được gửi

### 6. Testing Checklist

- [ ] Kết nối WiFi AP "DHA-1_Config"
- [ ] Truy cập http://192.168.4.1
- [ ] Login với admin/admin123
- [ ] Load configuration từ SD card
- [ ] Chỉnh sửa các thông số
- [ ] Save configuration
- [ ] Kiểm tra file Config.json trên SD card
- [ ] Verify ESP32 restart sau khi save
- [ ] Dashboard hiển thị real-time data

### 7. Troubleshooting

**Lỗi "Unauthorized":**
- Kiểm tra localStorage có authToken không
- Session có thể đã timeout (1 giờ)
- Login lại

**Lỗi "SD card not available":**
- Kiểm tra SD card được khởi tạo chưa
- Gọi `atService_SD.begin()` trong main code

**RAM overflow:**
- Giảm số sensors hiển thị trên dashboard
- Tăng delay giữa các request
- Monitor free heap: `ESP.getFreeHeap()`

**Web không load:**
- Kiểm tra đã include file `webApp.h`
- File HTML quá lớn → nén hoặc chia nhỏ
- Tăng stack size của task: `xTaskCreate(..., 8192, ...)`
