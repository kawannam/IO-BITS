/*
 * streak_and_valley: A visualization that turns black the more
 * days in a row you hit the button and yellow the more days in
 * a row you miss (to a max of 7)
 */

 #define LINE_WIDTH 10

void streaks_and_valleys() {
    uint16_t w, h;
    int streak, chance_threshold;
    long randNumber;
    randomSeed(analogRead(0));

    struct streaks my_streaks;
    int days[3][MAX_NUMBER_OF_DAYS];

    
    get_streak(&my_streaks, days);

    streak = my_streaks.Total;

    display.fillScreen(GxEPD_WHITE);
    if (streak < 0) {
      chance_threshold =   (abs(streak) * (100/MAX_STREAK));
      for ( int i = 0; i < DISPLAY_WIDTH; i++) {
        for (int j = 0; j < DISPLAY_HEIGHT; j++) {
          randNumber =  random(99);
          if ( randNumber < chance_threshold) display.drawPixel(i, j, GxEPD_BLACK);
        }
      }
    }
    else {
      int line_height = 0;
      int line_height_offset = 0;
      for (int i = 0; i < streak; i++) {
            line_height_offset = DISPLAY_HEIGHT - ((streak-i-1)*2*LINE_WIDTH);
            
            
        for (int j = 0; j < LINE_WIDTH; j++)  {
          line_height = line_height_offset - j;          
          if (days[A_INDEX][i] > 0 && days[B_INDEX][i] > 0) {
            display.drawLine(0, line_height, DISPLAY_WIDTH, line_height, GxEPD_BLACK);
          } else if (days[A_INDEX][i] > 0) {
            for (int k = 0; k < DISPLAY_WIDTH; k += (LINE_WIDTH*2)) {
              display.drawLine(k, line_height, k+LINE_WIDTH, line_height, GxEPD_BLACK);
            }
          } else if (days[B_INDEX][i] > 0) {
            for (int k = 0; k < DISPLAY_WIDTH; k += (LINE_WIDTH*3)) {
              display.drawLine(k, line_height, k+(2*LINE_WIDTH), line_height, GxEPD_BLACK);
            }
          }
        }
      }
    }
  
    get_text_dimensions(String(streak), GxEPD_BLACK, &w, &h);
    display.fillRect(DISPLAY_WIDTH - w - w - EDGE_BUFFER, DISPLAY_HEIGHT - h - h - EDGE_BUFFER, DISPLAY_WIDTH, DISPLAY_HEIGHT, GxEPD_WHITE);
    display_text_adv(String(streak), GxEPD_BLACK, f7b, DISPLAY_WIDTH - w - w , DISPLAY_HEIGHT - h);
}
