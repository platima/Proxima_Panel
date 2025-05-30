// WiFi_Connection.ino for Proxima Arduino Firmware
// 2025 Platima (https://github.com/platima https://plati.ma)
// Handles wi-fi connection & portal in non-blocking mode

// WiFi connection states
enum WiFiConnectionState {
  WIFI_INIT,
  WIFI_CONNECTING,
  WIFI_CONFIG_PORTAL,
  WIFI_CONNECTED,
  WIFI_FAILED
};

WiFiConnectionState wifiConnState = WIFI_INIT;
unsigned long wifiLastStateChange = 0;
const unsigned long WIFI_CONNECT_TIMEOUT = 15000; // 15 seconds timeout for connection attempt
const unsigned long WIFI_PORTAL_TIMEOUT = 180000; // 3 minutes timeout for portal

WiFiManager wifiManager;
char apName[20] = "";
int j = 0; // For WiFi animation
unsigned long lastAnimationTime = 0;
const unsigned long ANIMATION_INTERVAL = 500; // Animation frame interval in ms
bool webServerStarted = false; // Flag to track if web server was started

// Callback for when WiFi credentials change
void saveWiFiConfigCallback() {
  Serial.println("WiFi credentials saved, also saving panel settings");
  // Also save our custom settings when WiFi settings change
  saveSettings();
}

ICACHE_FLASH_ATTR void setupWiFi() {
  // Feed watchdog timer first thing
  ESP.wdtFeed();

  // Initialize WiFi state machine
  wifiConnState = WIFI_INIT;
  wifiLastStateChange = millis();
  
  // Set AP name with device chip ID
  sprintf(apName, "Proxima-%X", ESP.getChipId());

  wifiManager.setCustomHeadElement(""); // Minimize custom HTML
  wifiManager.setMinimumSignalQuality(10); // Lower requirement
  wifiManager.setRemoveDuplicateAPs(false);
  wifiManager.setDebugOutput(false); // Disable debug output
  wifiManager.setMinimumSignalQuality(10); // Lower requirements
  wifiManager.setConfigPortalTimeout(180); // 3 minutes (in seconds)
  wifiManager.setConnectTimeout(30); // 30 seconds connection timeout
  wifiManager.setAPCallback(NULL); // Remove callback
  wifiManager.setSaveConfigCallback(NULL); // Remove callback
  
  // Configure WiFiManager
  wifiManager.setConfigPortalTimeout(WIFI_PORTAL_TIMEOUT / 1000); // seconds
  
  // Set callback for when settings are saved
  wifiManager.setSaveConfigCallback(saveWiFiConfigCallback);
  
  // Start the connection process
  startWiFiConnection();
}

ICACHE_FLASH_ATTR void startWiFiConnection() {
  // Feed watchdog timer first thing
  ESP.wdtFeed();

  wifiConnState = WIFI_CONNECTING;
  wifiLastStateChange = millis();
  
  if (displayAvailable) {
    // Display connecting animation
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Connecting to WiFi");
    display.display();
  }
  
  // Begin connection attempt (non-blocking)
  WiFi.begin();
}

ICACHE_FLASH_ATTR void handleWiFiConfigPortal() {
  // Feed watchdog timer first thing
  ESP.wdtFeed();

  // Only start portal if we're in the right state
  if (wifiConnState != WIFI_CONFIG_PORTAL) {
    wifiConnState = WIFI_CONFIG_PORTAL;
    wifiLastStateChange = millis();
    
    if (displayAvailable) {
      // Display config portal information
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println("WiFi Setup Mode");
      display.setCursor(0, 12);
      display.println("Connect to WiFi:");
      display.setCursor(0, 24);
      display.println(apName);
      display.setCursor(0, 36);
      display.println("Browse to:");
      display.setCursor(0, 48);
      display.println("192.168.4.1");
      display.display();
    }
    
    // Start config portal (non-blocking)
    wifiManager.startConfigPortal(apName);
  }
}

void processWiFiConnection() {
  // Feed watchdog timer first thing
  ESP.wdtFeed();

  // Handle animation timing
  unsigned long currentMillis = millis();

  if (currentMillis - lastAnimationTime >= ANIMATION_INTERVAL) {
    lastAnimationTime = currentMillis;
    
    // Update animation frame
    if (wifiConnState == WIFI_CONNECTING || wifiConnState == WIFI_CONFIG_PORTAL) {
      j = (j + 1) % 3; // Clamp to bounds using modulus in case of error
    }
  }
  
  // State machine for WiFi connection
  switch (wifiConnState) {
    case WIFI_INIT:
      // Should transition quickly to CONNECTING
      startWiFiConnection();
      break;
      
    case WIFI_CONNECTING:
      // Check if connected
      if (WiFi.status() == WL_CONNECTED) {
        // Connected successfully
        wifiConnState = WIFI_CONNECTED;
        wifiLastStateChange = millis();
        wifiStatus = true;
        
        // Start the web server if not already started
        if (!webServerStarted) {
          setupWebServer();
          webServerStarted = true;
        }
        
        if (displayAvailable) {
          // Display connected info
          display.clearDisplay();
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(0, 0);
          display.println("Connected to WiFi");
          display.setCursor(0, 20);
          display.println("IP: " + WiFi.localIP().toString());
          display.setCursor(0, 36);
          display.println("Web Control:");
          display.setCursor(0, 48);
          display.println("http://proxima.local");
          display.display();
        }
        
        Serial.println("WiFi connected");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        
        // Save settings when connection is established
        saveSettings();
        
        // Show connection info for 3 seconds then continue
        unsigned long connectStart = millis();
        while (millis() - connectStart < 3000) {
          ESP.wdtFeed(); // Feed watchdog during delay
        }
      } 
      else if (millis() - wifiLastStateChange >= WIFI_CONNECT_TIMEOUT) {
        // Timeout, start config portal
        handleWiFiConfigPortal();
      } 
      else {
        // Still connecting, update display with animation
        Wifi_connection_animation();
      }
      break;
      
    case WIFI_CONFIG_PORTAL:
      // Check if connected through portal
      if (WiFi.status() == WL_CONNECTED) {
        wifiConnState = WIFI_CONNECTED;
        wifiLastStateChange = millis();
        wifiStatus = true;
        
        // Start the web server if not already started
        if (!webServerStarted) {
          setupWebServer();
          webServerStarted = true;
        }
        
        if (displayAvailable) {
          // Display connected info
          display.clearDisplay();
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(0, 0);
          display.println("Connected to WiFi");
          display.setCursor(0, 20);
          display.println("IP: " + WiFi.localIP().toString());
          display.setCursor(0, 36);
          display.println("Web Control:");
          display.setCursor(0, 48);
          display.println("http://proxima.local");
          display.display();
        } 
        
        Serial.println("WiFi connected via portal");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        
        // Save settings after portal connection
        saveSettings();
        
        // Show connection info for 3 seconds then continue
        unsigned long connectStart = millis();
        bool skipWait = false;  

        while (millis() - connectStart < 3000 && !skipWait) {
          // Break if a button is pressed
          upButton.update();
          downButton.update();
          enterButton.update();
          if (enterButton.read() == HIGH || upButton.read() == HIGH || downButton.read() == HIGH) { skipWait = true; } // This currently does not work. Raised issue
          
          ESP.wdtFeed(); // Feed watchdog during delay
        }
      } 
      else if (millis() - wifiLastStateChange >= WIFI_PORTAL_TIMEOUT) {
        // Portal timeout
        wifiConnState = WIFI_FAILED;
        wifiLastStateChange = millis();
        
        if (displayAvailable) {
          // Display timeout message
          display.clearDisplay();
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(0, 0);
          display.println("WiFi Setup Failed");
          display.setCursor(0, 20);
          display.println("Continue without WiFi");
          display.display();
        }
        
        Serial.println("WiFi portal timeout");
        
        // Show message for 2 seconds then continue
        unsigned long timeoutStart = millis();
        while (millis() - timeoutStart < 2000) {
          ESP.wdtFeed(); // Feed watchdog during delay
        }
      } 
      else {
        // Still in portal, update display with animation
        Wifi_connection_animation();
      }
      break;
      
    case WIFI_CONNECTED:
      // Already connected, nothing to do
      // However, periodically check if still connected
      if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi connection lost, attempting to reconnect");
        wifiStatus = false;
        wifiConnState = WIFI_CONNECTING;
        wifiLastStateChange = millis();
        WiFi.begin(); // Try to reconnect
      }
      break;
      
    case WIFI_FAILED:
      // Failed to connect, but continue with the rest of the program
      // Periodically try to reconnect (every 60 seconds)
      if (millis() - wifiLastStateChange >= WIFI_RECONNECT_INTERVAL) {
        Serial.println("Attempting to reconnect WiFi");
        wifiConnState = WIFI_CONNECTING;
        wifiLastStateChange = millis();
        WiFi.begin(); // Try to reconnect
      }
      break;
  }
}

ICACHE_FLASH_ATTR void Wifi_connection_animation() {
  if (!displayAvailable) return; // Skip if no display
  
  switch (j) {
    case 0:
      display.drawBitmap(105, 0, bitmap_WIFI1, 18, 8, WHITE);
      break;
    case 1:
      display.drawBitmap(105, 0, bitmap_WIFI2, 18, 8, WHITE);
      break;
    case 2:
      display.drawBitmap(105, 0, bitmap_WIFI3, 18, 8, WHITE);
      break;
  }
  
  display.display();
}

// Show IP address
ICACHE_FLASH_ATTR void Wifi_OLED_ShowIP() {

  if (!displayAvailable) return; // Skip if no display
  String ipAddress = WiFi.localIP().toString();

  display.setTextSize(1);             
  display.setTextColor(WHITE);        
  display.setCursor((127 - (ipAddress.length() * OLED_CHRWIDTH)), OLED_LINE_6); // 128 wide display minus 15 digits for max IP width * 6 pixels per digit

  display.println(ipAddress);
}

// Add a function to manually trigger the configuration portal
// This could be bound to a button press
void startConfigPortalManually() {
  if (wifiConnState == WIFI_CONNECTED || wifiConnState == WIFI_FAILED) {
    handleWiFiConfigPortal();
  }
}

void processWifiOverride() {
  // Check for long-press of both up and down buttons to force config portal
  if (upButton.read() == HIGH && downButton.read() == HIGH) {
    // Wait to see if it's a long press (about 3 seconds)
    unsigned long pressStart = millis();
    
    while (upButton.read() == HIGH && downButton.read() == HIGH) {
      // Update buttons
      upButton.update();
      downButton.update();
      ESP.wdtFeed(); // Feed watchdog during wait
      
      // Show feedback on display
      if (displayAvailable) {
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("Hold buttons for");
        display.setCursor(0, 12);
        display.println("WiFi Setup...");
        display.setCursor(0, 30);

        // Show progress bar
        int progress = ((millis() - pressStart) * 100) / 3000; // 3 seconds
        progress = min(100, progress);
        display.drawRect(0, 40, 100, 10, WHITE);
        display.fillRect(0, 40, progress, 10, WHITE);
        display.display();
      }
      
      if (millis() - pressStart > 3000) {
        // Long press detected, start config portal
        startConfigPortalManually();
        break;
      }
    }
  }
}