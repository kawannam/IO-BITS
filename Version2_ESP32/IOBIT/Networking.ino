void connect_to_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  if ( WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to the WiFi network");
  }
}

void connect_to_time_server() {
  configTime(timezone, dst, "pool.ntp.org");  
  setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 0);  
  while(time(nullptr) <= 100000) {
        Serial.print("Connecting to time server...");
        delay(1000);
      }
  Serial.println("Connected");
}

void connect_to_mqtt() {
  client.setServer(mqttServer, mqttPort);
  WiFi.mode(WIFI_STA);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect(device_id)) {
      Serial.println("connected");
    }
    else {
      Serial.println("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }

  client.subscribe("iobits/DataResponse");
  client.subscribe("iobits/DataResponsePoints");
  client.setCallback(message_callback);
}

void message_callback(char* topic, byte* payload, unsigned int length) {
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
  time_t the_time;
  struct tm p_tm;
  char char_time[length - 4 + 1];
  memcpy(char_time, payload + 4, 22);
  char_time[length - 4] = '\n';
    //(points[i].time_stamp).toCharArray(char_time, 30);
    strptime(char_time, "%Y-%m-%d %H:%M:%S", &p_tm);
    //the_time = mktime(&p_tm);  // t is now your desired time_t

    //p_tm = &(localtime(&the_time));

  
  points[number_of_expected_messages] = { payload[2], &p_tm};
  if ( number_of_expected_messages <= 0 ) {
    Serial.println("All messages recieved - deciding on vis");
    int vis_option = 0;
    vis_option = 2;
    switch (vis_option) {
      case 0: 
        //dotted_week();
        break;
      case 1:
        //tug_of_war();
        break;
      case 2:
        //plain_text();
        break;
      case 3:
        //streaks_and_valleys();
        break;
      default:
        //stacked_bars();
        break;
    }
    //sleep();
  }
}

void conditional_publish(char * channel, char * message, time_t time) {
  if (time - last_pub > PUBLISH_WAIT) {
    Serial.println("Sending request...");
    client.publish(channel, message);
    last_pub = time;
  }
}

