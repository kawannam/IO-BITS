
//--------------sizing and spacing consts--------------//
const int space_for_titles = 15;
const int d_height = DISPLAY_HEIGHT - space_for_titles;
const int d_width = DISPLAY_WIDTH - space_for_titles;
const int dot_size = d_height/HOURS_IN_A_DAY;
//-----------------------------------------------------//

void dotted_week() {
  return;
  /*
  display.fillScreen(GxEPD_WHITE);

  coord spot;
  time_t now = time(nullptr);
  //struct tm* now_tm = localtime(&now);
  //struct tm* p_tm;
  time_t point_time;

  spot = dotted_week_get_spot(now.tm_hour, now.tm_wday);
  display.drawRect(spot.x - (dot_size/2), spot.y - (dot_size/2), dot_size, dot_size, GxEPD_BLACK);

  for(int i = 0; i < MAX_NUMBER_OF_DATA_POINTS; i++) {
    if(points[i].time_stamp == NULL) {
      continue;
    }

    point_time = points[i].time_stamp;
    //Need to handle year roll over
    if (now_tm.tm_wday >= (now_tm.tm_yday - p_tm.tm_yday) + (DAYS_IN_A_YEAR*(now_tm.tm_year - p_tm.tm_year))) {
      spot = dotted_week_get_spot(p_tm->tm_hour, now_tm->tm_wday);
      if (points[i].button == 'A') {
        display.fillCircle(spot.x, spot.y, (dot_size/2), GxEPD_BLACK); //button is A -> colour black
      } else {
        display.fillCircle(spot.x, spot.y, (dot_size/2), GxEPD_RED); //button is B -> colour yellow/red
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
  }*/
}
/*
coord dotted_week_get_spot(int p_hour, int p_wday) {
  coord spot;
  spot.y = ((p_hour*d_height)/HOURS_IN_A_DAY) + (d_height/(HOURS_IN_A_DAY*2)) + space_for_titles;
  spot.x = ((p_wday*d_width)/DAYS_IN_A_WEEK) + (d_width/(DAYS_IN_A_WEEK*2)) + space_for_titles;
  return spot;
}*/



