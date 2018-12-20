/*
 * 
 */
#define NOW_BOX_SIDE 7

int h_section = (DISPLAY_HEIGHT - space_for_titles)/(HOURS_IN_A_DAY); 
int w_section = (DISPLAY_WIDTH - space_for_titles)/DAYS_IN_A_WEEK;


void stacked_bars() {
  int x, y;
  uint16_t w, h;
  struct tm start_time;
  struct tm end_time;

  time_t now = time(nullptr);
  struct tm now_tm = *localtime(&now);

  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_RED);
  display.setFont();

  get_text_dimensions("W", NULL, &w, &h);

  for (int i = 1; i < current_number_of_points; i = i+2) {
    start_time = *localtime(&points[i-1].timestamp);
    end_time = *localtime(&points[i].timestamp);

    if (now_tm.tm_wday >= (start_time.tm_yday - end_time.tm_yday)) {
      draw_time_interval(start_time, end_time);
    }
  }

  x = (now_tm.tm_wday * w_section) + ((w_section - NOW_BOX_SIDE)/2) + space_for_titles;
  y = (now_tm.tm_hour * h_section) + ((h_section - NOW_BOX_SIDE)/2) + space_for_titles;
  if (((count_A + count_B) % 2) != 0) {
    start_time = *(localtime(&points[current_number_of_points - 1].timestamp));
    draw_time_interval(start_time, now_tm);
    display.fillRect(x, y, NOW_BOX_SIDE, NOW_BOX_SIDE, GxEPD_RED);
  }
  display.drawRect(x, y, NOW_BOX_SIDE, NOW_BOX_SIDE, GxEPD_BLACK);
  
  for (int i = 0; i <= DAYS_IN_A_WEEK; i++) {
    x = ( i * w_section ) + space_for_titles;
    display.drawLine(x, space_for_titles, x, space_for_titles + (h_section*HOURS_IN_A_DAY), GxEPD_BLACK);
    display.setCursor(x + ((w_section - w)/2) , 0);
    display.print(days[i]);
  }

  for (int i = 0; i <= HOURS_IN_A_DAY; i = i+3) {
    y = space_for_titles + (h_section*i);
    display.drawLine(space_for_titles, y, space_for_titles + (w_section*DAYS_IN_A_WEEK), y, GxEPD_BLACK);
    display.setCursor(0,y);
    display.print(i);
  }

}


void draw_time_interval(tm start_time, tm end_time) {
  int height, x, y;
  x = (start_time.tm_wday * w_section) + space_for_titles;
  y = (start_time.tm_hour * h_section) + space_for_titles;

  if (start_time.tm_yday == end_time.tm_yday) {
    height = (end_time.tm_hour - start_time.tm_hour)* h_section;
    display.fillRect( x, y, w_section, height, GxEPD_RED);
        
  } else {
    height = ((HOURS_IN_A_DAY - start_time.tm_hour)*h_section);
    display.fillRect( x, y, w_section, height, GxEPD_RED);

    y = space_for_titles;
    height = (HOURS_IN_A_DAY*h_section);
    for (int i = 1; i < end_time.tm_yday - start_time.tm_yday; i++) {
      x = ((start_time.tm_wday + i)*w_section) + space_for_titles;    
      display.fillRect(x, y, w_section, height, GxEPD_RED);
    }
    
    x = (end_time.tm_wday * w_section) + space_for_titles;
    height = (end_time.tm_hour)* h_section;
    display.fillRect( x, y, w_section, height, GxEPD_RED);
  }
}

