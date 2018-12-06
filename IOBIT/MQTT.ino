/*
 * MQTT: Handles the connection and comunication between
 * the device and the server using the MQTT protocol
 * 
 * If you wanted to add message types do it in this file:
 * Add to message types, and either add a function to send
 * or add functionallity to message_callback
 * 
 * void connect_to_mqtt() - Initializes a connection to server
 * via mqtt. If it fails it retries indefinetly. Additionally, 
 * it subscribes to all the messages we want to sets the 
 * callback fuction.
 * Note to kendra - it should give up and notify user eventually
 * 
 * void message_callback(char* topic, byte* payload, unsigned int length) -
 * This is triggered when one of the messages we were subscribed to is
 * received (note the device must not be in deepsleep mode to receive messages)
 * The function confirms that the message is for this device, before updating 
 * number_of_expected_messages and triggering the corresponding action
 * 
 * void request_data() - Due to the hardware's limitated memory all the data
 * is actually stored on a server, request data asks the server to send the
 * last "MAX_NUMBER_OF_DATA_POINTS" to us to visualize. It runs client.loop() 
 * on loop until the all messages are received or until a timeout and a new
 * request is sent
 * Note to Kendra - Handle the case where it shoudl giveup getting the data
 * 
 * void notify_button_press(char type) - A helper function that formats and
 * sends a button press notification message. ("iobits/ButtonPress")
 * 
 * Note to Kendra - change the server you are using
 */

//--------------General defines--------------------//
#define PUBLISH_WAIT 10
#define MAX_INT 2147483647
//-------------------------------------------------//

//--------------MQTT Consts------------------------//
const char* device_id = "IOBITClient" + my_name;
const char* mqttServer = "test.mosquitto.org";
const int mqttPort = 1883;
//-------------------------------------------------//

//--------------Message Types----------------------//
//Outgoing 
const char* data_request = "iobits/DataRequest";
const char* button_press = "iobits/ButtonPress";

//Incoming 
const char* data_response = "iobits/DataResponse";
const char* data_response_points = "iobits/DataResponsePoints";
//-------------------------------------------------//

//--------------MQTT Variables---------------------//
WiFiClient espClient;
PubSubClient client(espClient);
int number_of_expected_messages = 0;
//-------------------------------------------------//

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

  client.subscribe(data_response);
  client.subscribe(data_response_points);
  client.setCallback(message_callback);
}


void message_callback(char* topic, byte* payload, unsigned int length) {
  char device = char(payload[0]);
  
  if (device == my_name) {
    String type = String(topic);
    
    if (type ==  "iobits/DataResponse") {
      Serial.println("Data Response Recieved ");
      number_of_expected_messages = data_message(payload, length);

    } else if (type == "iobits/DataResponsePoints") {
      Serial.println("DataResponsePoints ");
      number_of_expected_messages = number_of_expected_messages - 1;
      data_response_message_points(payload, length, number_of_expected_messages);

    } else {
      Serial.println("Error: Unknown message type recieved");
    }
  }
}


void request_data() {
  time_t now;
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


void notify_button_press(char type) {
  char time_string[30];
  time_t now = time(nullptr);
  time_to_string(now, time_string);
  
  char message[35] = {my_name, ',', type, ','};
  strcat(message, time_string);
  client.publish("iobits/ButtonPress", message);
}









