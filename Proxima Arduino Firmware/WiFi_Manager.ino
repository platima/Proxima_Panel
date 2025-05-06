// WiFiManager Configuration for Proxima Device
#include <WiFiManager.h>

// WiFiManager instance
WiFiManager wifiManager;

// AP Settings
#define AP_NAME "Proxima-Setup"
#define AP_PASSWORD "proxima123"  // Optional, can be removed for open AP

// Timeout settings (in seconds)
#define CONFIG_PORTAL_TIMEOUT 180  // 3 minutes

// Function prototypes
void configModeCallback(WiFiManager *myWiFiManager);
void saveConfigCallback();

// Flag for saving data
bool shouldSaveConfig = false;

void setupWiFiManager() {
  // Set WiFiManager callback functions
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  
  // Set timeout for config portal
  wifiManager.setConfigPortalTimeout(CONFIG_PORTAL_TIMEOUT);
  
  // Customize AP settings
  wifiManager.setAPStaticIPConfig(IPAddress(192,168,4,1), IPAddress(192,168,4,1), IPAddress(255,255,255,0));
  
  // Set minimum signal quality (percentage)
  wifiManager.setMinimumSignalQuality(30);
  
  // Add custom parameter if needed
  // WiFiManagerParameter custom_field("field_id", "Field Label", "default value", 40);
  // wifiManager.addParameter(&custom_field);
}

// Function to connect to WiFi - replaces your current conn() function
bool connectWiFi() {
  // Show connecting animation on display
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Starting WiFi");
  display.display();
  
  // Initialize WiFiManager
  setupWiFiManager();
  
  // Try to connect with saved credentials
  // If connection fails, it will start AP mode automatically
  Serial.println("Connecting to WiFi...");
  
  if (wifiManager.autoConnect(AP_NAME, AP_PASSWORD)) {
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    // Show connected status on display
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("WiFi connected");
    display.setCursor(0, 10);
    display.print("IP: ");
    display.println(WiFi.localIP());
    display.display();
    delay(2000);  // Show for 2 seconds
    
    wifiStatus = true;
    return true;
  } else {
    Serial.println("WiFi connection failed");
    wifiStatus = false;
    return false;
  }
}

// Reset saved WiFi settings and restart
void resetWiFiSettings() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Resetting WiFi");
  display.println("Settings...");
  display.display();
  
  // Reset settings
  wifiManager.resetSettings();
  
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Reset Complete");
  display.println("Restarting...");
  display.display();
  
  delay(1000);
  ESP.restart();
}

// Called when entering config mode (AP mode)
void configModeCallback(WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
  
  // Show this info on OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("WiFi Setup Mode");
  display.setCursor(0, 10);
  display.println("Connect to WiFi:");
  display.setCursor(0, 20);
  display.println(myWiFiManager->getConfigPortalSSID());
  display.setCursor(0, 30);
  display.println("Then go to:");
  display.setCursor(0, 40);
  display.println("192.168.4.1");
  display.display();
}

// Called when configuration is saved
void saveConfigCallback() {
  Serial.println("Configuration saved");
  shouldSaveConfig = true;
  
  // Show on OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("WiFi Config Saved");
  display.setCursor(0, 20);
  display.println("Connecting...");
  display.display();
}

// Called when user presses button to start WiFi setup
void startWiFiPortal() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Starting WiFi");
  display.println("Setup Portal...");
  display.display();
  
  // Start config portal without timeout
  wifiManager.startConfigPortal(AP_NAME, AP_PASSWORD);
}
