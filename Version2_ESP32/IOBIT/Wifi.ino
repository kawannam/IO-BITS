
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
