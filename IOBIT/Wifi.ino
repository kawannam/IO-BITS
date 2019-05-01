/*
 * Wifi: Initializes connection to wifi
 * 
 * void connect_to_wifi() - tries to connect to 
 * wifi indefinately. 
 * 
 * Note to Kendra - maybe it should give up and give an error
 * Note to Kendra - write a wifi deconstructor
 * 
 */
#define MAX_NUMBER_OF_CONNECTION_TRIES 10
 
bool connect_to_wifi() {
  WiFi.begin(ssid, password);
  int connection_tries = 0;
  while (WiFi.status() != WL_CONNECTED && (connection_tries < MAX_NUMBER_OF_CONNECTION_TRIES)) {
    digitalWrite(STATUS_LIGHT, HIGH);
    delay(500);
    Serial.println("Connecting to WiFi..");
    digitalWrite(STATUS_LIGHT, LOW);
    delay(500);
    connection_tries++;
  }
  if ( WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to the WiFi network");
    return true;
  } else {
    display_error("Cannot \nconnect \nto wifi", f18b);
    return false;
  }
}
