  //int space_for_titles = 15;
  
  int s_height = DISPLAY_HEIGHT - space_for_titles;
  int s_width = DISPLAY_WIDTH - space_for_titles;

coord stacked_bar_get_spot(int p_hour, int p_wday) {
  coord spot;
  spot.x = ((p_wday*s_width)/7) + (s_width/14) + space_for_titles;
  spot.y = ((p_hour*s_height)/24) + (s_height/48) + space_for_titles;
  return spot;
}

void draw_time_interval(tm* start_time, tm* end_time) {
  int x = (start_time->tm_wday * s_width)/7 + space_for_titles + 1;
  //int y = start_time->tm_hour * (s_height/24) + space_for_titles;
  int y = ((start_time->tm_hour*s_height)/24) + space_for_titles + 1;
  if (start_time->tm_yday == end_time->tm_yday) {
    int height = ((start_time->tm_hour - end_time->tm_hour)* s_height)/-24;
    display.fillRect( x, y, (s_width)/7, height, GxEPD_RED);
  } else {
    display.fillRect( x, y, (s_width)/7 - 1, s_height - y, GxEPD_RED);
    int d_day = start_time->tm_wday + 1;
    while (d_day < end_time->tm_wday) {
      display.fillRect( ((d_day*s_width)/7) + space_for_titles, space_for_titles, (s_width)/7 - 1, s_height, GxEPD_RED);
      d_day++;
    }
    int height = (end_time->tm_hour* s_height)/24 + space_for_titles;
    int x2 = (end_time->tm_wday * s_width)/7 + space_for_titles + 1;
    display.fillRect( x2, space_for_titles+2, (s_width)/7, height, GxEPD_RED);
  }
}

void stacked_bars() {
  Serial.println("Stacked Bars");
  
  display.setRotation(1);
  display.fillScreen(GxEPD_WHITE);

  //Draw graph space (titles not included)
  display.drawRect(space_for_titles, space_for_titles, s_height- 2, s_width - 2, GxEPD_BLACK);

  //Add titles
  display.setTextColor(GxEPD_RED);
  coord spot;
  for( int i = 0; i < 24; i = i + 3) {
    spot = stacked_bar_get_spot(i, 0);
    display.setCursor(0, spot.y - 3);
    display.println(i);
    display.drawLine(space_for_titles, (((i*s_height)/24) + space_for_titles), DISPLAY_WIDTH, (((i*s_height)/24) + space_for_titles), GxEPD_BLACK);
  }
  char days[7] = {'U', 'M', 'T', 'W', 'R', 'F', 'S'};
  for( int i = 0; i < 7; i++) {
    spot = stacked_bar_get_spot(0, i);
    display.setCursor( spot.x - 2, 0);

    display.println(days[i]);
    display.drawLine(((i*s_width)/7) + space_for_titles, space_for_titles, ((i*s_width)/7) + space_for_titles, DISPLAY_HEIGHT, GxEPD_BLACK);
  }

  //Fill in data
  time_t now = time(nullptr);
  struct tm* now_tm = localtime(&now);

  for(int i = 0; i < MAX_NUMBER_OF_DATA_POINTS - 1; i = i +2) {
    struct tm start_time;
    struct tm end_time;
    char s_time[30];
    char e_time[30];
      
    int j = i + 1;

    if ( points[i].time_stamp != NULL ) {
    (points[i].time_stamp).toCharArray(s_time, 30);
    strptime(s_time, "%Y-%m-%d %H:%M:%S", &start_time);
    if (points[j].time_stamp != NULL ) {
    (points[j].time_stamp).toCharArray(e_time, 30);
    strptime(e_time, "%Y-%m-%d %H:%M:%S", &end_time);
    } else {
      end_time = *now_tm;
    }
    if (start_time.tm_wday >= (start_time.tm_yday - end_time.tm_yday)) {
      draw_time_interval(&start_time, &end_time);
      spot = stacked_bar_get_spot(start_time.tm_hour, end_time.tm_wday);
      //if (points[i].button == 'A') {
    } else {
      Serial.println(start_time.tm_wday);
      Serial.println(start_time.tm_yday);
      Serial.println(end_time.tm_yday);
    }
  }
  }

    spot = dotted_week_get_spot(now_tm->tm_hour, now_tm->tm_wday);
  int x = (now_tm->tm_wday * (s_width/7)) + (s_width/14) + space_for_titles - 1;
  int y = (now_tm->tm_hour * (s_height/24)) + (s_height/48) + space_for_titles + 3;

  if (((count_A + count_B) % 2) == 0) {
    display.drawRect(x, y, 7, 7, GxEPD_BLACK);
  } else {
    display.fillRect(x, y, 7, 7, GxEPD_RED);
    display.drawRect(x, y, 7, 7, GxEPD_BLACK);
  }
   display.update();
   //Put the chip to sleep. 
   ESP.deepSleep(6e7); // 20e6 is 20 microseconds
}


