//File needs cleaning

#define MAX_STREAK 14

void get_days(bool days[3][MAX_NUMBER_OF_DAYS]) {
  struct tm point_tm;
  int checking_day, checking_year, index;
  for (int i = 0; i < MAX_NUMBER_OF_DAYS; i++) {
    days[0][i] = false;
    days[1][i] = false;
    days[2][i] = false;
  }

  checking_day = current_time_tm.tm_yday;
  checking_year = current_time_tm.tm_year;
  
  for (int i = 0; i < current_number_of_points; i++) {
    if ( points[i].timestamp != NULL ) {
      point_tm = *localtime(&points[i].timestamp);
      index = (checking_day - point_tm.tm_yday) + ((checking_year - point_tm.tm_year)*DAYS_IN_A_YEAR);
      if (index < MAX_NUMBER_OF_DAYS) {
        days[0][index] = true;
        if (points[i].button == 'A') days[1][index] = true;
        else if (points[i].button == 'B') days[2][index] = true;
      }
    }
  }
}

void get_streak(streaks *my_streaks, bool days[3][MAX_NUMBER_OF_DAYS] ) {

  get_days(days);

  int  streak, A_streak, B_streak;
  bool is_A_streaking = true;
  bool is_B_streaking = true;
  bool is_streaking = true;
  streak = 1;
  A_streak = 1;
  B_streak = 1;
  while( (streak < MAX_NUMBER_OF_DAYS) && (is_streaking)) {
    if (days[0][streak] == days[0][1]) streak++;
    else break;
    
    if ((is_A_streaking) && (days[1][streak] == days[1][1])) A_streak++;
    else is_A_streaking = false;

    if ((is_B_streaking) && (days[2][streak] == days[2][1])) B_streak++;
    else is_B_streaking = false;

    Serial.println("Loop " + String(streak));
  }

  int temp = process_streak(streak, days[0][0], days[0][1]);
  my_streaks->A = process_streak(A_streak, days[1][0], days[1][1]);
  my_streaks->B = process_streak(B_streak, days[2][0], days[2][1]);
  my_streaks->Total = temp;
  
  Serial.println("Streak A " + String(A_streak));
  Serial.println("Streak B " + String(B_streak));
  Serial.println("Streak Total " + String(streak));

  return;
}

int process_streak(int streak, bool day_zero, bool day_one) {

  if (!day_one) streak = streak * -1;
  if ((day_zero) && (streak < 0)) streak = 0;
  if (day_zero) streak++;
  
  return streak;
}

