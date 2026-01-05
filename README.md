# ESP32 OTA + Provisioning + ThingsBoard

This repository contains a complete implementation of **Over-The-Air (OTA)** updates, **Automatic Provisioning**, and **ThingsBoard** integration using **ESP32**. 

## 📋 Description

This project demonstrates how to implement:
- **Automatic Device Provisioning**: Devices automatically register with ThingsBoard using Device Provisioning
- **Remote Firmware Updates (OTA)**: Update device firmware remotely through ThingsBoard
- **Persistent Credentials**: Device tokens are stored in NVS (Non-Volatile Storage) for automatic reconnection

### 💡 Key Features

**Automatic Provisioning**:
- Device automatically registers with ThingsBoard on first boot
- Uses Device Provisioning Profile with device key and secret
- Device name is automatically set to the ESP32 MAC address
- Access token is obtained and stored in NVS for future connections

**OTA Updates**:
- Automatic firmware version checking
- Automatic reboot after successful update

**Telemetry Data**:
- Temperature readings (every 5 seconds)
- WiFi diagnostics: RSSI, Channel, BSSID, Local IP, SSID

**NVS Storage**:
- Persistent storage of access token
- No need to re-provision after power loss

## 🔧 Required Hardware

- **Heltec WiFi LoRa 32 V3** 
- WiFi connection
- ThingsBoard

## 📚 Libraries Used

- `ThingsBoard` (0.14.0) - ThingsBoard client with provisioning support
- `ArduinoMqttClient` - MQTT client
- `ArduinoHttpClient` - HTTP client
- `PubSubClient` - Additional MQTT client
- `Preferences` - NVS (Non-Volatile Storage) for ESP32

## 🚀 Installation and Configuration

### 1. Prerequisites

- [PlatformIO](https://platformio.org/) installed in VS Code
- [ThingsBoard Cloud](https://thingsboard.cloud/) account

### 2. Configure Credentials

**⚠️ IMPORTANT**: Before compiling the project, you must create the credentials file.

1. Navigate to the `include/` folder
2. Copy the file `credentials.template.h` and rename it to `credentials.h`
3. Edit `credentials.h` with your actual data:

```cpp
// ===========================================
// WiFi Configuration
// ===========================================
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// ===========================================
// ThingsBoard Server Configuration
// ===========================================
#define TB_SERVER "YOUR_THINGSBOARD_SERVER_IP"

// ===========================================
// ThingsBoard Provisioning Configuration
// ===========================================
#define PROVISION_DEVICE_KEY "YOUR_PROVISION_DEVICE_KEY"
#define PROVISION_DEVICE_SECRET "YOUR_PROVISION_DEVICE_SECRET"
```

**Note**: You do NOT need to configure a device token. The token is automatically obtained through the provisioning process and stored in NVS.

### 3. Configure ThingsBoard Device Provisioning

1. Create an account on [ThingsBoard Cloud](https://thingsboard.cloud/)
2. Go to **Device profiles** and create or edit a profile
3. Enable **Device provisioning** in the profile settings:
   - Set **Provision strategy**
4. Copy the **Provision device key** and **Provision device secret**
5. Use these values in `credentials.h` (PROVISION_DEVICE_KEY and PROVISION_DEVICE_SECRET)

### 4. Upload and Run

1. Connect your ESP32 to the computer
2. Upload the code using PlatformIO
3. Open the Serial Monitor (115200 baud)

**First Boot (Provisioning)**:
```
========================================
Firmware: CORE_ESP32
Version: 1.0.0
========================================
[NVS] Checking for saved token...
[PROVISION] Not provisioned - will request provisioning
[PROVISION] Connecting to: YOUR_SERVER_IP
[PROVISION] Sending provisioning request...
[PROVISION] Device name: XX:XX:XX:XX:XX:XX
[PROVISION] Response received!
[PROVISION] Access Token: xxxxxxxxxxxxxxxx
[NVS] Token saved successfully
```

**Subsequent Boots (Auto-connect)**:
```
========================================
Firmware: CORE_ESP32
Version: 1.0.0
========================================
[NVS] Checking for saved token...
[NVS] Token loaded: xxxxxxxxxxxxxxxx
[PROVISION] Already provisioned
Connected to ThingsBoard!
[OTA] Current firmware info sent: CORE_ESP32 v1.0.0
[TELEMETRY] Data sent to ThingsBoard
```

### 5. Force Re-Provisioning (Optional)

If you need to re-provision the device, uncomment this line in `setup()`:
```cpp
clearTokenFromNVS();
```

This will erase the stored token and force the device to provision again on next boot.


## 📊 Telemetry Data Sent

The current version (1.0.0) sends the following data to ThingsBoard every 5 seconds:

- **temperature**: Random value between 10-20°C (for demonstration)
- **rssi**: WiFi signal strength
- **channel**: WiFi channel number
- **bssid**: Access Point MAC address
- **localIp**: Device IP address
- **ssid**: Connected WiFi network name

## 🔄 OTA Update Process

1. Go to ThingsBoard dashboard
2. Navigate to **OTA updates** section
3. Upload new firmware binary with a different version number
4. Assign the update to your device
5. Device will automatically detect and download the new firmware
6. After successful download, device will reboot with new firmware

## 📁 Project Structure

```
include/
├── config.h                    # General configuration
├── credentials.h               # WiFi and ThingsBoard credentials (not in repo)
├── credentials.template.h      # Template for credentials
├── nvs_manager.h               # NVS storage functions
└── provision_manager.h         # Provisioning callback handlers

src/
└── main.cpp                   # Main application code
```

## 🔑 Key Components

### NVS Manager (`nvs_manager.h`)
Handles persistent storage of the access token:
- `loadTokenFromNVS()`: Loads token from NVS on boot
- `saveTokenToNVS()`: Saves received token to NVS
- `clearTokenFromNVS()`: Erases stored token

### Provision Manager (`provision_manager.h`)
Handles provisioning callbacks:
- `processProvisionResponse()`: Processes received access token
- `requestTimedOut()`: Handles provisioning timeout

## 🛠️ Troubleshooting

**Device not provisioning**:
- Verify PROVISION_DEVICE_KEY and PROVISION_DEVICE_SECRET are correct
- Check Device Profile has provisioning enabled
- Ensure WiFi credentials are correct

**Device not reconnecting after reboot**:
- Check if token was saved to NVS (should see "[NVS] Token saved successfully")
- Try clearing NVS and re-provisioning

**OTA update fails**:
- Ensure firmware version is different
- Check ThingsBoard server is accessible
- Verify enough free memory for download

## 📝 Notes

- Device name is automatically set to MAC address
- Access token is never hardcoded, always obtained through provisioning
- Token persists across reboots using NVS
- Firmware version format: "CORE_ESP32" / "1.0.0"
