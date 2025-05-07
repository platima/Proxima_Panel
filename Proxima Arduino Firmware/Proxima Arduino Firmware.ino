//
// Proxima LED Panel
// Version 0.1.0
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

// ESP8266 WiFi Connection
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// WS2812B
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h>
#endif

// DISPLAY
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#define i2c_Address 0x3C
#define WHITE 1

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 

#define OLED_RESET     -1 
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// WS2812B
#define LED_PIN D7
#define LED_COUNT 40
Adafruit_NeoPixel RGBpanel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// CONN
#define WIFI_SSID "Pi-Fi"
#define WIFI_PASSWORD "PiFi1234"

// BTN
#define btn_up D5
#define btn_enter D6
#define btn_down D0 

// WIFI STATUS
bool wifiStatus = false;

// RGB PANEL SETTINGS
String panelMode = "Auto";
int red = 20;
int green = 20;
int blue = 20;
int brightness[10] = {0, 20, 50, 80, 100, 120, 150, 180, 200, 255};
int brightness_Level = 1;


// MENU
int menuLayer = 0;

const unsigned char bitmap_WIFI1[] PROGMEM = {
    0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 
    0x60, 0xc1, 0x80, 
    0xf1, 0x22, 0x40, 
    0xf1, 0x62, 0xc0, 
    0x60, 0xc1, 0x80, 
    0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00
};

const unsigned char bitmap_WIFI2[] PROGMEM = {
    0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 
    0x60, 0xc1, 0x80, 
    0x91, 0xe2, 0x40, 
    0xb1, 0xe2, 0xc0, 
    0x60, 0xc1, 0x80, 
    0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00
};

const unsigned char bitmap_WIFI3[] PROGMEM = {
    0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 
    0x60, 0xc1, 0x80, 
    0x91, 0x23, 0xc0, 
    0xb1, 0x63, 0xc0, 
    0x60, 0xc1, 0x80, 
    0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00
};


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

  // Join wifi
  conn();
}

void loop() {
  RGB_Panel_routine();
}
