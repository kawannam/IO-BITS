void plain_text() {
  
  Serial.println("Text Vis");
  if (count_A == 0 ) {
    plain_text_single_input_display(count_B, get_last_instance_timestamp('B'));
  } else if (count_B == 0) {
    plain_text_single_input_display(count_A, get_last_instance_timestamp('A'));
  } else {
    plain_text_two_input_display();
  }
  display.update();
}

String get_last_instance_timestamp(char button) {
   char buff[30];
  for (int i = 0; i < 10; i++) {
    Serial.print(button);
    Serial.println(points[i].button);
    if (points[i].button == button) {
    strftime(buff, 30, "%H:%M:%S %d/%m/%y", points[i].time_stamp);
      return buff;
    }
  }
  return "";
}

void plain_text_single_input_display(int count, String timestamp) {
  Serial.println("Single Value Text Vis");

  display.setRotation(1);
  
  display.fillScreen(GxEPD_WHITE);
  display.fillRect(0, 110, 154, 154, GxEPD_BLACK);

  
  display.setTextColor(GxEPD_WHITE);
  display.setFont(f9b);
  display.setCursor(5, 125);
  display.println(timestamp);

  display.setTextColor(GxEPD_BLACK);
  if (count > 999) {
    display.setFont(f30b);
    display.setCursor(10, 75);
  } else {
    display.setFont(f36b);
    display.setCursor(20, 90);
  }
  


  display.println(count);

  display.setFont(f9);

}

void plain_text_two_input_display() {
  Serial.println("Two Value Text Vis");
  display.fillScreen(GxEPD_BLACK);
  display.fillRect(75, 0, 154, 154, GxEPD_WHITE);

  display.setRotation(1);
  
  display.setTextColor(GxEPD_WHITE);
  display.setFont(f9b);
  display.setCursor(30, 140);
  display.println(get_last_instance_timestamp('A'));

  display.setTextColor(GxEPD_BLACK);
  display.setCursor(20, 60);
  display.println(get_last_instance_timestamp('B'));

  display.setFont(f30b);
  
  display.setCursor(10, 50);
  display.println(count_A);

  display.setTextColor(GxEPD_WHITE);
  if (count_B < 10) {
    display.setCursor(120, 130);
  } else if (count_B < 100) {
    display.setCursor(80, 130);
  } else if (count_B < 1000) {
    display.setCursor(50, 130);
  } else {
    display.setCursor(15, 130);
  }
  
  display.println(count_B);
}

