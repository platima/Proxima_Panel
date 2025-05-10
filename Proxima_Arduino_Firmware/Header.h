// ESP8266 WiFi Connection
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager

// Storage libraries
#include <LittleFS.h>             // For file system
#include <ArduinoJson.h>          // For JSON serialization - install version 6.x

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
#define BLACK 0

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 

#define OLED_RESET     -1 
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// WS2812B
#define LED_PIN D7
#define LED_COUNT 40
Adafruit_NeoPixel RGBpanel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

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

// Animation mode enum
enum AnimationMode {
  STATIC,
  BREATHING,
  RAINBOW,
  PULSE,
  COLOR_FADE
};

// Current animation mode
extern AnimationMode currentAnimation = STATIC;

// Animation functions - declared here to be used in main file
void processAnimations();
String getAnimationName(AnimationMode mode);
AnimationMode getAnimationByName(String name);

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