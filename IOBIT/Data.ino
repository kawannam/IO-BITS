/*
 * Data: Parses and stores data while the esp32 is awake
 * 
 * Data - all the data the visualizations have access
 * to
 * 
 * void data_message(byte* payload, unsigned int length) -
 * After a data request is sent to the server, the server
 * first responds with "data response" message which contains
 * the number of total presses for each button (A and B). 
 * This function parses out those values from the message.
 * 
 * void data_response_message_points(byte* payload, unsigned int length, int number_of_expected_messages) -
 * After a data request is sent to the server and the data response message has been parsed, this 
 * function parses and stores the data points in points, from the "data response points" message.
 *  
 * 
 * 
 */

//--------------Data------------------------------//
#define MAX_NUMBER_OF_DATA_POINTS 100
data_point points[MAX_NUMBER_OF_DATA_POINTS];
int count_A = 0;
int count_B = 0;
int current_number_of_points = 0;
//-------------------------------------------------//


int data_message(byte* payload, unsigned int length) {
  char pay[length];
  memcpy(pay, payload, length);

  char *str;
  str = strtok(pay, ",");

  str = strtok(NULL, ",");
  count_A = atoi(str);
  str = strtok(NULL, ",");
  count_B = atoi(str);
  str = strtok(NULL, ",");
  current_number_of_points = atoi(str);
  return current_number_of_points;
}

void data_response_message_points(byte* payload, unsigned int length, int number_of_expected_messages) {
  if (length == 0) return;
  char *str;
  char pay[length];
  time_t the_time;
  
  memcpy(pay, payload, length);

  str = strtok(pay, ",");
  str = strtok(NULL, ",");
  char button = *str;
  str = strtok(NULL, ",");
  string_to_time(str, &the_time);
  points[number_of_expected_messages] = (data_point) { button, the_time};
}

