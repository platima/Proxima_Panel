// Display_Init.ino for Proxima Arduino Firmware
// 2025 Platima (https://github.com/platima https://plati.ma)
// Handles OLED display initialisation due to potential addresses

ICACHE_FLASH_ATTR bool OLED_Init() {
  Serial.println("Scanning for OLED display...");
  
  for (int i = 0; i < numAddresses; i++) {
    Serial.print("Trying I2C address 0x");
    Serial.println(addresses[i], HEX);
    
    // Try to initialize the display at this address
    if (display.begin(addresses[i], true)) {
      Serial.print("OLED display found at address 0x");
      Serial.println(addresses[i], HEX);
      displayAvailable = true;
      return true;
    }
    
    // Non-blocking delay between attempts
    unsigned long delayStart = millis();
    while (millis() - delayStart < 100) {
      ESP.wdtFeed(); // Feed watchdog during wait
    }
  }
  
  Serial.println("No OLED display found at any address. Continuing without display...");
  displayAvailable = false;
  return false;
}