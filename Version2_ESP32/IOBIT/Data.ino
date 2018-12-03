//--------------Message Parsing-------------------//
#define DATA_COUNT_A_OFFSET 2
#define DATA_COUNT_B_OFFSET 7
#define DATA_COUNT_LENGTH 4
#define DATA_NUMBER_OF_POINTS_OFFSET 12
#define DATA_NUMBER_OF_POINTS_LENGTH 3
#define MAX_NUMBER_OF_DATA_POINTS 10
//-------------------------------------------------//

//--------------Data------------------------------//
data_point points[MAX_NUMBER_OF_DATA_POINTS];
int count_A = 0;
int count_B = 0;
//-------------------------------------------------//
int message_parser(byte* payload, int offset, int mem_size) {
  char value_char[mem_size + 1];
  memcpy(value_char, payload + offset, mem_size);
  value_char[mem_size] = '\0';
  return atoi(value_char);
}

int dm_get_number_of_expected_messages (byte* payload) {
  return message_parser(payload, DATA_NUMBER_OF_POINTS_OFFSET, DATA_NUMBER_OF_POINTS_LENGTH);
}

void data_message(byte* payload, unsigned int length) {
  char pay[length];
  memcpy(pay, payload, length);
  Serial.println(pay);
  count_A = message_parser(payload, DATA_COUNT_A_OFFSET, DATA_COUNT_LENGTH);
  count_B = message_parser(payload, DATA_COUNT_A_OFFSET, DATA_COUNT_LENGTH); 
}

void data_response_points(byte* payload, unsigned int length, int number_of_expected_messages) {
  char *ptr;
  time_t the_time;
  struct tm p_tm;
  char char_time[length - 4 + 1];
  memcpy(char_time, payload + 4, 22);
  char_time[length - 4] = '\n';

  p_tm = string_to_time(char_time, the_time);
  points[number_of_expected_messages] = { payload[2], &p_tm};
  Serial.println("ADDING DATA " + number_of_expected_messages);
}

