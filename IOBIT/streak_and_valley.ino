/*
 * streak_and_valley: A visualization that turns black the more
 * days in a row you hit the button and yellow the more days in
 * a row you miss (to a max of 7)
 */

void streaks_and_valleys() {
    uint16_t w, h;
    int streak, chance_threshold;
    long randNumber;
    randomSeed(analogRead(0));

    struct streaks my_streaks;
    bool days[3][MAX_NUMBER_OF_DAYS];

    
    get_streak(&my_streaks, days);

    streak = my_streaks.Total;
    Serial.println(String(my_streaks.Total));
    Serial.println(streak);

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
      for (int i = 0; i < streak; i++) {
        for (int j = 0; j < i + 1; j++)  {
          if (days[0][i]) {
            display.drawLine(0, i + 1, DISPLAY_WIDTH, i + 1, GxEPD_BLACK);
          } else if (days[1][i]) {
            display.drawLine(0, i + 1, DISPLAY_WIDTH, i + 1, GxEPD_BLACK);
          } else {
           display.drawLine(0, i + 1, DISPLAY_WIDTH, i + 1, GxEPD_BLACK);
          }
        }
      }
    }
  
    get_text_dimensions(String(streak), GxEPD_BLACK, &w, &h);
    display.fillRect(DISPLAY_WIDTH - w - w - EDGE_BUFFER, DISPLAY_HEIGHT - h - h - EDGE_BUFFER, DISPLAY_WIDTH, DISPLAY_HEIGHT, GxEPD_WHITE);
    display_text_adv(String(streak), GxEPD_BLACK, f7b, DISPLAY_WIDTH - w - w , DISPLAY_HEIGHT - h);
}
