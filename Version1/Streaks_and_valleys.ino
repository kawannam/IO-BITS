#define MAX_STREAK 7


long randNumber = 0;
int colour; 

void streaks_and_valleys() {
  Serial.println("Streaks and Valleys");
  if (display_type == "1.5bwr") {
    display.setRotation(1);
  } 
  int streak = get_streak();
  int dispaly_streak = streak;
  display.fillScreen(GxEPD_WHITE);
  if ( streak == 0) {
    return;
  } else if (streak < 0) {
     colour = GxEPD_RED;
     streak = streak *-1;
  } else {
    colour = GxEPD_BLACK;
  }
  if (streak > MAX_STREAK) {
    streak = MAX_STREAK;
  }
  Serial.println("here");
  for ( int i = 0; i < DISPLAY_WIDTH; i++) {
    for (int j = 0; j < DISPLAY_HEIGHT; j++) {
      randNumber =  random(99);
      if ( randNumber < (streak * (100/MAX_STREAK)) + 1) {
        display.drawPixel(i, j, colour);
      }
    }
  }

  display.fillRect(0, DISPLAY_HEIGHT - 29, 50, DISPLAY_HEIGHT, GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f7b);
  display.setCursor(8, DISPLAY_HEIGHT - 6);
  display.println(dispaly_streak);
  display.update();
}

int get_streak() {
  struct tm* point_time;
  char p_time[30];
  bool days[MAX_NUMBER_OF_DATA_POINTS];
  for (int i = 0; i < MAX_NUMBER_OF_DATA_POINTS; i++) {
    days[i] = false;
  }
  time_t now = time(nullptr);
  struct tm* now_tm = localtime(&now);
  int checking_day = now_tm->tm_yday - 1;
  int streak = 0;
  int index;
  for (int i = 0; i < MAX_NUMBER_OF_DATA_POINTS; i++) {
    if ( points[i].time_stamp != NULL ) {
      //(points[i].time_stamp).toCharArray(p_time, 30);
      //strptime(p_time, "%Y-%m-%d %H:%M:%S", &point_time);
      point_time = points[i].time_stamp;
      index = (now_tm->tm_yday - point_time->tm_yday) + ((now_tm->tm_year - point_time->tm_year)*365);
      if (index < MAX_NUMBER_OF_DATA_POINTS) {
        days[index] = true;
      }
    }
  }
  Serial.println("Streak array built");
  if (days[0]) {
    streak = 1;
    while(streak < MAX_NUMBER_OF_DATA_POINTS && days[streak]) {
      streak++;
    }
  } else {
    if (days[1]) {
      streak = 1;
      int count = streak + 1;
      while(count < MAX_NUMBER_OF_DATA_POINTS && days[count]) {
        streak++;
        count++;
      }
    } else {
      int count = 2;
      streak = -1;
      while(count < MAX_NUMBER_OF_DATA_POINTS && !days[count]) {
        streak--;
        count++;
      }
    }
  }
  Serial.println(streak);
  if (streak > -999) {
    return streak;
  }
  return 0;
}

