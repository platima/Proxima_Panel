// 
// WiFi_Connection.ino - Non-blocking WiFi connection implementation
// For Proxima LED Panel
// Version 0.2.0
//

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
const unsigned long WIFI_PORTAL_TIMEOUT = 180000; // 3 minutes timeout for portal (same as before)

WiFiManager wifiManager;
String apName = "";
int j = 0; // For WiFi animation
unsigned long lastAnimationTime = 0;
const unsigned long ANIMATION_INTERVAL = 500; // Animation frame interval in ms

void setupWiFi() {
  // Initialize WiFi state machine
  wifiConnState = WIFI_INIT;
  wifiLastStateChange = millis();
  
  // Set AP name with device chip ID
  apName = "Proxima-" + String(ESP.getChipId(), HEX);
  
  // Configure WiFiManager
  wifiManager.setConfigPortalTimeout(WIFI_PORTAL_TIMEOUT / 1000); // seconds
  
  // Start the connection process
  startWiFiConnection();
}

void startWiFiConnection() {
  wifiConnState = WIFI_CONNECTING;
  wifiLastStateChange = millis();
  
  // Display connecting animation
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Connecting to WiFi");
  display.display();
  
  // Begin connection attempt (non-blocking)
  WiFi.begin();
}

void handleWiFiConfigPortal() {
  // Only start portal if we're in the right state
  if (wifiConnState != WIFI_CONFIG_PORTAL) {
    wifiConnState = WIFI_CONFIG_PORTAL;
    wifiLastStateChange = millis();
    
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
    
    // Start config portal (non-blocking)
    wifiManager.startConfigPortal(apName.c_str());
  }
}

void processWiFiConnection() {
  // Handle animation timing
  unsigned long currentMillis = millis();
  if (currentMillis - lastAnimationTime >= ANIMATION_INTERVAL) {
    lastAnimationTime = currentMillis;
    
    // Update animation frame
    if (wifiConnState == WIFI_CONNECTING || wifiConnState == WIFI_CONFIG_PORTAL) {
      j = (j + 1) % 3;
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
        
        // Display connected info
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("Connected to WiFi");
        display.setCursor(0, 20);
        display.println("IP: " + WiFi.localIP().toString());
        display.display();
        
        Serial.println("WiFi connected");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        
        // Show connection info briefly, but don't block
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
        
        // Display connected info
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("Connected to WiFi");
        display.setCursor(0, 20);
        display.println("IP: " + WiFi.localIP().toString());
        display.display();
        
        Serial.println("WiFi connected via portal");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
      } 
      else if (millis() - wifiLastStateChange >= WIFI_PORTAL_TIMEOUT) {
        // Portal timeout
        wifiConnState = WIFI_FAILED;
        wifiLastStateChange = millis();
        
        // Display timeout message
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("WiFi Setup Failed");
        display.setCursor(0, 20);
        display.println("Continue without WiFi");
        display.display();
        
        Serial.println("WiFi portal timeout");
      } 
      else {
        // Still in portal, update display with animation
        Wifi_connection_animation();
      }
      break;
      
    case WIFI_CONNECTED:
      // Already connected, nothing to do
      break;
      
    case WIFI_FAILED:
      // Failed to connect, but continue with the rest of the program
      // Can attempt reconnection later if needed
      break;
  }
}

void Wifi_connection_animation() {
  // Only update the connection animation, don't redraw the whole screen
  display.fillRect(105, 0, 18, 8, BLACK); // Clear the animation area
  
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

void Wifi_connected_animation() {
  display.setTextSize(1);             
  display.setTextColor(WHITE);        
  display.setCursor(50, 0);             
  // Show IP address
  String ipAddress = WiFi.localIP().toString();
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
  if (digitalRead(btn_up) == HIGH && digitalRead(btn_down) == HIGH) {
    // Wait to see if it's a long press (about 3 seconds)
    unsigned long pressStart = millis();
    while (digitalRead(btn_up) == HIGH && digitalRead(btn_down) == HIGH) {
      // Show feedback on display
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
      
      if (millis() - pressStart > 3000) {
        // Long press detected, start config portal
        startConfigPortalManually();
        break;
      }
      delay(50);
    }
  }
}