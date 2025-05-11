//
// Proxima LED Panel
// Version 0.2.3
// Based on original work of Objex Proxima Alpha by Salvatore Raccardi
// Start date: 2025-05-05
// New author: Platima
// https://github.com/platima
// https://plati.ma
// ESP Board: ESP-12E / 12F
// Board URL: http://arduino.esp8266.com/stable/package_esp8266com_index.json
//
// Original Credits:
//
// OBJEX-IOT.TECH 
// OBJEX PROXIMA FIRMWARE 0.0.1
// Developer: SALVATORE RACCARDI 
//

#include "Header.h"

void setup() {
  Serial.begin(9600);
  Serial.println((String)"Proxima LED Panel " + PROXIMA_VERSION + (String)" Starting...");
  
  // Initialize storage first
  Serial.println("Initializing storage...");
  initStorage();
  
  // Load settings
  loadSettings();
  
  // BTNS INIT
  pinMode(btn_up, INPUT);
  pinMode(btn_enter, INPUT);
  pinMode(btn_down, INPUT);

  // WS2812B INIT
  ws2812bInit();
  
  // DISPLAY INIT
  delay(250); // wait for the OLED to power up
  display.begin(OLED_I2C_ADDRESS, true);
  
  // Non-blocking WiFi setup
  setupWiFi();
  
  // Set initial panel state
  if (currentAnimation == STATIC) {
    panelSet(0);
  }
}

void loop() {
  // Handle manual override if happening
  processWifiOverride();

  // Handle WiFi connection state machine
  processWiFiConnection();
  
  // If WiFi is connected, handle web server requests
  if (wifiStatus) {
    handleWebServer();
  }
  
  // Process animations (if any)
  processAnimations();
  
  // Continue with normal operation regardless of WiFi status
  if (currentAnimation == STATIC) {
    processRGBPanel();
  }
  
  // Check if settings need to be saved (will only save if values changed)
  saveSettingsIfNeeded();
}