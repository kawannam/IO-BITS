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

// The int handler will just signal that the int has happen
// we will do the work from the main loop.
void awaken_callback(){
  Serial.println("awaken call_back");
  if (awake == false)  {
    wakeup();
  }
  time_t now = time(nullptr);
  detect_button_press(now);
}

void sleep() {
  Serial.println("Going to sleep");
  gpio_pin_wakeup_enable(GPIO_ID_PIN(MCP_CONNECTION_2), GPIO_PIN_INTR_HILEVEL);
  attachInterrupt(MCP_CONNECTION_2,awaken_callback,FALLING);
  Serial.println("Enabled Wakeup");
  wifi_station_disconnect();
  Serial.println("WifiDisconnected");
  bool stopped;
  awake = false;
  do
  {
    stopped = wifi_station_get_connect_status() == DHCP_STOPPED;
    if (!stopped)
    {
      Serial.println("dhcp not stopped?");
      delay(100);
    }
  } while (!stopped);
  Serial.println("---off...");
  wifi_set_opmode(NULL_MODE);
  wifi_set_sleep_type(MODEM_SLEEP_T);
  wifi_fpm_open();
  wifi_fpm_do_sleep(0xFFFFFFF);
  Serial.println("Sleep successful");
}

/*void detect_button_press(time_t now) {
  if (now - last_button_press < BUTTON_PRESS_WAIT) {
    char buff[30];
    struct tm* p_tm = localtime(&now);
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
    } else if (digitalRead(BUTTON_UNDO) == LOW) {
      Serial.println("UndoButtonPress");
      char message[35] = {my_name};
      client.publish("iobits/UndoButtonPress", message);
    }  else if (digitalRead(BUTTON_NEW_LOGFILE) == LOW) {
      Serial.println("NewFileRequest");
      char message[35] = {my_name};
      client.publish("iobits/NewFileRequest", message);
    } else if (digitalRead(BUTTON_SWITCH_VIS) == LOW) {
      int vis_option = (int)EEPROM.read(1);
      vis_option = (vis_option + 1) % NUMBER_OF_VIS_OPTIONS;
      //Write to memory
      EEPROM.write(0, (byte)vis_option);
      EEPROM.commit(); 
    }
  }
}*/

void detect_button_press(time_t now) {
  if (now - last_button_press > BUTTON_PRESS_WAIT) {
    Serial.println("Detecting Button Press");
    char buff[30];
    struct tm* p_tm = localtime(&now);
    strftime(buff, 30, "%Y-%m-%d %H:%M:%S", p_tm);
    // Get more information from the MCP from the INT
    Serial.println("HERE !!!!!! strftime ");
      uint8_t pin=mcp.getLastInterruptPin();
      Serial.println("HERE !!!!!! getLastInterruptPin ");
      uint8_t val=mcp.getLastInterruptPinValue();
      Serial.println("HERE !!!!!! getLastInterruptPinValue ");
      
      Serial.print("THERE !!!!!! Last pin ");
      Serial.println( pin);
      
      if(pin==BUTTON_A) {
        char message[35] = {my_name, ',', 'A', ','};
        strcat(message, buff);
        Serial.println("Button A has been pressed");
        client.publish("iobits/ButtonPress", message);
      } else if (pin==BUTTON_B) {
        char message[35] = {my_name, ',', 'B', ','};
        strcat(message, buff);
        Serial.println("Button B is Low");
        client.publish("iobits/ButtonPress", message);
      } else if (pin==BUTTON_UNDO) {
        Serial.println("UndoButtonPress");
        char message[35] = {my_name};
        client.publish("iobits/UndoButtonPress", message);
      } else if (pin == BUTTON_NEW_LOGFILE) {
        Serial.println("NewFileRequest");
        char message[35] = {my_name};
        client.publish("iobits/NewFileRequest", message);
      } else if (pin==BUTTON_SWITCH_VIS) {
        Serial.println("Switch Vis");
        int vis_option = (int)EEPROM.read(1);
        vis_option = (vis_option + 1) % NUMBER_OF_VIS_OPTIONS;
        //Write to memory
        EEPROM.write(0, (byte)vis_option);
        EEPROM.commit(); 
      } else {
        Serial.println("Unknown input ");
      }
  }
}
  

void conditional_publish(char * channel, char * message, time_t time) {
  if (time - last_pub > PUBLISH_WAIT) {
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
    int vis_option = (int)EEPROM.read(0);
    vis_option = 2;
    switch (vis_option) {
      case 0: 
        dotted_week();
        break;
      case 1:
        tug_of_war();
        break;
      case 2:
        plain_text();
        break;
      case 3:
        streaks_and_valleys();
        break;
      default:
        stacked_bars();
        break;
    }
    sleep();
  }
}
