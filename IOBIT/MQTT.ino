#define MAX_INT 2147483647

//--------------Networking-------------------------//
#define PUBLISH_WAIT 10
//-------------------------------------------------//

const char* device_id = "IOBITClient" + my_name;
const char* mqttServer = "test.mosquitto.org";
const int mqttPort = 1883;

//Message Types
const char* data_request = "iobits/DataRequest";
const char* button_press = "iobits/ButtonPress";

int number_of_expected_messages = 0;

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

void notify_button_press(char type) {
  char time_string[30];
  time_t now = time(nullptr);
  time_to_string(now, time_string);
  
  char message[35] = {my_name, ',', type, ','};
  strcat(message, time_string);
  client.publish("iobits/ButtonPress", message);
}

void request_data() {
  
  Serial.println("request_data");
  time_t now = time(nullptr);
  time_t last_pub = time(nullptr) - PUBLISH_WAIT - 1;
  
  do {
    now = time(nullptr);
    if (now > (last_pub + PUBLISH_WAIT)) {
      number_of_expected_messages = MAX_INT;
      client.publish(data_request, &my_name);
      last_pub = now;
    }
    client.loop();
  } while (number_of_expected_messages > 0);
}

void message_callback(char* topic, byte* payload, unsigned int length) {

  //Is the server talking to me?
  char device = char(payload[0]);
  if (device == my_name) {
    //What type of message is it
    String type = String(topic);
    if (type ==  "iobits/DataResponse") {
      Serial.println("Data Response Recieved ");
      
      number_of_expected_messages = dm_get_number_of_expected_messages(payload);
      data_message(payload, length);
      Serial.println(number_of_expected_messages);
      
    } else if (type == "iobits/DataResponsePoints") {
      Serial.println("DataResponsePoints ");
      number_of_expected_messages = number_of_expected_messages - 1;
      data_response_points(payload, length, number_of_expected_messages);
      
    } else {
      Serial.println("Error: Unknown message type recieved");
    }
  }
}





