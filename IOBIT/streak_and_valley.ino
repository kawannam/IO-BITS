/*
 * streak_and_valley: A visualization that turns black the more
 * days in a row you hit the button and yellow the more days in
 * a row you miss (to a max of 7)
 */


#define MAX_STREAK 7
#define MAX_NUMBER_OF_DAYS 500


void streaks_and_valleys() {
    uint16_t w, h;
    int streak, chance_threshold, colour;
    long randNumber;
    randomSeed(analogRead(0));
    
    streak = get_streak();
    if ( streak <= (MAX_NUMBER_OF_DAYS*-1)) {
      no_data_display();
      return;
    }

    display.fillScreen(GxEPD_WHITE);
    if ( streak == 0) colour = GxEPD_WHITE;
    else if (streak < 0) colour = GxEPD_RED;
    else colour = GxEPD_BLACK;
  
    chance_threshold =   (abs(streak) * (100/MAX_STREAK));
    for ( int i = 0; i < DISPLAY_WIDTH; i++) {
      for (int j = 0; j < DISPLAY_HEIGHT; j++) {
        randNumber =  random(99);
        if ( randNumber < chance_threshold) display.drawPixel(i, j, colour);
      }
    }
  
  get_text_dimensions(String(streak), GxEPD_BLACK, &w, &h);
  display.fillRect(DISPLAY_WIDTH - w - w - EDGE_BUFFER, DISPLAY_HEIGHT - h - h - EDGE_BUFFER, DISPLAY_WIDTH, DISPLAY_HEIGHT, GxEPD_WHITE);
  display_text_adv(String(streak), GxEPD_BLACK, f7b, DISPLAY_WIDTH - w - w , DISPLAY_HEIGHT - h);
}

int get_streak() {
  struct tm* now_tm;
  struct tm* point_tm;
  int checking_day, checking_year, index, streak;
  bool days[MAX_NUMBER_OF_DAYS];
  for (int i = 0; i < MAX_NUMBER_OF_DAYS; i++) days[i] = false;

  time_t now = time(nullptr);
  now_tm = localtime(&now);
  checking_day = now_tm->tm_yday;
  checking_year = now_tm->tm_year;
  
  Serial.println("NOW " + String(now_tm->tm_yday) + " YEAR " + String(now_tm->tm_year));
  for (int i = 0; i < current_number_of_points; i++) {
    if ( points[i].timestamp != NULL ) {
      point_tm = localtime(&points[i].timestamp);
      index = (checking_day - point_tm->tm_yday) + ((checking_year - point_tm->tm_year)*DAYS_IN_A_YEAR);
      if (index < MAX_NUMBER_OF_DAYS) days[index] = true;
    }
  }

  streak = 1;
  while( (streak < MAX_NUMBER_OF_DAYS) && (days[streak] == days[1])) streak++;
  if (!days[1]) streak = streak * -1;
  if ((days[0]) && (streak < 0)) streak = 0;
  if (days[0]) streak++;
  return streak;
}

