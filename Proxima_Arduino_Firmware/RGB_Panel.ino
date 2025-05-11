void processRGBPanel(){
  if(panelMode == "Auto"){
      panelSet(0);
      display_mode_auto();
  }
}

void panelSet(byte wait){
  // Use brightness_Level directly (0-255 range)
  RGBpanel.setBrightness(brightness_Level);
  for(byte index = 0; index < RGBpanel.numPixels(); index++){
    RGBpanel.setPixelColor(index, red, green, blue);
    RGBpanel.show();                          
    delay(wait);
  }
}