//
// Proxima LED Panel
// Version 0.2.5
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
  rgbPanelInit();
  
  // DISPLAY INIT - now with scanning
  delay(250); // wait for the OLED to power up
  initDisplayWithScan();

  // If no display, blink the RGB panel to indicate status
  if (!displayAvailable) {
    for (int i = 0; i < 3; i++) {
      RGBpanel.fill(RGBpanel.Color(255, 255, 255), 0, LED_COUNT);
      RGBpanel.show();
      delay(200);
      RGBpanel.fill(RGBpanel.Color(0, 0, 0), 0, LED_COUNT);
      RGBpanel.show();
      delay(200);
    }
  }
  
  // If display is not available, we'll continue without it
  if (!displayAvailable) {
    Serial.println("Running in WiFi-only mode (no display)");
  }
  
  // Non-blocking WiFi setup
  setupWiFi();
  
  // Set initial panel state
  if (currentAnimation == STATIC) {
    rgbPanelSet(0);
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
    rgbPanelProcess();
  }
  
  // Check if settings need to be saved (will only save if values changed)
  saveSettingsIfNeeded();
}