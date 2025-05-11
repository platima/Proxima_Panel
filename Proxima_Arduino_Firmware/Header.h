// Header.h for Proxima Arduino Firmware
// 2025 Platima (https://github.com/platima https://plati.ma)
// Mainly for struct and enum limitations of Arduino

#define PROXIMA_VERSION "0.2.6"

// ESP8266 WiFi Connection
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager

// Storage libraries
#include <LittleFS.h>             // For file system
#include <ArduinoJson.h>          // For JSON serialization

// WS2812B
#include <Adafruit_NeoPixel.h>
#define LED_PIN D7
#define LED_COUNT 40
Adafruit_NeoPixel RGBpanel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// DISPLAY
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#define WHITE 1
#define BLACK 0
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET     -1 
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
bool displayAvailable = false;
const uint8_t addresses[] = {0x3C, 0x3D, 0x3E, 0x3F}; // Addresses to try in order
const int numAddresses = sizeof(addresses) / sizeof(addresses[0]);

// Buttons
#define btn_up D5
#define btn_enter D6
#define btn_down D0 

// WIFI STATUS
bool wifiStatus = false;

// RGB PANEL SETTINGS
String rgbPanelMode = "Auto";
int red = 20;
int green = 20;
int blue = 20;
// Remove the brightness array - now using 0-255 directly
int rgbBrightnessLevel = 50; // Changed to default to a reasonable value (instead of 1)

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
AnimationMode currentAnimation = STATIC;

// Animation functions - declared here to be used in main file
void processAnimations();
String getAnimationName(AnimationMode mode);
AnimationMode getAnimationByName(String name);

// Animation timing variables
unsigned long lastAnimationUpdate = 0;
unsigned long animationInterval = 30; // Default 30ms (about 33fps)

// Animation parameters
float breathingIntensity = 0.0;
float breathingDirection = 1.0; // 1.0 = increasing, -1.0 = decreasing
float rainbowOffset = 0.0;
float fadeHue = 0.0;
float pulseStep = 0.0;
float pulseDirection = 1.0;

// Animation configuration structure
struct AnimationConfig {
  const char* name;
  bool allowRGBControl;
  bool allowBrightnessControl;
  float speed; // Speed multiplier for the animation
};

// Define animation configurations
const AnimationConfig animationConfigs[] = {
  {"Static", true, true, 1.0},
  {"Breathing", true, true, 0.5},
  {"Rainbow", false, true, 1.0},
  {"Pulse", true, true, 0.8},
  {"Color Fade", false, true, 0.3}
};

// Animation names for display and storage (maintain compatibility)
const char* animationNames[] = {
  "Static",
  "Breathing",
  "Rainbow",
  "Pulse",
  "Color Fade"
};

// Stored OLED images
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