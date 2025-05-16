//
// Proxima LED Panel
// Version 0.2.8
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
  
  // Enable watchdog timer - 8 second timeout
  ESP.wdtEnable(8000);
  
  // Initialize storage first
  Serial.println("Initializing storage...");
  initStorage();
  
  // Load settings
  loadSettings();
  
  // Initialize Bounce2 buttons
  upButton.attach(btn_up, INPUT);
  downButton.attach(btn_down, INPUT);
  enterButton.attach(btn_enter, INPUT);
  
  // Set debounce intervals
  upButton.interval(BUTTON_REPEAT);
  downButton.interval(BUTTON_REPEAT);
  enterButton.interval(BUTTON_REPEAT);

  // WS2812B INIT
  rgbPanelInit();
  
  // DISPLAY INIT - now with scanning
  unsigned long displayInitStart = millis();
  OLED_Init();

  // If no display, blink the RGB panel to indicate status
  if (!displayAvailable) {
    for (int i = 0; i < 3; i++) {
      RGBpanel.fill(RGBpanel.Color(255, 255, 255), 0, LED_COUNT);
      RGBpanel.show();
      unsigned long flashStart = millis();
      while (millis() - flashStart < 200) {
        ESP.wdtFeed(); // Feed watchdog during flash
      }
      RGBpanel.fill(RGBpanel.Color(0, 0, 0), 0, LED_COUNT);
      RGBpanel.show();
      flashStart = millis();
      while (millis() - flashStart < 200) {
        ESP.wdtFeed(); // Feed watchdog during pause
      }
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
  
  // Initialize timing variables
  lastUpdateTime = millis();
  lastDisplayUpdate = lastUpdateTime;
  lastAnimationUpdate = lastUpdateTime;
  lastFrameTime = lastUpdateTime;
}

void loop() {
  // Feed watchdog timer
  ESP.wdtFeed();
  
  mainloopCurrentTime = millis();
  
  // Always update button state regardless of throttling
  upButton.update();
  downButton.update();
  enterButton.update();
  
  // Process button input immediately regardless of throttling
  handleButtons();
  
  // Throttle main loop processing
  if (mainloopCurrentTime - lastUpdateTime >= UPDATE_INTERVAL) {
    lastUpdateTime = mainloopCurrentTime;
    
    // Handle manual override if happening
    processWifiOverride();

    // Handle WiFi connection state machine
    processWiFiConnection();
    
    // If WiFi is connected, handle web server requests
    if (wifiStatus) {
      handleWebServer();
    }
    
    // Always display menu regardless of animation mode
    displayMain();
    
    // Process animations if in non-static mode
    if (currentAnimation != STATIC) {
      processAnimations();
    } else {
      rgbPanelSet(0);
    }
    
    // Check if settings need to be saved
    saveSettingsIfNeeded();
  }
}