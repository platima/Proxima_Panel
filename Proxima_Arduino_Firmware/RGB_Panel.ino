// RGB_Panel.ino for Proxima Arduino Firmware
// 2025 Platima (https://github.com/platima https://plati.ma)
// Handles RGB panel functions

void rgbPanelInit() {
  RGBpanel.begin(); 
  RGBpanel.show();  
}  

void rgbPanelSet(byte wait) {
  RGBpanel.setBrightness(rgbBrightnessLevel);

  for(byte index = 0; index < RGBpanel.numPixels(); index++){
    RGBpanel.setPixelColor(index, red, green, blue);
  }

  RGBpanel.show();
  
  // Non-blocking delay replacement
  if (wait > 0) {
    unsigned long delayStart = millis();
    while (millis() - delayStart < wait) {
      ESP.wdtFeed(); // Feed watchdog during wait
    }
  }
}