  int space_for_titles_hour = 8;
  int space_for_titles_week = 8;
  
  int d_height = DISPLAY_HEIGHT - (5*2);
  int d_width = DISPLAY_WIDTH - space_for_titles_hour;

coord dotted_week_get_spot(int p_hour, int p_wday) {
  p_hour = 24 - p_hour;
  coord spot;
  spot.y = ((p_hour*d_height)/24) + (d_height/48) - space_for_titles_week;
  spot.x = ((p_wday*d_width)/7) + (d_width/14) + space_for_titles_hour;
  return spot;
}

void dotted_week() {
  
  Serial.println("DottedDisplay");
  display.fillScreen(GxEPD_WHITE);

  coord spot;
  time_t now = time(nullptr);
  struct tm* now_tm = localtime(&now);
  time_t the_time;
  struct tm* p_tm;
  char char_time[100];

  spot = dotted_week_get_spot(now_tm->tm_hour, now_tm->tm_wday);
  display.drawRect(spot.y - 3, spot.x - 3, 7, 7, GxEPD_BLACK);

  for(int i = 0; i < 10; i++) {
      (points[i].time_stamp).toCharArray(char_time, 30);
      strptime(char_time, "%Y-%m-%d %H:%M:%S", p_tm);
      the_time = mktime(p_tm);  // t is now your desired time_t
      //the_time = tsfstr(points[i].time_stamp);
      p_tm = localtime(&the_time);
      //Need to handle year roll over
      if (p_tm->tm_wday <= (now_tm->tm_yday - p_tm->tm_yday)) {
    //if (now_tm->tm_wday >= p_tm->tm_wday &&  (now_tm->tm_yday - p_tm->tm_yday < 6)) { //is less than seven days from point
      spot = dotted_week_get_spot(p_tm->tm_hour, now_tm->tm_wday);
      if (points[i].button == 'A') {
        display.fillCircle(spot.y, spot.x, 3, GxEPD_BLACK); //button is A -> colour black
      } else {
        display.fillCircle(spot.y, spot.x, 3, GxEPD_RED); //button is B -> colour yellow
      }
    }
  }

  for (int i = 0; i < 24; i++) {
    for(int j = 0; j < 7; j++) {
      if (! (i == now_tm->tm_hour && j == now_tm->tm_wday)) {
        spot = dotted_week_get_spot(i, j);
        display.drawCircle(spot.y, spot.x, 3, GxEPD_BLACK);
      } else {
      }
    }
  }

  display.setCursor(0, 0);
  display.setTextColor(GxEPD_RED);
  display.setRotation(1);
  for( int i = 0; i < 24; i = i +3) {
    spot = dotted_week_get_spot( i+1, 0);
    display.setCursor(0, spot.y + 2);
    display.println(24-i);
  }
  char days[7] = {'U', 'M', 'T', 'W', 'R', 'F', 'S'};
  for( int i = 0; i < 7; i++) {
    spot = dotted_week_get_spot(0, i);
    display.setCursor( spot.x - 2, 0);
    display.println(days[i]);
  }
  
   display.update();
   //Put the chip to sleep. 
   ESP.deepSleep(6e7); // 20e6 is 20 microseconds
}


