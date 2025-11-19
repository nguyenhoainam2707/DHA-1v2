# WebApp.html JavaScript Integration - Hoàn Tất

## Tóm Tắt Thay Đổi

Đã cập nhật hoàn tất các hàm JavaScript trong `webApp.html` để kết nối với ESP32 REST API.

---

## 1. Hàm loadConfiguration()

**Mục đích:** Tải cấu hình từ ESP32 và điền vào các trường form

**Endpoint:** `GET /api/config`

**Chức năng:**
- Lấy auth token từ localStorage
- Gửi GET request với Bearer token
- Parse JSON response và điền vào tất cả các trường:
  - Device Info (name, id, firmware version)
  - LCD Config (enable, backlight, refresh rate)
  - AI Config (refresh rate, 4 channels)
  - DO Config (refresh rate, 4 channels)
  - DI Config (refresh rate, 4 channels)
  - Sensor Config (10 sensors với đầy đủ thông số)
  - MQTT Config (host, port, credentials, topics)
  - RTC Config (enable flag)

**Error Handling:**
- 401 Unauthorized → logout và hiển thị login modal
- Network errors → hiển thị thông báo lỗi

**Được gọi:** Sau khi login thành công

---

## 2. Hàm saveConfig()

**Mục đích:** Lưu cấu hình xuống SD card và restart ESP32

**Endpoint:** `POST /api/save`

**Chức năng:**
- Thu thập tất cả dữ liệu từ form fields
- Build JSON object với cấu trúc hoàn chỉnh
- Gửi POST request với Bearer token
- Hiển thị thông báo "Device restarting..."
- Clear auth token sau 3 giây (do ESP32 restart)
- Redirect về login page

**Quy trình:**
1. Confirm dialog → OK?
2. Collect form data
3. POST to /api/save
4. ESP32 updates atObj_Param
5. ESP32 calls saveConfigToJSON()
6. ESP32 delays 500ms
7. ESP32 calls ESP.restart()
8. WebApp shows countdown and logout

**Error Handling:**
- 401 Unauthorized → logout
- Network/Server errors → hiển thị lỗi

---

## 3. Hàm updateDashboard()

**Mục đích:** Cập nhật real-time data lên dashboard

**Endpoint:** `GET /api/dashboard`

**Chức năng:**
- Lấy auth token (skip nếu chưa login)
- Gửi GET request với Bearer token
- Parse JSON và cập nhật readonly fields:
  - sensor_raw_value_list[0-9] → sensor-raw-1 to sensor-raw-10
  - sensor_calib_value_list[0-9] → sensor-calib-1 to sensor-calib-10
  - sensor_status_list[0-9] → sensor-status-1 to sensor-status-10
    * 0 = Normal (green)
    * 1 = Calibration (orange)
    * 2 = Error (red)
  - ai_raw_value_list[0-3] (optional)
  - di_status_list[0-3] (optional)
  - do_status_list[0-3] (optional)

**Auto-refresh:**
- `setInterval(updateDashboard, 2000)` → cập nhật mỗi 2 giây
- Dừng khi 401 Unauthorized

**Error Handling:**
- 401 → stop refresh (không logout vì có thể đang offline)
- Network errors → log to console only

---

## 4. Helper Functions

### showSuccess(message)
- Hiển thị thông báo thành công màu xanh
- Tự động ẩn sau 5 giây

### showError(message)
- Hiển thị thông báo lỗi màu đỏ
- Tự động ẩn sau 5 giây

---

## Quy Trình Làm Việc Hoàn Chỉnh

### A. First Time Setup
```
1. Connect to WiFi "DHA-1_Config" (password: 12345678)
2. Open browser: http://192.168.4.1
3. Login page appears automatically
4. Enter username/password
5. handleLogin() sends POST /api/login
6. Receive token, save to localStorage
7. loadConfiguration() auto-called
8. Configuration populated in form
9. Dashboard starts auto-refreshing
```

### B. Modify Configuration
```
1. Edit form fields in any tab
2. Click "Save Configuration to SD Card" button
3. Confirm dialog → OK
4. saveConfig() collects all data
5. POST /api/save with complete config
6. ESP32 updates RAM variables
7. ESP32 saves to /CONFIG/Config.json
8. ESP32 delays 500ms (ensure response sent)
9. ESP32 calls ESP.restart()
10. WebApp shows "Device restarting..."
11. After 3s, logout and show login page
12. User reconnects after ~30 seconds
```

### C. Real-time Monitoring
```
1. Switch to "Dashboard" tab
2. updateDashboard() runs every 2 seconds
3. Sensor values update automatically
4. Status colors change based on state
5. No manual refresh needed
```

---

## API Endpoints Đã Tích Hợp

| Endpoint | Method | Function | Auth Required |
|----------|--------|----------|---------------|
| `/api/login` | POST | handleLogin() | No |
| `/api/logout` | POST | logout() | Yes |
| `/api/config` | GET | loadConfiguration() | Yes |
| `/api/save` | POST | saveConfig() | Yes |
| `/api/dashboard` | GET | updateDashboard() | Yes |
| `/` | GET | (root page) | No |

---

## Authentication Flow

```
Login:
  → POST /api/login {username, password}
  ← {status: "success", token: "xxx"}
  → localStorage.setItem('authToken', token)
  → loadConfiguration()

API Calls:
  → Headers: {'Authorization': 'Bearer ' + token}
  
Logout:
  → POST /api/logout with token
  ← {status: "success"}
  → localStorage.removeItem('authToken')
  → showLogin()

Session Timeout:
  ← 401 Unauthorized
  → Auto logout
  → Show login page
```

---

## Memory Optimization

**ESP32 Side:**
- Use `doc.clear()` after serialization
- Send minimal dashboard JSON (only arrays)
- Chunked responses for large config
- `delay(500)` before restart

**WebApp Side:**
- Conditional dashboard updates (skip if not logged in)
- Error throttling (log only, don't spam UI)
- Clean localStorage on logout

---

## Testing Checklist

- [x] Login with correct credentials
- [x] Login with wrong credentials (should show error)
- [ ] Load configuration after login
- [ ] Verify all form fields populated correctly
- [ ] Edit configuration values
- [ ] Save configuration
- [ ] Verify "Device restarting..." message
- [ ] Verify auto-logout after save
- [ ] Reconnect after restart
- [ ] Verify changes persisted
- [ ] Dashboard auto-refresh working
- [ ] Sensor status colors correct
- [ ] Session timeout after 1 hour
- [ ] Logout button works
- [ ] Multiple tabs sync correctly

---

## Troubleshooting

### Problem: Can't login
**Solution:** Check ESP32 serial monitor for [AP] logs, verify WiFi connection

### Problem: Configuration not loading
**Solution:** Check /CONFIG/Config.json exists on SD card, verify loadConfigFromJSON() works

### Problem: Save doesn't restart device
**Solution:** Check saveConfigToJSON() returns true, verify ESP.restart() is called

### Problem: Dashboard not updating
**Solution:** Check auth token exists, verify /api/dashboard returns data, check console for errors

### Problem: 401 Unauthorized
**Solution:** Token expired (1 hour), logout and login again

---

## Code Locations

- **handleLogin()**: Line ~3392
- **logout()**: Line ~3465
- **loadConfiguration()**: Line ~3210
- **saveConfig()**: Line ~3327
- **updateDashboard()**: Line ~3820
- **showSuccess/showError()**: Line ~3503

---

## Next Steps

1. **Test trên thiết bị thật:**
   - Upload code lên ESP32-S3
   - Connect WiFi AP
   - Test toàn bộ workflow

2. **Bổ sung thêm:**
   - Password change API endpoint
   - Firmware update via web
   - Real-time logs viewer
   - Export config as JSON file
   - Import config from JSON file

3. **Tối ưu hóa:**
   - Compress JSON responses
   - WebSocket cho dashboard (thay vì polling)
   - Progressive Web App (PWA) manifest
   - Offline support

---

**Created:** $(date)
**Status:** ✅ Hoàn tất - Ready for testing
**Version:** 1.0
