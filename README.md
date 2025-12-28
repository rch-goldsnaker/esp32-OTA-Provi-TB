# ESP32 OTA ThingsBoard Example

This repository contains implementation examples of **Over-The-Air (OTA)** updates using **ESP32** and **ThingsBoard**. Specifically designed for a **Heltec WiFi LoRa 32 V3** board with integrated OLED display.

## 📋 Description

This project demonstrates how to implement remote firmware updates (OTA) through ThingsBoard, providing two different examples:

- **Version TEST**: Basic implementation to understand OTA functionality
- **Version CORE**: Complete version with graphical interface on Heltec V3 OLED display

### 💡 Example Use Case: Temperature and Humidity Monitoring

This project includes a practical example of version management where:

- **Version 1.0.0**: Configured to send **temperature** telemetry data to ThingsBoard
- **Version 1.0.1**: Updated version configured to send **humidity** telemetry data instead

This demonstrates how OTA updates can be used to change device functionality remotely without physical access, perfect for:
- Switching between different sensor readings
- Updating device behavior based on deployment needs
- Testing different telemetry configurations in production environments

## 🔧 Required Hardware

- **Heltec WiFi LoRa 32 V3** (or compatible ESP32 with OLED display)
- WiFi connection
- ThingsBoard Cloud account

## 📁 Project Structure

```
📦 esp32-OTA-TB
├── 📁 include/
│   ├── config.h                   # General configuration
│   ├── credentials.h              # ⚠️ File to create (see instructions)
│   └── credentials.template.h     # Template for credentials
├── 📁 lib/                       # Local libraries
├── 📁 src/
│   └── main.cpp                   # Main file (copy from versions)
├── 📁 test/                      # Test files
├── 📁 Version TEST/
│   ├── 📁 v1.0.0/
│   │   └── main.cpp              # Basic OTA example
│   └── 📁 v2.0.0/
│       └── main.cpp              # Updated version
├── 📁 Version CORE/
│   ├── 📁 v1.0.0/
│   │   └── main.cpp              # Version with OLED display
│   └── 📁 v2.0.0/
│       └── main.cpp              # Updated version
└── platformio.ini                # PlatformIO configuration
```

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
// ThingsBoard Device Token
// ===========================================
#define TB_TOKEN "YOUR_THINGSBOARD_TOKEN"
```

### 3. Configure ThingsBoard

1. Create an account on [ThingsBoard Cloud](https://thingsboard.cloud/)
2. Create a new device in your dashboard
3. Copy the **Device Token** and use it in `credentials.h`

### 4. Select Version

Copy the content from the desired version's `main.cpp` file to `src/main.cpp`:

- For basic example: `Version TEST/v1.0.0/main.cpp` → `src/main.cpp`
- For version with display: `Version CORE/v1.0.0/main.cpp` → `src/main.cpp`

### 5. Compile and Upload

```bash
# Compile the project
pio run

# Upload to board
pio run --target upload
```

## 📊 Available Versions

### Version TEST
- **Purpose**: Basic OTA demonstration
- **Features**:
  - WiFi connection
  - MQTT communication with ThingsBoard
  - Automatic update downloads
  - Detailed logging in Serial Monitor

### Version CORE
- **Purpose**: Complete implementation with visual interface
- **Features**:
  - Everything from Version TEST
  - Graphical interface on 128x64 OLED display
  - Visual states: BOOT, OTA CHECK, OTA PROGRESS, RUN
  - Simulated telemetry (current and voltage)
  - WiFi and ThingsBoard connectivity indicators
  - Progress bar during OTA update

### 📈 Version History - Practical Example

#### Version 1.0.0 - Temperature Sensor
- **Telemetry**: Sends temperature data (°C)
- **Purpose**: Monitor ambient temperature
- **Data Sent**:
  ```cpp
  temperature: 25.4°C  // Simulated or from sensor
  ```
- **Use Case**: Temperature monitoring in server rooms, greenhouses, or climate control systems

#### Version 1.0.1 - Humidity Sensor
- **Telemetry**: Sends humidity data (%)
- **Purpose**: Monitor relative humidity
- **Data Sent**:
  ```cpp
  humidity: 65.2%  // Simulated or from sensor
  ```
- **Use Case**: Humidity monitoring in storage facilities, museums, or agricultural environments

This example demonstrates how a single device can be repurposed through OTA updates:
- Deploy v1.0.0 to monitor temperature
- Update to v1.0.1 via ThingsBoard dashboard to switch to humidity monitoring
- No physical access required - update happens remotely
- Perfect for testing different sensor configurations or changing requirements

## 🔄 How OTA Works

1. **Startup**: ESP32 connects to WiFi and ThingsBoard
2. **Identification**: Sends current firmware information (title and version)
3. **Verification**: ThingsBoard compares with available version on server
4. **Download**: If new version exists, downloads automatically
5. **Installation**: Updates firmware and restarts device

## 📚 Libraries Used

- `ThingsBoard` (0.14.0) - ThingsBoard client for ESP32
- `ArduinoMqttClient` - MQTT client
- `ArduinoHttpClient` - HTTP client
- `U8g2` (2.34.22) - OLED display control (CORE Version only)
- `PubSubClient` - Additional MQTT client

## 🛠️ Hardware Configuration

### Heltec WiFi LoRa 32 V3 - OLED Pins
```cpp
#define SDA_OLED 17    // SDA pin for OLED display
#define SCL_OLED 18    // SCL pin for OLED display  
#define RST_OLED 21    // Reset pin for OLED display
#define Vext     36    // External power pin
```

## 📱 User Interface (Version CORE)

The CORE version shows different screens based on status:

- **BOOT**: Startup screen with "MIRUTEC" logo
- **OTA CHECK**: Checking for available updates
- **OTA PROGRESS**: Progress bar during download
- **RUN**: Main screen with telemetry and connection status

## 🔧 Troubleshooting

### Common Issues

1. **Compilation error due to credentials**:
   - Verify that `credentials.h` exists in the `include/` folder
   - Make sure there are no special characters in credentials

2. **Won't connect to WiFi**:
   - Verify SSID and password in `credentials.h`
   - Check that WiFi is on 2.4GHz (ESP32 doesn't support 5GHz)

3. **Won't connect to ThingsBoard**:
   - Verify device token
   - Check that device is active in ThingsBoard

4. **OLED display not working**:
   - Verify I2C connections (pins 17, 18, 21)
   - Make sure you're using CORE Version

## 📝 Security Notes

- **NEVER** upload the `credentials.h` file to public repositories
- Add `credentials.h` to your `.gitignore`
- Use unique device tokens for each ESP32

## 🤝 Contributions

Contributions are welcome. Please:

1. Fork the project
2. Create a branch for your feature (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📄 License

This project is open source and available under the MIT license.

## 🔗 Useful Links

- [ThingsBoard Documentation](https://thingsboard.io/docs/)
- [Heltec WiFi LoRa 32 V3 Documentation](https://heltec.org/project/wifi-lora-32-v3/)
- [PlatformIO Documentation](https://docs.platformio.org/)
- [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32)

---

⚡ **Developed to demonstrate robust and scalable OTA implementations with ESP32 and ThingsBoard**