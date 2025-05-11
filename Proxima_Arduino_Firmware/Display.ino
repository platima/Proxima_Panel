// Array for cursor position
byte cursorPosition[5] = {15, 25, 35, 45, 55}; // Added one more position for Reset option
// cursorPosition[cursorIndex]
byte cursorIndex = 0;
byte menu_layer = 0;

void menu(){
  switch(menu_layer){
    case 0:
      menuLayer_0();
      break; // Added break statement to prevent fallthrough
    case 1:
      menuLayer_1();
      break; // Added break statement
    case 2:
      menuLayer_2(); // Added a new menu layer for reset confirmation
      break;
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
   if(cursorIndex != 4){ // Changed to 4 to include the new Reset option
    cursorIndex++;
   }  
  }

  if(digitalRead(btn_enter) == HIGH){
   while(digitalRead(btn_enter) == HIGH){}
   if(cursorIndex == 4) { // Reset option
     menu_layer = 2; // Go to reset confirmation menu
   } else {
     menu_layer = 1; // Normal RGB/brightness menu
   }
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
      brightness_Level += 5; // Increment by 5 for 0-255 range
      if(brightness_Level > 255){
         brightness_Level = 255;
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
         brightness_Level -= 5; // Decrement by 5 for 0-255 range
      }
      if(brightness_Level < 0){
         brightness_Level = 0;
      }
    }
    delay(20);     
  }

  if(digitalRead(btn_enter) == HIGH){
   while(digitalRead(btn_enter) == HIGH){}
   menu_layer = 0;
   // Save settings immediately after exiting this menu
   saveSettings();
  }
}

// New menu layer for reset confirmation
void menuLayer_2() {
  static byte confirmCursor = 0; // 0 = No, 1 = Yes
  
  if(digitalRead(btn_up) == HIGH || digitalRead(btn_down) == HIGH){
    while(digitalRead(btn_up) == HIGH || digitalRead(btn_down) == HIGH){}
    confirmCursor = !confirmCursor; // Toggle between 0 and 1
  }
  
  if(digitalRead(btn_enter) == HIGH){
    while(digitalRead(btn_enter) == HIGH){}
    if(confirmCursor == 1) { // Yes was selected
      resetSettings();
      // Apply reset settings immediately
      panelSet(0);
    }
    // Return to main menu regardless of choice
    menu_layer = 0;
    confirmCursor = 0; // Reset for next time
  }
  
  // Display reset confirmation screen
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Reset Settings?");
  
  display.setCursor(30, 20);
  display.println("No");
  display.setCursor(30, 35);
  display.println("Yes");
  
  // Show cursor
  display.setCursor(20, 20 + (confirmCursor * 15));
  display.println(">");
  
  display.display();
}

void display_mode_auto(){
  menu();
  
  // Only draw the regular menu if we're not in reset confirmation
  if(menu_layer != 2) {
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
    // BRIGHTNESS - Now shows 0-255 range
    display.setTextSize(1);             
    display.setTextColor(WHITE);        
    display.setCursor(10,45);             
    display.println("LUX");
    // Draw brightness bar, scale down for display (255/3 = 85 max length)
    int brightnessLength = brightness_Level / 3;
    display.drawFastHLine(30, 48, brightnessLength, WHITE);
    display.drawFastHLine(30, 49, brightnessLength, WHITE);
    display.drawFastHLine(30, 50, brightnessLength, WHITE);
    
    // Show actual brightness value for clarity
    display.setCursor(75, 45);
    display.print(brightness_Level);
    
    // RESET OPTION
    display.setTextSize(1);             
    display.setTextColor(WHITE);        
    display.setCursor(10,55);             
    display.println("RESET");
    
    // SEND DATA TO DISPLAY
    display.display();
  }
}