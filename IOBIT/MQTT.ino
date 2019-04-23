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
#define publish_wait 10000
time_t last_published_data_request = 0;

#define MAX_INT 2147483647
//-------------------------------------------------//

//--------------MQTT Consts------------------------//
const char* device_id = "IOBITClient" + my_name;
const char* mqttServer = "192.168.4.1";
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
      Serial.println("DataResponse recieved");
      number_of_expected_messages = data_message(payload, length);

    } else if (type == "iobits/DataResponsePoints") {
      number_of_expected_messages = number_of_expected_messages - 1;
      data_response_message_points(payload, length, number_of_expected_messages);

    } else {
      Serial.println("Error: Unknown message type recieved");
    }
  }
}


void request_data() {
  time_t now;  
  do {
    now = time(nullptr);
    if ((now - last_published_data_request) > publish_wait) {
      Serial.println(now);
      Serial.println("REQUESTING DATA...Number of expected Messages " + String(number_of_expected_messages));
      number_of_expected_messages = MAX_INT;
      client.publish(data_request, &my_name);
      delay(110);
      last_published_data_request = now;
    }
    client.loop();
  } while (number_of_expected_messages > 0);
  Serial.println("DATA RECEIVED");
}


void notify_button_press(char type) {
  char time_string[30];
  char message[35] = {my_name, ',', type, ','};
  Serial.println(" notify_button_press " + String(message));
  client.publish("iobits/ButtonPress", message);
}









