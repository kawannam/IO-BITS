/*
 * 
 */

//--------------sizing and spacing consts--------------//
const int dot_rad = ((DISPLAY_HEIGHT - space_for_titles)/(HOURS_IN_A_DAY*2)) + .5;
const int dot_diam = dot_rad*2;
//-----------------------------------------------------//

void dotted_week() {
  coord spot;
  int checking_yday, checking_wday, colour;
  display.fillScreen(GxEPD_WHITE);

  struct tm point_tm;

  for(int i = 0; i < current_number_of_points; i++) {
    point_tm = *localtime(&points[i].timestamp);
    if (current_time_tm.tm_wday >= (current_time_tm.tm_yday - point_tm.tm_yday)) { 
      if (points[i].button == 'A') colour = GxEPD_BLACK;
      else colour = GxCOLOUR;
      spot = dotted_week_get_spot(point_tm.tm_hour, point_tm.tm_wday); 
      display.fillCircle(spot.x, spot.y, dot_rad, colour); 
    }
  }

  for (int i = 0; i <= HOURS_IN_A_DAY; i++) {
    for(int j = 0; j < DAYS_IN_A_WEEK; j++) {
      spot = dotted_week_get_spot(i, j);
      if ((i == current_time_tm.tm_hour) && (j == current_time_tm.tm_wday)) {
        display.drawRect(spot.x - dot_rad, spot.y - dot_rad, dot_diam + 1, dot_diam + 1, GxEPD_BLACK);
      } else {
        display.drawCircle(spot.x, spot.y, dot_rad, GxEPD_BLACK);
      }
    }
  }

  display.setFont(f7b);
  display.setTextColor(GxCOLOUR);
  uint16_t w, h;

  get_text_dimensions("0", f7b, &w, &h);
  for( int i = 0; i < HOURS_IN_A_DAY; i = i +3) {
    spot = dotted_week_get_spot( i, 0);
    display.setCursor(0, spot.y  + (h/4) );
    display.println(i);
  }
  
  for( int i = 0; i < DAYS_IN_A_WEEK; i++) {
    spot = dotted_week_get_spot(0, i);
    display.setCursor( spot.x - ((w*3)/4), h);
    display.println(days[i]);
  }
}

coord dotted_week_get_spot(int p_hour, int p_wday) {
  coord spot;
  int d_height = DISPLAY_HEIGHT - space_for_titles;
  int d_width = DISPLAY_WIDTH - space_for_titles;
  spot.y = ((p_hour*d_height)/HOURS_IN_A_DAY) + (d_height/(HOURS_IN_A_DAY*2)) + space_for_titles;
  spot.x = ((p_wday*d_width)/DAYS_IN_A_WEEK) + (d_width/(DAYS_IN_A_WEEK*2)) + space_for_titles;
  return spot;
}

