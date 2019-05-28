//File needs cleaning

#define MAX_STREAK 14
#define A_INDEX 0
#define B_INDEX 1
#define BOTH_INDEX 2

void get_days(int days[3][MAX_NUMBER_OF_DAYS]) {
  struct tm point_tm;
  int checking_day, checking_year, index;
  for (int i = 0; i < MAX_NUMBER_OF_DAYS; i++) {
    days[A_INDEX][i] = 0;
    days[B_INDEX][i] = 0;
    days[BOTH_INDEX][i] = 0;
  }

  checking_day = current_time_tm.tm_yday;
  checking_year = current_time_tm.tm_year;
  
  for (int i = 0; i < current_number_of_points; i++) {
    if ( points[i].button != NULL ) {
      point_tm = points[i].timestamp;
      index = (checking_day - point_tm.tm_yday) + ((checking_year - point_tm.tm_year)*DAYS_IN_A_YEAR);
      if (index < MAX_NUMBER_OF_DAYS) {
        days[BOTH_INDEX][index]++;
        if (points[i].button == 'A') days[A_INDEX][index]++;
        else if (points[i].button == 'B') days[B_INDEX][index]++;
      }
    }
  }
}


void get_streak(streaks *my_streaks, int days[3][MAX_NUMBER_OF_DAYS] ) {

  get_days(days);

  int  streak, A_streak, B_streak;
  bool is_A_streaking = true;
  bool is_B_streaking = true;
  bool is_streaking = true;
  streak = 1;
  A_streak = 1;
  B_streak = 1;
  int index = 2;
  while( (index < MAX_NUMBER_OF_DAYS) && (is_streaking)) {
    if ((is_streaking) && 
      (((days[BOTH_INDEX][index] == 0) && (days[BOTH_INDEX][1] == 0)) || 
        ((days[BOTH_INDEX][1]) > 0 && (days[BOTH_INDEX][index] > 0)))) streak++;
    else break;
    
   if ((is_A_streaking) && 
      (((days[A_INDEX][index] == 0) && (days[A_INDEX][1] == 0)) || 
        ((days[A_INDEX][1]) > 0 && (days[A_INDEX][index] > 0)))) A_streak++;
    else is_A_streaking = false;

    if ((is_B_streaking) && 
      (((days[B_INDEX][index] == 0) && (days[B_INDEX][1] == 0)) || 
        ((days[B_INDEX][1]) > 0 && (days[B_INDEX][index] > 0)))) B_streak++;
    else is_B_streaking = false;
    index++;
  }
   
  my_streaks->A = process_streak(A_streak, days[A_INDEX][0], days[A_INDEX][1]);
  my_streaks->B = process_streak(B_streak, days[B_INDEX][0], days[B_INDEX][1]);
  my_streaks->Total = process_streak(streak, days[BOTH_INDEX][0], days[BOTH_INDEX][1]);

  return;
}

int get_day_delta(struct tm day_one, struct tm day_two) {
  int year_diff = day_two.tm_year - day_one.tm_year;
  int day_diff = day_two.tm_yday - day_two.tm_yday;
  return day_diff + (365*year_diff);
}

int process_streak(int streak, bool day_zero, bool day_one) {

  if (!day_one) streak = streak * -1;
  if ((day_zero) && (streak < 1)) streak = 0;
  if (day_zero) streak++;
  
  return streak;
}

