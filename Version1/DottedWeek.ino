#define HOURS_IN_A_DAY 24
#define DAYS_IN_A_WEEK 7
#define DAYS_IN_A_YEAR 365
const int space_for_titles = DISPLAY_HEIGHT * 0.10;
const int d_height = DISPLAY_HEIGHT - space_for_titles;
const int d_width = DISPLAY_WIDTH - space_for_titles;
const int dot_size = (d_height + 1)/24;

coord dotted_week_get_spot(int p_hour, int p_wday) {
  coord spot;
  spot.y = ((p_hour*d_height)/HOURS_IN_A_DAY) + (d_height/(HOURS_IN_A_DAY*2)) + space_for_titles;
  spot.x = ((p_wday*d_width)/DAYS_IN_A_WEEK) + (d_width/(DAYS_IN_A_WEEK*2)) + space_for_titles;
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
  display.drawRect(spot.x - (dot_size/2), spot.y - (dot_size/2), dot_size, dot_size, GxEPD_BLACK);

  for(int i = 0; i < MAX_NUMBER_OF_DATA_POINTS; i++) {
    if(points[i].time_stamp == NULL) {
      continue;
    }
    (points[i].time_stamp).toCharArray(char_time, 30);
    strptime(char_time, "%Y-%m-%d %H:%M:%S", p_tm);
    the_time = mktime(p_tm);  // t is now your desired time_t
    //the_time = tsfstr(points[i].time_stamp);
    p_tm = localtime(&the_time);
    //Need to handle year roll over
    if (now_tm->tm_wday >= (now_tm->tm_yday - p_tm->tm_yday) + (365*(now_tm->tm_year - p_tm->tm_year))) {
      spot = dotted_week_get_spot(p_tm->tm_hour, now_tm->tm_wday);
      if (points[i].button == 'A') {
        display.fillCircle(spot.x, spot.y, (dot_size/2), GxEPD_BLACK); //button is A -> colour black
      } else {
        display.fillCircle(spot.x, spot.y, (dot_size/2), GxEPD_RED); //button is B -> colour yellow
      }
    }
  }

  for (int i = 0; i < 24; i++) {
    for(int j = 0; j < 7; j++) {
      if (! (i == now_tm->tm_hour && j == now_tm->tm_wday)) {
        spot = dotted_week_get_spot(i, j);
        display.drawCircle(spot.x, spot.y, (dot_size/2), GxEPD_BLACK);
      } else {
      }
    }
  }

  display.setCursor(0, 0);
 
  if (display_type == "4.2") {
    display.setTextColor(GxEPD_BLACK);
  } else {
     display.setTextColor(GxEPD_RED);
  }
  for( int i = 0; i < 24; i = i +3) {
    spot = dotted_week_get_spot( i, 0);
    display.setCursor(0, spot.y  - 2);
    display.println(i);
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


