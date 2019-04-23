/*
 * tug_of_war: A visualization that highlights the ratio 
 * of button presses between two button. Has two rectangles
 * that push against each other to show the ratio. The %
 * will be displayed on both sides if their is enough room
 * 
 */

void tug_of_war() {
    uint16_t w, h;
    int ratio_A, count;
    String percent_A, percent_B;
    
    const GFXfont* small_font = f9;

    count = count_A + count_B;

    Serial.println("Count " + String(count));
    
    if (count == 0) {
      no_data_display();
      return;
    }

      ratio_A = ((count_A * DISPLAY_WIDTH) / count);           
      display.fillScreen(GxEPD_WHITE);
      display.fillRect(0, 0, ratio_A, DISPLAY_HEIGHT, GxCOLOUR);
      
      percent_A = String((count_A * 100) / count) + "%";
      get_text_dimensions(percent_A, small_font, &w, &h);
      if (ratio_A > w + EDGE_BUFFER) display_text_adv(percent_A, GxEPD_WHITE, small_font, EDGE_BUFFER, DISPLAY_HEIGHT - h);
      
      percent_B = String((count_B * 100) / count) + "%";
      get_text_dimensions(percent_B, small_font, &w, &h);
      if (DISPLAY_WIDTH - ratio_A > w + EDGE_BUFFER) display_text_adv(percent_B, GxCOLOUR, small_font, DISPLAY_WIDTH - w - EDGE_BUFFER, DISPLAY_HEIGHT - h);
}
