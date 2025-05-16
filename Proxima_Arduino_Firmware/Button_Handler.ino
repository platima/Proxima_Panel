// Button_Handler.ino for Proxima Arduino Firmware
// 2025 Platima (https://github.com/platima https://plati.ma)
// Handles button input with Bounce2 library

// Timing variables for button handling
unsigned long lastButtonUpdate = 0;
const unsigned long BUTTON_UPDATE_INTERVAL = 10; // Update buttons every 10ms

// Button pressed duration tracking for long presses
unsigned long upPressStart = 0;
unsigned long downPressStart = 0;
unsigned long enterPressStart = 0;
bool upLongPressActive = false;
bool downLongPressActive = false;
bool enterLongPressActive = false;

void handleButtons() {
  // Feed watchdog timer first thing
  ESP.wdtFeed();
  
  unsigned long currentTime = millis();
  
  // ------ UP BUTTON ------
  // Initial press detection (Bounce2 handles this perfectly)
  if (upButton.pressed()) {
    upPressStart = currentTime;
    upLongPressActive = false;
    // Handle immediate button press
    handleUpPress(false);
  }
  
  // Released detection
  if (upButton.released()) {
    upLongPressActive = false;
    upPressStart = 0;
  }
  
  // Long press detection
  if (upButton.read() == HIGH && upPressStart > 0) {
    // First time we've held long enough?
    if (!upLongPressActive && (currentTime - upPressStart > 300)) {
      upLongPressActive = true;
      handleUpPress(true); // First long press action
      lastUpRepeatTime = currentTime; // Initialize repeat timer
    }
    
    // Auto-repeat for long press
    if (upLongPressActive && (currentTime - lastUpRepeatTime > BUTTON_REPEAT)) {
      lastUpRepeatTime = currentTime;
      handleUpPress(true); // Repeated long press action
    }
  }
  
  // ------ DOWN BUTTON ------
  // Same pattern for down button
  if (downButton.pressed()) {
    downPressStart = currentTime;
    downLongPressActive = false;
    handleDownPress(false);
  }
  
  if (downButton.released()) {
    downLongPressActive = false;
    downPressStart = 0;
  }
  
  if (downButton.read() == HIGH && downPressStart > 0) {
    if (!downLongPressActive && (currentTime - downPressStart > 300)) {
      downLongPressActive = true;
      handleDownPress(true);
      lastDownRepeatTime = currentTime;
    }
    
    if (downLongPressActive && (currentTime - lastDownRepeatTime > BUTTON_REPEAT)) {
      lastDownRepeatTime = currentTime;
      handleDownPress(true);
    }
  }
  
  // ------ ENTER BUTTON ------
  // Simpler handling for enter - no auto-repeat needed
  if (enterButton.pressed()) {
    enterPressStart = currentTime;
    handleEnterPress();
  }
  
  if (enterButton.released()) {
    enterPressStart = 0;
  }
}

void handleUpPress(bool longPress) {
  // Implement based on current menu layer
  switch(menuLayer) {
    case 0:
      if (cursorIndex != 0) {
        cursorIndex--;
      }
      break;
      
    case 1:
      // RGB/Brightness adjustment
      if(cursorIndex == 0 && red < 255) {
        red++;
      }
      if(cursorIndex == 1 && green < 255) {
        green++;
      }
      if(cursorIndex == 2 && blue < 255) {
        blue++;
      }
      if(cursorIndex == 3 && rgbBrightnessLevel < 255) {
        rgbBrightnessLevel += (longPress ? 5 : 1);
        if(rgbBrightnessLevel > 255) {
          rgbBrightnessLevel = 255;
        }
      }
      
      // Update display and LEDs immediately
      if (currentAnimation == STATIC) {
        rgbPanelSet(0);
      }
      break;
      
    case 2:
      // Reset confirmation
      // Handled elsewhere
      break;
  }
}

void handleDownPress(bool longPress) {
  switch(menuLayer) {
    case 0:
      if (cursorIndex != 4) {
        cursorIndex++;
      }
      break;
      
    case 1:
      // RGB/Brightness adjustment
      if(cursorIndex == 0 && red > 0) {
        red--;
      }
      if(cursorIndex == 1 && green > 0) {
        green--;
      }
      if(cursorIndex == 2 && blue > 0) {
        blue--;
      }
      if(cursorIndex == 3 && rgbBrightnessLevel > 0) {
        rgbBrightnessLevel -= (longPress ? 5 : 1);
        if(rgbBrightnessLevel < 0) {
          rgbBrightnessLevel = 0;
        }
      }
      
      // Update display and LEDs immediately
      if (currentAnimation == STATIC) {
        rgbPanelSet(0);
      }
      break;
      
    case 2:
      // Reset confirmation
      // Handled elsewhere
      break;
  }
}

void handleEnterPress() {
  switch(menuLayer) {
    case 0:
      if(cursorIndex == 4) { // Reset option
        menuLayer = 2; // Go to reset confirmation menu
      } else {
        menuLayer = 1; // Normal RGB/brightness menu
      }
      break;
      
    case 1:
      menuLayer = 0;
      // Save settings immediately after exiting this menu
      saveSettings();
      break;
      
    case 2:
      // Reset confirmation handled elsewhere
      break;
  }
}