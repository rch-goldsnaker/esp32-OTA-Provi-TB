// Credentials configuration file
// Copy this file to credentials.h and fill in your actual credentials

#ifndef CREDENTIALS_H
#define CREDENTIALS_H

// INSTRUCTIONS:
// 1. Copy this file as 'credentials.h' in the same folder
// 2. Fill in the credentials with your actual values
// 3. Add 'credentials.h' to your .gitignore to avoid uploading it to repository

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
// Keys for ThingsBoard provisioning
// These values are obtained from the Device Profile in ThingsBoard
#define PROVISION_DEVICE_KEY "YOUR_PROVISION_DEVICE_KEY"
#define PROVISION_DEVICE_SECRET "YOUR_PROVISION_DEVICE_SECRET"

#endif // CREDENTIALS_H