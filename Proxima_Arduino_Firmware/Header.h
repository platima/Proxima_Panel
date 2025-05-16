// Header.h for Proxima Arduino Firmware
// 2025 Platima (https://github.com/platima https://plati.ma)
// Mainly for struct and enum limitations of Arduino

// Arduino setup
// Upload speed: 115200
// Debug: disabled
// Flash size: 4MP (2+1)
// C++ Exceptions: disabled
// IwIP variant: v2 Lower (no features)
// Buildin LED: 2
// Debug Level: None
// MMU: 16KB cache + 48KB IRAM
// Non-32-Bit: Use pgm_read macros
// SSL support: Basic
// Stack protection: Enabled
// VTables: Flash
// Erase: Only Sketch
// CPU fequency: 80MHz

#define PROXIMA_VERSION "0.2.8"

#ifndef ICACHE_FLASH_ATTR
#define ICACHE_FLASH_ATTR __attribute__((section(".irom0.text")))
#endif

// ESP8266 WiFi Connection
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager

// Storage libraries
#include <LittleFS.h>             // For file system
#include <ArduinoJson.h>          // For JSON serialization

// Button debouncing
#include <Bounce2.h>

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
#define SCREEN_WIDTH  128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET    -1 
#define OLED_CHRWIDTH 6
#define OLED_LINE_1   5
#define OLED_LINE_2   15
#define OLED_LINE_3   25
#define OLED_LINE_4   35
#define OLED_LINE_5   45
#define OLED_LINE_6   55
bool displayAvailable = false;
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
const uint8_t addresses[] = {0x3C, 0x3D, 0x3E, 0x3F}; // Addresses to try in order
const int numAddresses = sizeof(addresses) / sizeof(addresses[0]);

// Buttons with Bounce2
#define BUTTON_REPEAT 50 // de-bounce and repeat in ms
#define btn_up D5
#define btn_enter D6
#define btn_down D0
Bounce2::Button upButton = Bounce2::Button();
Bounce2::Button downButton = Bounce2::Button();
Bounce2::Button enterButton = Bounce2::Button();
static unsigned long lastUpRepeatTime = 0;
static unsigned long lastDownRepeatTime = 0;

// WIFI STATUS
bool wifiStatus = false;

// RGB PANEL SETTINGS
uint8_t  red = 20;
uint8_t  green = 20;
uint8_t  blue = 20;
uint8_t  rgbBrightnessLevel = 50; // Changed to default to a reasonable value

// Array for cursor position
uint8_t  cursorPosition[5] = {OLED_LINE_1, OLED_LINE_2, OLED_LINE_3, OLED_LINE_4, OLED_LINE_5}; // Added one more position for Reset option

// cursorPosition[cursorIndex]
uint8_t  cursorIndex = 0;
uint8_t  menuLayer = 0;

// Display update timing
unsigned long lastDisplayUpdateTime = 0;

// Timing constants
const unsigned long UPDATE_INTERVAL = 50; // Minimum 50ms between updates
const unsigned long DISPLAY_UPDATE_INTERVAL = 100; // Display updates less frequently
const unsigned long WIFI_RECONNECT_INTERVAL = 60000; // 60 seconds between WiFi reconnection attempts
const unsigned long WEB_RATE_LIMIT_INTERVAL = 100; // 100ms minimum between web requests

// Timing variables
unsigned long mainloopCurrentTime = 0;
unsigned long lastUpdateTime = 0;
unsigned long lastDisplayUpdate = 0;
unsigned long lastWebRequest = 0;

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
const char* getAnimationName(AnimationMode mode);
AnimationMode getAnimationByName(const char* name);

// Animation timing variables
unsigned long lastAnimationUpdate = 0;
unsigned long animationInterval = 30; // Default 30ms (about 33fps)
unsigned long lastFrameTime = 0;  // For adaptive frame rate

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