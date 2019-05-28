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

    int space_for_today = 0;
    if (!(days[A_INDEX][0] > 0 || days[B_INDEX][0] > 0)) {
      space_for_today =  2*LINE_WIDTH;
    }

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
      for (int i = 0; i <= streak+1; i++) {
        line_height_offset = DISPLAY_HEIGHT - ((streak-i-1)*2*LINE_WIDTH) - space_for_today;
        if (days[A_INDEX][i] > 0 && days[B_INDEX][i] > 0) {
          display.fillRect(0, line_height_offset - LINE_WIDTH, DISPLAY_WIDTH, LINE_WIDTH, GxEPD_BLACK);
        } else if (days[A_INDEX][i] > 0) {
          for (int k = 0; k < DISPLAY_WIDTH; k += (LINE_WIDTH*2)) {
            display.fillRect(k, line_height_offset - LINE_WIDTH, LINE_WIDTH, LINE_WIDTH, GxEPD_BLACK);
          }
        } else if (days[B_INDEX][i] > 0) {
          for (int k = 0; k < DISPLAY_WIDTH; k += (LINE_WIDTH*3)) {
            display.fillRect(k, line_height_offset - LINE_WIDTH, (2*LINE_WIDTH), LINE_WIDTH, GxEPD_BLACK);
          }
        }
      }
    }
  
    get_text_dimensions(String(streak), GxEPD_BLACK, &w, &h);
    display.fillRect(DISPLAY_WIDTH - w - w - EDGE_BUFFER, DISPLAY_HEIGHT - h - h - EDGE_BUFFER, DISPLAY_WIDTH, DISPLAY_HEIGHT, GxEPD_WHITE);
    display_text_adv(String(streak), GxEPD_BLACK, f7b, DISPLAY_WIDTH - w - w , DISPLAY_HEIGHT - h);
}
