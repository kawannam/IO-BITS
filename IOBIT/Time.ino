/*
 * Time: Connects to a server to get the current time and handles converting
 * time into the different forms it is needed in.
 * 
 * void connect_to_time_server() - Initializes a connection to the time 
 * server. Uses the customization timezone info.
 * 
 * void time_to_string(time_t timestamp, char time_string[]) - converts a
 * time_t object to a string and places it in the given char array (time_string)
 * 
 * tm string_to_time(char time_string[], time_t timestamp) - converts a string
 * time to a time_t and places it in the given time_t object. Additionally, it
 * returns a tm -incase a user needs that instead.
 * 
 * Note to Kendra: explore esp32 capabilities to get a timestamp when connections
 * to the time server fail
 * 
 * Note to Kendra: consider adding https://github.com/JChristensen/Timezone
 * library to handle timezones/daylight savings. Not sure this handles it
 */

void connect_to_time_server() {
  configTime(timezone, dst, "pool.ntp.org");  
  setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 0);  
  while(time(nullptr) <= 100000) {
        Serial.print("Connecting to time server...");
        delay(1000);
      }
  Serial.println("Connected");
}

void time_to_string_display(time_t timestamp, char time_string[]) {
  struct tm* p_tm = localtime(&timestamp);
  strftime(time_string, 30, "%H:%M %b %e '%y", p_tm);
}

void time_to_string(time_t timestamp, char time_string[]) {
  struct tm* p_tm = localtime(&timestamp);
  strftime(time_string, 30, "%Y-%m-%d %H:%M:%S", p_tm);
}

void string_to_time(char time_string[], time_t* timestamp) {
    struct tm p_tm;
    strptime(time_string, "%Y-%m-%d %H:%M:%S", &p_tm);
    *timestamp = mktime(&p_tm);  // t is now your desired time_t
}

