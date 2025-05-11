// Display_Init.ino for Proxima Arduino Firmware
// 2025 Platima (https://github.com/platima https://plati.ma)
// Handles OLED display initialisatio due to potential addresses

bool initDisplayWithScan() {
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
    
    delay(100); // Small delay between attempts
  }
  
  Serial.println("No OLED display found at any address. Continuing without display...");
  displayAvailable = false;
  return false;
}
