#ifndef NVS_MANAGER_H
#define NVS_MANAGER_H

#include <Preferences.h>
#include <Arduino.h>

// NVS Configuration
constexpr char NVS_NAMESPACE[] = "tb_creds";
constexpr char NVS_TOKEN_KEY[] = "token";

// External reference to Preferences object
extern Preferences preferences;

// External reference to access token
extern String accessToken;

/// @brief Save access token to NVS (Non-Volatile Storage)
/// This ensures the access token persists through resets and factory resets
inline void saveTokenToNVS() {
  preferences.begin(NVS_NAMESPACE, false);
  preferences.putString(NVS_TOKEN_KEY, accessToken);
  preferences.end();
  
  Serial.println("[NVS] Access token saved successfully");
  Serial.printf("[NVS] Token: %s\n", accessToken.c_str());
}

/// @brief Load access token from NVS
/// @return true if token was found and loaded, false otherwise
inline bool loadTokenFromNVS() {
  preferences.begin(NVS_NAMESPACE, true);
  accessToken = preferences.getString(NVS_TOKEN_KEY, "");
  preferences.end();
  
  if (accessToken.length() == 0) {
    Serial.println("[NVS] No token found");
    return false;
  }
  
  Serial.println("[NVS] Token loaded successfully");
  Serial.printf("[NVS] Token: %s\n", accessToken.c_str());
  return true;
}

/// @brief Clear token from NVS
/// Useful for testing or re-provisioning
inline void clearTokenFromNVS() {
  preferences.begin(NVS_NAMESPACE, false);
  preferences.clear();
  preferences.end();
  Serial.println("[NVS] Token cleared");
}

#endif // NVS_MANAGER_H
