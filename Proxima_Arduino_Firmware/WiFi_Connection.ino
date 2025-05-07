void configModeCallback(WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
  
  // Display configuration mode on OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("WiFi Setup Mode");
  display.setCursor(0, 12);
  display.println("Connect to WiFi:");
  display.setCursor(0, 24);
  display.println(myWiFiManager->getConfigPortalSSID());
  display.setCursor(0, 36);
  display.println("Browse to:");
  display.setCursor(0, 48);
  display.println("192.168.4.1");
  display.display();
  
  // Show WiFi animation for config mode
  j = 0;
  Wifi_connection_animation();
}

void conn() {
  // Initialize WiFiManager
  WiFiManager wifiManager;
  
  // Set timeout for configuration portal (3 minutes)
  wifiManager.setConfigPortalTimeout(180);
  
  // Set custom AP name with device chip ID
  String apName = "Proxima-" + String(ESP.getChipId(), HEX);
  
  // Display connecting animation while attempting to connect
  wifiManager.setAPCallback(configModeCallback);
  
  Serial.println("Connecting to WiFi...");
  
  // Display connecting animation while attempting to connect
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Connecting to WiFi");
  display.display();
  
  // Connect to WiFi or start configuration portal
  if (wifiManager.autoConnect(apName.c_str())) {
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
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
    delay(2000); // Show the connection info briefly
  } else {
    Serial.println("Failed to connect and timeout occurred");
    // You could reset here if desired
    // ESP.restart();
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("WiFi Setup Failed");
    display.setCursor(0, 20);
    display.println("Please restart device");
    display.display();
    delay(5000);
  }
}
