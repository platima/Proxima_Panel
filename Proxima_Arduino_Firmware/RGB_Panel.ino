// RGB_Panel.ino for Proxima Arduino Firmware
// 2025 Platima (https://github.com/platima https://plati.ma)
// Handles RGB panel functions

void rgbPanelInit() {
  RGBpanel.begin(); 
  RGBpanel.show();  
}  

void rgbPanelProcess() {
  if(rgbPanelMode == "Auto"){
      rgbPanelSet(0);
      display_mode_auto();
  }
}

void rgbPanelSet(byte wait) {
  RGBpanel.setBrightness(rgbBrightnessLevel);

  for(byte index = 0; index < RGBpanel.numPixels(); index++){
    RGBpanel.setPixelColor(index, red, green, blue);
  }

  RGBpanel.show();                          
  delay(wait);
}