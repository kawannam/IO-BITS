void callback(char* topic, byte* payload, unsigned int length) {
  //Parse data
  //Is the server talking to me?
  char device = char(payload[0]);
  if (  device != my_name) {
      //Message is not for me
      return;
  }
  //What type of message is it
  String type = String(topic);
  if ( type ==  "iobits/DataResponse") {
    Serial.println("Data Response Recieved");
    data_message(payload, length);
  } else if (type == "iobits/DataResponsePoints") {
    data_response_points(payload, length);
  } else {
    Serial.println("Error: Unknown message type recieved");
  }
}

void conditional_publish(char * channel, char * message, time_t time) {
  if (time - last_pub > PUBLISH_WAIT) {
    struct tm* p_tm = localtime(&time);
    char buff[30];
    strftime(buff, 30, "%Y-%m-%d %H:%M:%S", p_tm);
    if(digitalRead(BUTTON_A) == LOW) {   
      char message[35] = {my_name, ',', 'A', ','};
      strcat(message, buff);
      Serial.println("Button A is Low");
      client.publish("iobits/ButtonPress", message);
    } else if(digitalRead(BUTTON_B) == LOW) {
      char message[35] = {my_name, ',', 'B', ','};
      strcat(message, buff);
      Serial.println("Button B is Low");
      client.publish("iobits/ButtonPress", message);
    } else if(digitalRead(BUTTON_B) == HIGH) {
      //Serial.println("Button B is High");
    }  else if(digitalRead(BUTTON_A) == HIGH) {
      //Serial.println("Button A5 is High");
    }
    Serial.println("Sending request...");
    client.publish(channel, message);
    last_pub = time;
  }
}

void data_message(byte* payload, unsigned int length) {
  char count_A_char[DATA_COUNT_LENGTH + 1];
  char count_B_char[DATA_COUNT_LENGTH + 1];
  char data_count[DATA_NUMBER_OF_POINTS_LENGTH + 1];
  memcpy(count_A_char, payload + DATA_COUNT_A_OFFSET, DATA_COUNT_LENGTH);
  memcpy(count_B_char, payload + DATA_COUNT_B_OFFSET, DATA_COUNT_LENGTH);
  memcpy(data_count, payload + DATA_NUMBER_OF_POINTS_OFFSET, DATA_NUMBER_OF_POINTS_LENGTH);
 
  count_A_char[DATA_COUNT_LENGTH] = '\0';
  count_B_char[DATA_COUNT_LENGTH] = '\0';
  data_count[DATA_NUMBER_OF_POINTS_LENGTH] = '\0';
  
  count_A = atoi(count_A_char);
  count_B = atoi(count_B_char);
  number_of_expected_messages = atoi(data_count);
}

void data_response_points(byte* payload, unsigned int length) {
  number_of_expected_messages = number_of_expected_messages - 1;
  char *ptr;
  char char_time[length - 4 + 1];
  memcpy(char_time, payload + 4, 22);
  char_time[length - 4] = '\n';
  points[number_of_expected_messages] = { payload[2], String(char_time)};
  if ( number_of_expected_messages <= 0 ) {
    Serial.println("All messages recieved - deciding on vis");
    //dotted_week();
    //tug_of_war();
    //plain_text();
    //streaks_and_valleys();
    stacked_bars();

  }
}
