

void connect_to_time_server() {
  configTime(timezone, dst, "pool.ntp.org");  
  setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 0);  
  while(time(nullptr) <= 100000) {
        Serial.print("Connecting to time server...");
        delay(1000);
      }
  Serial.println("Connected");
}


void time_to_string(time_t timestamp, char time_string[]) {
  struct tm* p_tm = localtime(&timestamp);
  strftime(time_string, 30, "%Y-%m-%d %H:%M:%S", p_tm);
  Serial.println(time_string);
}

tm string_to_time(char time_string[], time_t timestamp) {
    struct tm p_tm;
    strptime(time_string, "%Y-%m-%d %H:%M:%S", &p_tm);
    timestamp = mktime(&p_tm);  // t is now your desired time_t
    return p_tm;
}

