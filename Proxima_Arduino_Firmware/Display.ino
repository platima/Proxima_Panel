// Display.ino for Proxima Arduino Firmware
// 2025 Platima (https://github.com/platima https://plati.ma)
// Handles OLED display functions


// New menu layer for reset confirmation
void menuLayer_2() {
  static byte confirmCursor = 0; // 0 = No, 1 = Yes
  static unsigned long holdStartTime = 0;
  static bool resetConfirmed = false;
  
  // Handle buttons
  if(upButton.pressed() || downButton.pressed()) {
    confirmCursor = !confirmCursor; // Toggle between 0 and 1
  }
  
  if(enterButton.pressed()) {
    if(confirmCursor == 1) { // Yes was selected
      if(!resetConfirmed) {
        // Start 3-second hold timer
        holdStartTime = millis();
        resetConfirmed = true;
      }
    } else {
      // Return to main menu
      menuLayer = 0;
      confirmCursor = 0;
      resetConfirmed = false;
    }
  }
  
  // Check for held enter button
  if(resetConfirmed && enterButton.read() == HIGH) {
    if(millis() - holdStartTime >= 3000) {
      // 3 seconds passed, perform reset
      resetSettings();

      // Apply reset settings immediately
      rgbPanelSet(0);
      
      // Show confirmation
      if(displayAvailable) {
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 20);
        display.println("Settings Reset!");
        display.display();
      }
      
      // Wait a moment, then return to main menu
      unsigned long confirmStart = millis();
      while(millis() - confirmStart < 2000) {
        ESP.wdtFeed();
      }
      
      menuLayer = 0;
      confirmCursor = 0;
      resetConfirmed = false;
    }
  }
  
  // Handle button release
  if(enterButton.released() && resetConfirmed) {
    // Button released before 3 seconds
    holdStartTime = 0;
    resetConfirmed = false;
  }
  
  // Display reset confirmation screen
  if(displayAvailable) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    
    if(resetConfirmed) {
      display.println("Hold Enter...");
      display.setCursor(0, 15);
      display.println("(" + String((3000 - (millis() - holdStartTime)) / 1000 + 1) + " sec)");
    } else {
      display.println("Reset Settings?");
    }
    
    display.setCursor(30, 30);
    display.println("No");
    display.setCursor(30, 45);
    display.println("Yes");
    
    // Show cursor
    display.setCursor(20, 30 + (confirmCursor * 15));
    display.println(">");
    
    display.display();
  }
}

void displayMain(){
  if (!displayAvailable) return; // Skip if no display

  unsigned long currentTime = millis();
  
  // Throttle display updates to DISPLAY_UPDATE_INTERVAL
  if (currentTime - lastDisplayUpdateTime < DISPLAY_UPDATE_INTERVAL) {
    return;
  }
  lastDisplayUpdateTime = currentTime;

  // Only draw the regular menu if we're not in reset confirmation
  if(menuLayer != 2) {
    // TOP
    display.clearDisplay();
    display.setTextSize(1);             
    display.setTextColor(WHITE);        
    display.setCursor(0,0);
  
    if(wifiStatus == true){
      Wifi_OLED_ShowIP();
    }
  
    // CURSOR
    display.setTextSize(1);             
    display.setTextColor(WHITE);        
    display.setCursor(0, cursorPosition[cursorIndex]);             
    display.println(">");
    // RED
    display.setTextSize(1);             
    display.setTextColor(WHITE);        
    display.setCursor(10,OLED_LINE_1);             
    display.println("R");
    display.drawFastHLine(20, OLED_LINE_1+3, (red / 3), WHITE);
    // GREEN
    display.setTextSize(1);             
    display.setTextColor(WHITE);        
    display.setCursor(10,OLED_LINE_2);             
    display.println("G");
    display.drawFastHLine(20, OLED_LINE_2+3, (green / 3), WHITE);
    // BLUE
    display.setTextSize(1);             
    display.setTextColor(WHITE);        
    display.setCursor(10,25);             
    display.println("B");
    display.drawFastHLine(20, OLED_LINE_3+3, (blue / 3), WHITE);

    // BRIGHTNESS - Now shows 0-255 range
    display.setTextSize(1);             
    display.setTextColor(WHITE);        
    display.setCursor(10, OLED_LINE_4);             
    display.println("LUX");
    
    // Draw brightness bar, scale down for display (255/3 = 85 max length)
    int brightnessLength = rgbBrightnessLevel / 3;
    display.drawFastHLine(30, OLED_LINE_4+3, brightnessLength, WHITE);
    display.drawFastHLine(30, OLED_LINE_4+4, brightnessLength, WHITE);
    display.drawFastHLine(30, OLED_LINE_4+5, brightnessLength, WHITE);
    
    // Show actual brightness value for clarity
    display.setCursor(109, OLED_LINE_4);
    display.print(rgbBrightnessLevel);
    
    // RESET OPTION
    display.setTextSize(1);             
    display.setTextColor(WHITE);        
    display.setCursor(10, OLED_LINE_5);
    display.println("RESET");
    
    // SEND DATA TO DISPLAY
    display.display();
  } else {
    menuLayer_2();
  }
}