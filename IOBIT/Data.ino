/*
 * Data: Parses and stores data while the esp32 is awake
 * 
 * Message Parsing - defines the offsets and lengths of
 * the specific values sent from the server
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
 * int data_message_get_number_of_expected_messages (byte* payload) -
 * After a data request is sent to the server, the server first 
 * responds with "data response" message which contains the number
 * of data points the server is going to send back to this device
 * 
 * void data_response_message_points(byte* payload, unsigned int length, int number_of_expected_messages) -
 * After a data request is sent to the server and the data response message has been parsed, this 
 * function parses and stores the data points in points, from the "data response points" message.
 *  
 * int message_parser(byte* payload, int offset, int mem_size) -
 * A helper function that returns part of the payload, starting
 * at the offset for the mem_size
 * 
 * 
 */
 
//--------------Message Parsing-------------------//
#define DATA_COUNT_A_OFFSET 2
#define DATA_COUNT_B_OFFSET 7
#define DATA_COUNT_LENGTH 2
#define DATA_NUMBER_OF_POINTS_OFFSET 12
#define DATA_NUMBER_OF_POINTS_LENGTH 3
#define MAX_NUMBER_OF_DATA_POINTS 10
#define DATA_TIMESTAMP_OFFSET 4
#define DATA_TIMESTAMP_SIZE 22
//-------------------------------------------------//

//--------------Data------------------------------//
data_point points[MAX_NUMBER_OF_DATA_POINTS];
int count_A = 0;
int count_B = 0;
//-------------------------------------------------//


int data_message(byte* payload, unsigned int length) {
  char pay[length];
  memcpy(pay, payload, length);

  Serial.println(pay);

  char *str;
  str = strtok(pay, ",");

  Serial.println(str);
  str = strtok(NULL, ",");
  count_A = atoi(str);
  str = strtok(NULL, ",");
  count_B = atoi(str);
  str = strtok(NULL, ",");

  return atoi(str);
}

void data_response_message_points(byte* payload, unsigned int length, int number_of_expected_messages) {
  char *str;
  char pay[length];
  time_t the_time;
  
  memcpy(pay, payload, length);

  str = strtok(pay, ",");
  str = strtok(NULL, ",");
  char button = *str;
  str = strtok(NULL, ",");
  string_to_time(str, &the_time);
  //Serial.println("TEST");
  //Serial.println(str);
  Serial.println(number_of_expected_messages);
  points[number_of_expected_messages] = (data_point) { button, the_time};
}


int message_parser(byte* payload, int offset, int mem_size) {
  //testing
  char pay[30];
  memcpy(pay, payload + offset, 30);
  Serial.println(pay);

  
  char value_char[mem_size + 1];
  memcpy(value_char, payload + offset, mem_size);
  value_char[mem_size] = '\0';
  Serial.println(value_char);
  return atoi(value_char);
}

