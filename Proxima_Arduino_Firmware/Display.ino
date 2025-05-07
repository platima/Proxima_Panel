// Array for cursor position
byte cursorPosition[4] = {15, 25, 35, 45};
// cursorPosition[cursorIndex]
byte cursorIndex = 0;
byte menu_layer = 0;

void menu(){
  switch(menu_layer){
    case 0:
      menuLayer_0();
    case 1:
      menuLayer_1();
  }
}

void menuLayer_0(){
  if(digitalRead(btn_up) == HIGH){
   while(digitalRead(btn_up) == HIGH){}
   if(cursorIndex != 0){
    cursorIndex--;
   }  
  }

  if(digitalRead(btn_down) == HIGH){
   while(digitalRead(btn_down) == HIGH){}
   if(cursorIndex != 3){
    cursorIndex++;
   }  
  }

  if(digitalRead(btn_enter) == HIGH){
   while(digitalRead(btn_enter) == HIGH){}
   menu_layer = 1;
  }
}

void menuLayer_1(){
  while(digitalRead(btn_up) == HIGH){
    if(cursorIndex == 0){
      if(red < 255){
          red++;
        }
    }
    
    if(cursorIndex == 1){
      if(green < 255){
          green++;
        }
    }

    if(cursorIndex == 2){
      if(blue < 255){
          blue++;
        }
    }   

    if(cursorIndex == 3){
      while(digitalRead(btn_up) == HIGH){}
      brightness_Level++;
      if(brightness_Level > 9){
         brightness_Level = 9;
      }
    }
    delay(20);
  }

  while(digitalRead(btn_down) == HIGH){
    if(cursorIndex == 0){
      if(red > 0){
          red--;
        }
    }
    
    if(cursorIndex == 1){
      if(green > 0){
          green--;
        }
    }

    if(cursorIndex == 2){
      if(blue > 0){
          blue--;
        }
    }  

    if(cursorIndex == 3){
      while(digitalRead(btn_down) == HIGH){}
      if(brightness_Level > 0){
         brightness_Level--;
      }
    }
    delay(20);     
  }

  if(digitalRead(btn_enter) == HIGH){
   while(digitalRead(btn_enter) == HIGH){}
   menu_layer = 0;
  }
}

void display_mode_auto(){
  menu();
  // TOP
  display.clearDisplay();
  display.setTextSize(1);             
  display.setTextColor(WHITE);        
  display.setCursor(0,0);             
  display.println(panelMode);

  if(wifiStatus == true){
    Wifi_connected_animation();
  }

  // CURSOR
  display.setTextSize(1);             
  display.setTextColor(WHITE);        
  display.setCursor(0,cursorPosition[cursorIndex]);             
  display.println(">");
  // RED
  display.setTextSize(1);             
  display.setTextColor(WHITE);        
  display.setCursor(10,15);             
  display.println("R");
  display.drawFastHLine(20, 18, (red / 3), WHITE);
  // GREEN
  display.setTextSize(1);             
  display.setTextColor(WHITE);        
  display.setCursor(10,25);             
  display.println("G");
  display.drawFastHLine(20, 28, (green / 3), WHITE);
  // BLUE
  display.setTextSize(1);             
  display.setTextColor(WHITE);        
  display.setCursor(10,35);             
  display.println("B");
  display.drawFastHLine(20, 38, (blue / 3), WHITE);
  // BRIGHTNESS
  display.setTextSize(1);             
  display.setTextColor(WHITE);        
  display.setCursor(10,45);             
  display.println("LUX");
  display.drawFastHLine(30, 48, (brightness_Level * 7), WHITE);
  display.drawFastHLine(30, 49, (brightness_Level * 7), WHITE);
  display.drawFastHLine(30, 50, (brightness_Level * 7), WHITE);
  // SEND DATA TO DISPLAY
  display.display();
}

void Wifi_connected_animation(){
  display.setTextSize(1);             
  display.setTextColor(WHITE);        
  display.setCursor(50,0);             
  // Show IP address instead of just "WIFI"
  String ipAddress = WiFi.localIP().toString();
  display.println(ipAddress);
}

int j = 0;
void Wifi_connection_animation(){
  display_mode_auto();
  switch (j){
    case 0:
      display.drawBitmap(105, 0, bitmap_WIFI1, 18, 8, WHITE); 
      display.display();
    case 1:
      display.drawBitmap(105, 0, bitmap_WIFI2, 18, 8, WHITE); 
      display.display(); 
    case 2:
      display.drawBitmap(105, 0, bitmap_WIFI3, 18, 8, WHITE); 
      display.display();
      break;
  }
  
  if(j == 2){
    j = 0;
  }else{
    j++;
  } 
}

// Add a function to open the configuration portal on demand
// This could be bound to a long-press of a button if needed
void startConfigPortal() {
  // Initialize WiFiManager
  WiFiManager wifiManager;
  
  // Set timeout for configuration portal (3 minutes)
  wifiManager.setConfigPortalTimeout(180);
  
  // Display message when portal starts  
  Serial.println("Starting configuration portal");
  
  // Create a unique name for the config portal
  String apName = "Proxima-" + String(ESP.getChipId(), HEX);
  
  // Start config portal
  wifiManager.setAPCallback(configModeCallback);
  
  if (!wifiManager.startConfigPortal(apName.c_str())) {
    Serial.println("Failed to connect or timeout occurred");
  } else {
    // Connected to WiFi
    Serial.println("Connected to WiFi");
    wifiStatus = true;
  }
}
