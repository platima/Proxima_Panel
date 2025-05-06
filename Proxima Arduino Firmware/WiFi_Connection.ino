void conn(){
  // Use the WiFiManager to handle connection
  wifiStatus = connectWiFi();
  
  // If WiFi is not connected, show appropriate message on display
  if (!wifiStatus) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("WiFi Not Connected");
    display.println("Use Portal to Setup");
    display.display();
  }
}

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
