long randNumber = 0;
  int colour;

void streaks_and_valleys() {
  Serial.println("Streaks and Valleys");
  display.setRotation(1);
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
  if (streak > 4) {
    streak = 4;
  }
  Serial.println("here");
  for ( int i = 0; i < DISPLAY_HEIGHT; i++) {
    for (int j = 0; j < DISPLAY_WIDTH; j++) {
      randNumber =  random(101);
      if ( randNumber < (streak * 25)) {
        display.drawPixel(i, j, colour);
      }
    }
  }

  display.fillRect(120, 120, 154, 154, GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(135, 135);
  display.println(dispaly_streak);
  display.update();
  ESP.deepSleep(6e7); // 20e6 is 20 microseconds
}

int get_streak() {
  return -8;
}

