/*
 * plain_text: a simple visualization where the count of each button is
 * displayed in large font and the last time it was pressed is display 
 * under it
 */


#define EDGE_BUFFER 5


void plain_text() {
  const GFXfont* large_font = f30b;
  const GFXfont* small_font = f7b;

  uint16_t s_w, s_h, l_w, l_h;

  String time_A = get_last_instance_timestamp('A');
  String time_B = get_last_instance_timestamp('B');

  get_text_dimensions(String(count_B), large_font, &l_w, &l_h);
  get_text_dimensions(String(time_B), small_font, &s_w, &s_h);
  
  display.fillScreen(GxEPD_WHITE);
  display.fillRect( 0, (DISPLAY_HEIGHT/2), DISPLAY_WIDTH, (DISPLAY_HEIGHT/2) + 1, GxEPD_BLACK);
  
  display_text_adv(String(count_A), GxEPD_BLACK, large_font, EDGE_BUFFER, l_h + EDGE_BUFFER);
  display_text_adv(String(time_A), GxEPD_BLACK, small_font, EDGE_BUFFER, s_h + l_h + EDGE_BUFFER + EDGE_BUFFER);
  display_text_adv(String(time_B), GxEPD_WHITE, small_font, (DISPLAY_WIDTH - s_w - EDGE_BUFFER), (DISPLAY_HEIGHT - s_h));
  display_text_adv(String(count_B), GxEPD_WHITE, large_font, (DISPLAY_WIDTH - l_w - EDGE_BUFFER), (DISPLAY_HEIGHT - s_h - s_h - EDGE_BUFFER));
}


String get_last_instance_timestamp(char button) {
  Serial.println("get_last_instance_timestamp");
  char buff[30];
  for (int i = 0; i < 10; i++) {
    time_to_string(points[i].timestamp, buff);
    if (points[i].button == button) {
      time_to_string_display(points[i].timestamp, buff);
      return String(buff);
    }
  }
  return "No Recent Logs";
}

