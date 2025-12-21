// ================== LIBRARIES ==================
#include <WiFi.h>
#include <Arduino_MQTT_Client.h>
#include <OTA_Firmware_Update.h>
#include <ThingsBoard.h>
#include <Espressif_Updater.h>
#include <Wire.h>
#include <U8g2lib.h>
// Configuration files
#include "credentials.h"

// ================== HELTEC V3 PINS ==================
#define SDA_OLED 17
#define SCL_OLED 18
#define RST_OLED 21
#define Vext     36

// ================== OLED ==================
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(
  U8G2_R0, RST_OLED, SCL_OLED, SDA_OLED
);

// ================== FIRMWARE ==================
constexpr char FW_TITLE[]   = "CORE";
constexpr char FW_VERSION[] = "2.0.0";

// ================== OTA ==================
constexpr uint8_t  OTA_RETRIES = 12;
constexpr uint16_t OTA_PACKET_SIZE = 16384;
constexpr uint32_t OTA_CHECK_TIMEOUT = 5000;

// ================== WIFI ==================
constexpr char wifiSSID[] = WIFI_SSID;
constexpr char wifiPassword[] = WIFI_PASSWORD;

// ================== THINGSBOARD ==================
constexpr char token[] = TB_TOKEN;
constexpr char TB_HOST[] = "thingsboard.cloud";
constexpr uint16_t TB_PORT = 1883;

// ================== SYSTEM ==================
constexpr uint32_t SERIAL_BAUD = 115200;
constexpr uint32_t TELEMETRY_INTERVAL = 10000;
constexpr uint32_t SCREEN_INTERVAL = 2000;

// ================== LOGS ==================
#define LOGI(x) Serial.println(String("[INFO] ") + x)
#define LOGW(x) Serial.println(String("[WARN] ") + x)
#define LOGE(x) Serial.println(String("[ERROR] ") + x)

// ================== CLIENTS ==================
WiFiClient espClient;
Arduino_MQTT_Client mqttClient(espClient);
OTA_Firmware_Update<> ota;
Espressif_Updater<> updater;
const std::array<IAPI_Implementation*, 1> apis = { &ota };
ThingsBoard tb(mqttClient, 512, 512, apis);

// ================== SCREEN STATE ==================
enum ScreenState {
  SCREEN_BOOT,
  SCREEN_OTA_CHECK,
  SCREEN_OTA_PROGRESS,
  SCREEN_RUN
};

ScreenState currentScreen = SCREEN_BOOT;

// ================== STATE ==================
bool fwInfoSent = false;
bool otaRequested = false;
bool otaInProgress = false;
bool otaDone = false;

uint32_t otaCheckStart = 0;

size_t otaProgress = 0;
size_t otaTotal = 0;

float lastPower = 0.0;
float currentCurrent = 0.0;
float currentVoltage = 0.0;

uint32_t lastTelemetry = 0;
uint32_t lastScreen = 0;

// ================== POWER ==================
inline void VextON()  { pinMode(Vext, OUTPUT); digitalWrite(Vext, LOW); }
inline void VextOFF() { pinMode(Vext, OUTPUT); digitalWrite(Vext, HIGH); }

// ================== UI HELPERS ==================
void drawHeader(const char* title) {
  u8g2.setFont(u8g2_font_5x7_tf);
  u8g2.drawStr(2, 9, title);
  if (WiFi.isConnected()) u8g2.drawStr(90, 9, "WiFi");
  if (tb.connected())     u8g2.drawStr(112, 9, "TB");
  u8g2.drawHLine(0, 12, 128);
}

void drawFooter() {
  u8g2.setFont(u8g2_font_5x7_tf);

  char fw[18];
  snprintf(fw, sizeof(fw), "FW v%s", FW_VERSION);
  u8g2.drawStr(2, 62, fw);

  u8g2.drawStr(88, 62, tb.connected() ? "ONLINE" : "OFFLINE");
}

// ================== SCREENS ==================
void showBoot() {
  u8g2.clearBuffer();
  drawHeader("BOOT");

  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.drawStr(28, 34, "MIRUTEC");

  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(20, 52, "Initializing...");

  u8g2.sendBuffer();
}

void showCheckingOTA() {
  u8g2.clearBuffer();
  drawHeader("OTA");

  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.drawStr(16, 34, "CHECKING");

  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(14, 52, "FIRMWARE...");

  u8g2.sendBuffer();
}

void showOTAUpdating() {
  u8g2.clearBuffer();
  drawHeader("OTA");

  // Title
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.drawStr(18, 26, "UPDATING");

  // Percentage
  int percent = (otaTotal > 0) ? (otaProgress * 100) / otaTotal : 0;
  char pStr[10];
  snprintf(pStr, sizeof(pStr), "%d%%", percent);

  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(54, 40, pStr);

  // Progress bar
  const uint8_t barX = 14;
  const uint8_t barY = 46;
  const uint8_t barW = 100;
  const uint8_t barH = 10;

  u8g2.drawFrame(barX, barY, barW, barH);

  uint8_t fillW = (percent * (barW - 2)) / 100;
  u8g2.drawBox(barX + 1, barY + 1, fillW, barH - 2);

  u8g2.sendBuffer();
}

void showRunScreen() {
  u8g2.clearBuffer();
  drawHeader("RUN");

  char pStr[12];

  // Display power with appropriate unit
  if (lastPower >= 1000.0) {
    snprintf(pStr, sizeof(pStr), "%.2fkW", lastPower / 1000.0);
  } else {
    snprintf(pStr, sizeof(pStr), "%.1fW", lastPower);
  }

  u8g2.setFont(u8g2_font_logisoso18_tr);
  
  // Center the power reading
  uint8_t w = u8g2.getStrWidth(pStr);
  u8g2.drawStr((128 - w) / 2, 42, pStr);

  drawFooter();
  u8g2.sendBuffer();
}

// ================== OTA CALLBACKS ==================
void otaStart() {
  LOGW("OTA started");
  otaInProgress = true;
  currentScreen = SCREEN_OTA_PROGRESS;
}

void otaProgressCb(const size_t& current, const size_t& total) {
  otaProgress = current;
  otaTotal = total;
}

void otaFinish(const bool &success) {
  if (success) {
    LOGI("OTA success");
    otaDone = true;
  } else {
    LOGE("OTA failed");
    otaInProgress = false;
    currentScreen = SCREEN_RUN;
  }
}

// ================== SETUP ==================
void setup() {
  Serial.begin(SERIAL_BAUD);
  VextON();
  u8g2.begin();

  showBoot();

  WiFi.begin(wifiSSID, wifiPassword);
  while (WiFi.status() != WL_CONNECTED) delay(100);
}

// ================== LOOP ==================
void loop() {

  if (otaDone) esp_restart();

  if (!tb.connected()) {
    tb.connect(TB_HOST, token, TB_PORT);
  }

  if (!fwInfoSent) {
    fwInfoSent = ota.Firmware_Send_Info(FW_TITLE, FW_VERSION);
  }

  // ---- OTA CHECK ----
  if (!otaRequested && fwInfoSent) {
    LOGI("Checking for firmware update...");
    currentScreen = SCREEN_OTA_CHECK;
    otaCheckStart = millis();

    OTA_Update_Callback cb(
      FW_TITLE, FW_VERSION, &updater,
      otaFinish, otaProgressCb, otaStart,
      OTA_RETRIES, OTA_PACKET_SIZE
    );

    otaRequested = ota.Start_Firmware_Update(cb);
  }

  // ---- OTA TIMEOUT ----
  if (!otaInProgress &&
      currentScreen == SCREEN_OTA_CHECK &&
      millis() - otaCheckStart > OTA_CHECK_TIMEOUT) {

    LOGI("No update available → RUN");
    currentScreen = SCREEN_RUN;
  }

  // ---- TELEMETRY ----
  if (currentScreen == SCREEN_RUN &&
      millis() - lastTelemetry > TELEMETRY_INTERVAL) {

    // Generate simulated current and voltage values
    currentCurrent = 5.0 + random(0, 800) / 100.0;  // 5.0 to 13.0 A
    currentVoltage = 210 + random(0, 200) / 10.0;   // 210 to 230 V
    
    // Calculate power: P = V × I
    lastPower = currentVoltage * currentCurrent;

    // Send power telemetry to ThingsBoard
    tb.sendTelemetryData("power", lastPower);

    lastTelemetry = millis();
  }

  // ---- SCREEN REFRESH ----
  if (millis() - lastScreen > SCREEN_INTERVAL) {

    if (currentScreen == SCREEN_BOOT) showBoot();
    else if (currentScreen == SCREEN_OTA_CHECK) showCheckingOTA();
    else if (currentScreen == SCREEN_OTA_PROGRESS) showOTAUpdating();
    else showRunScreen();

    lastScreen = millis();
  }

  tb.loop();
}
