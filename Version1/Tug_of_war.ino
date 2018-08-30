void tug_of_war() {

    //Add font/ writting 
      display.setFont(f9);
      display.setRotation(1);

    if (count_A == 0 && count_B == 0) {
      display.fillScreen(GxEPD_WHITE);
      display.setCursor(70, 70);
      display.setTextColor(GxEPD_RED);
      display.println("No Data");
      
      display.update();
    } else {
      float ratio_A = ((count_A * DISPLAY_WIDTH) / (count_A + count_B));
      int percent_A = ((count_A * 100) / (count_A + count_B));
      int percent_B = ((count_B * 100) / (count_A + count_B));
  
      Serial.println("Math Done: " + String(percent_A) + " " + String(percent_B));
      
      display.fillScreen(GxEPD_BLACK);
      display.fillRect(0, 0, ratio_A, DISPLAY_HEIGHT, GxEPD_RED);

      if ( percent_A >= 25) {
        display.setCursor(5, 140);
        display.setTextColor(GxEPD_BLACK);
        display.println(String(percent_A) + "%");
      }

      if (percent_B >= 25) {
        display.setCursor(105, 140);
        display.setTextColor(GxEPD_RED);
        display.println(String(percent_B) + "%");
      }
  
      //Push changes to the display
      display.update();
  
      Serial.println("Drawing Done");
    }
  
    //Put the chip to sleep. 
    ESP.deepSleep(6e7); // 20e6 is 20 microseconds
    
}
