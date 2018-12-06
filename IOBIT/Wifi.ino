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
