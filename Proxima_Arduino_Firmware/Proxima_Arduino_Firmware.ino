//
// Proxima LED Panel
// Version 0.2.0
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
  
  // BTNS INIT
  pinMode(btn_up, INPUT);
  pinMode(btn_enter, INPUT);
  pinMode(btn_down, INPUT);

  // WS2812B INIT
  ws2812bInit();
  
  // DISPLAY INIT
  delay(250); // wait for the OLED to power up
  display.begin(i2c_Address, true);
  
  // Non-blocking WiFi setup
  setupWiFi();
  
  // Set initial panel state
  panelSet(0);
}

void loop() {
  // Handle manual override if happening
  processWifiOverride();

  // Handle WiFi connection state machine
  processWiFiConnection();
  
  // Continue with normal operation regardless of WiFi status
  processRGBPanel();
}