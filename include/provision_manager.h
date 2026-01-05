#ifndef PROVISION_MANAGER_H
#define PROVISION_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ThingsBoard.h>
#include "nvs_manager.h"

// Provisioning constants
constexpr char CREDENTIALS_VALUE[] = "credentialsValue";

// External references
extern bool provisionRequestSent;
extern bool provisionResponseProcessed;
extern bool isProvisioned;
extern ThingsBoard tb;

/// @brief Provision request did not receive a response in the expected amount of microseconds
inline void requestTimedOut() {
  Serial.println("[PROVISION] Request timed out. Did not receive response");
  provisionRequestSent = false; // Allow retry
}

/// @brief Process the provisioning response received from the server
/// @param data Reference to the object containing the provisioning response
inline void processProvisionResponse(const JsonDocument &data) {
  Serial.println("[PROVISION] Response received");

  if (strncmp(data["status"], "SUCCESS", strlen("SUCCESS")) != 0) {
    Serial.printf("[PROVISION] ERROR: %s\n", data["errorMsg"].as<const char*>());
    provisionRequestSent = false;
    return;
  }

  // Extract the access token
  accessToken = data[CREDENTIALS_VALUE].as<String>();
  Serial.println("[PROVISION] Access token received");

  // Save to NVS
  saveTokenToNVS();

  // Disconnect from provisioning account
  if (tb.connected()) {
    tb.disconnect();
  }
  
  provisionResponseProcessed = true;
  isProvisioned = true;
  
  Serial.println("[PROVISION] Provisioning complete!");
}

#endif // PROVISION_MANAGER_H
