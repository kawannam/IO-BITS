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
#define publish_wait 10
time_t last_published_data_request = publish_wait * (-1);

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

bool connect_to_mqtt() {
  client.setServer(mqttServer, mqttPort);
  WiFi.mode(WIFI_STA);
  int connection_tries = 0;
  while ((!client.connected()) && (connection_tries < MAX_NUMBER_OF_CONNECTION_TRIES)) {
    digitalWrite(STATUS_LIGHT, HIGH);
    Serial.println("Connecting to MQTT...");
 
    if (client.connect(device_id)) {
      digitalWrite(STATUS_LIGHT, LOW);
      Serial.println("connected");
    }
    else {
      Serial.println("failed with state ");
      Serial.print(client.state());
      digitalWrite(STATUS_LIGHT, LOW);
      delay(2000);
    }
    connection_tries++;
  }

  if ( client.connected() ) {
    client.subscribe(data_response);
    client.subscribe(data_response_points);
    client.setCallback(message_callback);
    Serial.println("Connected to the MQTT server");
    return true;
  } else {
    display_error("Cannot \nconnect \nto mqtt \nserver", f18b);
    return false;
  }
}


void message_callback(char* topic, byte* payload, unsigned int length) {
  char device = char(payload[0]);
  if (device == my_name) {
    String type = String(topic);
    
    if (type ==  "iobits/DataResponse") {
      Serial.println("DataResponse recieved, length: " + String(length));
      number_of_expected_messages = data_message(payload, length);

    } else if (type == "iobits/DataResponsePoints") {
      number_of_expected_messages = number_of_expected_messages - 1;
      data_response_message_points(payload, length, number_of_expected_messages);

    } else {
      Serial.println("Error: Unknown message type recieved");
    }
  }
}


bool request_data() {
  time_t now;
  int connection_tries = 0;
  do {
    digitalWrite(STATUS_LIGHT, HIGH);
    now = time(nullptr);
    if ((now - last_published_data_request) > publish_wait) {      
      Serial.println("REQUESTING DATA...");
      number_of_expected_messages = 99999;
      client.publish(data_request, &my_name);
      digitalWrite(STATUS_LIGHT, LOW);
      delay(110);
      last_published_data_request = now;
      connection_tries++;
    }
    client.loop();
  } while ((number_of_expected_messages > 0) && (connection_tries < MAX_NUMBER_OF_CONNECTION_TRIES));
  if (number_of_expected_messages <= 0) {
    Serial.println("DATA RECEIVED");
    return true;
  } else {
    Serial.println("DATA WAS NOT RECEIVED");
    display_error("Did not \nreceive \ndata", f18b);
    return false;
  }
}


void notify_button_press(char type) {
  char time_string[30];
  char message[35] = {my_name, ',', type, ','};
  Serial.println(" notify_button_press " + String(message));
  client.publish("iobits/ButtonPress", message);
}

