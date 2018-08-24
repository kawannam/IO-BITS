#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include <GxEPD.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include <time.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
#include <GxIO/GxIO.cpp>
#include <GxGDEW0154Z17/GxGDEW0154Z17.cpp>

#define BUSY 12 //D6
#define RST 2 //D4
#define DC 0 //D3
#define CS 15 //D8
#define CLK 14 //D5
#define DIN 13 //D7

#define BUTTON_A 5 //D1
#define BUTTON_B 4 //D2

#define CHIP_SELECT 13 // default: 5

#define ADALINE 7
#define BEETHOVEN 9

#define DEVICE_OFFSET 0
#define DEVICE_LENGTH 1

#define PUBLISH_WAIT 10

//For a data message
#define DATA_COUNT_A_OFFSET 2
#define DATA_COUNT_B_OFFSET 7
#define DATA_COUNT_LENGTH 4
#define DATA_NUMBER_OF_POINTS_OFFSET 12
#define DATA_NUMBER_OF_POINTS_LENGTH 3

#define DISPLAY_HEIGHT 154
#define DISPLAY_WIDTH 154

const char* ssid = "****";
const char* password = "****";

const char* mqttServer = "test.mosquitto.org";
const int mqttPort = 1883;

const int timezone = 0 * 3600;
const int dst = 0;

struct data_point {
  char button;
  unsigned long time_stamp;
};
struct coord {
  int x;
  int y;
};

data_point points[100];

GxIO_Class io(SPI, CHIP_SELECT, DC, RST);
GxEPD_Class display(io, RST, BUSY);

WiFiClient espClient;
PubSubClient client(espClient);
int number_of_expected_messages = 0;


void callback(char* topic, byte* payload, unsigned int length) {
  //Parse data
  Serial.println("Message Received");
  //Is the server talking to me?
  char device = char(payload[0]);
  if (  device != 'A') {
      //Message is not for me
      return;
  }
  //What type of message is it
  String type = String(topic);
  Serial.println("Message is for me");
  if ( type ==  "iobits/DataResponse") {
    Serial.println("Data Response Recieved");
    data_message(payload, length);
  } else if (type == "iobits/DataResponsePoints") {
    data_response_points(payload, length);
  } else {
    Serial.println("Error: Unknown message type recieved");
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
  
  int count_A = atoi(count_A_char);
  int count_B = atoi(count_B_char);
  number_of_expected_messages = atoi(data_count);
  //tug_of_war(count_A, count_B);
  text_vis(count_A, count_B);
}

void data_response_points(byte* payload, unsigned int length) {
  number_of_expected_messages = number_of_expected_messages - 1;
  char *ptr;
  char char_time[11];
  memcpy(char_time, payload + 4, 10);
  //String string_time = String(char_time);
  char_time[10] = '\0';
  points[number_of_expected_messages] = { payload[3], strtol(char_time, &ptr, 10)};

  if ( number_of_expected_messages <= 0 ) {
    //dotted_week();

  }
}

coord dotted_week_get_spot(int p_hour, int p_wday) {
  int space_for_titles_hour = 8;
  int space_for_titles_week = 2;
  
  int d_height = DISPLAY_HEIGHT - (5*2);
  int d_width = DISPLAY_WIDTH - space_for_titles_hour;
  
  coord spot;
  spot.y = ((p_hour*d_height)/24) + (d_height/48) - space_for_titles_week;
  spot.x = ((p_wday*d_width)/7) + (d_width/14) + space_for_titles_hour;
  return spot;
}

void dotted_week() {
  Serial.println("DottedDisplay");
  display.fillScreen(GxEPD_WHITE);

  coord spot;
  time_t now = time(nullptr);
  struct tm* now_tm = localtime(&now);
  time_t the_time;
  struct tm* p_tm;
  Serial.println("Now: " + now);

  spot = dotted_week_get_spot(now_tm->tm_hour, now_tm->tm_wday);
  display.drawRect(spot.y - 2, spot.x - 2, 6, 6, GxEPD_BLACK);

  for(int i = 0; i < 10; i++) {
      the_time = points[i].time_stamp;
      p_tm = localtime(&the_time);
      //Need to handle year roll over
    if (now_tm->tm_wday >= p_tm->tm_wday &&  (now_tm->tm_yday - p_tm->tm_yday < 6)) { //is less than seven days from point
      spot = dotted_week_get_spot(p_tm->tm_hour, now_tm->tm_wday);
      if (points[i].button == 'A') {
        display.fillCircle(spot.y, spot.x, 3, GxEPD_BLACK); //button is A -> colour black
      } else {
        display.fillCircle(spot.y, spot.x, 3, GxEPD_RED); //button is B -> colour yellow
      }
    }

  }

  for (int i = 0; i < 24; i++) {
    for(int j = 0; j < 7; j++) {
      if (! (i == now_tm->tm_hour && j == now_tm->tm_wday)) {
        spot = dotted_week_get_spot(i, j);
        display.drawCircle(spot.y, spot.x, 3, GxEPD_BLACK);
      } else {
        Serial.println("HEre");
      }
    }
  }

  display.setCursor(0, 0);
  display.setTextColor(GxEPD_RED);
  display.setRotation(1);
  for( int i = 0; i < 24; i = i +3) {
    spot = dotted_week_get_spot( i+1, 0);
    display.setCursor(0, spot.y + 2);
    display.println(i);
  }
  char days[7] = {'U', 'M', 'T', 'W', 'R', 'F', 'S'};
  for( int i = 0; i < 7; i++) {
    spot = dotted_week_get_spot(0, i);
    display.setCursor( spot.x - 2, 0);
    display.println(days[i]);
  }
  
   display.update();
   //Put the chip to sleep. 
   ESP.deepSleep(6e7); // 20e6 is 20 microseconds
}

void tug_of_war(int count_A, int count_B) {

    //Add font/ writting
      const GFXfont* f1 = &FreeSansBold9pt7b;  
      display.setFont(f1);
      display.setRotation(1);

    if (count_A == 0 && count_B == 0) {
      display.fillScreen(GxEPD_WHITE);
      display.setCursor(70, 70);
      display.setTextColor(GxEPD_RED);
      display.println("No Data");
      
      display.update();
    } else {
      float ratio_A = ((count_A * DISPLAY_WIDTH) / (count_A + count_B));
      int percent_A = ((count_A * 100) / (count_A + count_B));
      int percent_B = ((count_B * 100) / (count_A + count_B));
  
      Serial.println("Math Done: " + String(percent_A) + " " + String(percent_B));
      
      display.fillScreen(GxEPD_BLACK);
      display.fillRect(0, 0, ratio_A, DISPLAY_HEIGHT, GxEPD_RED);

      if ( percent_A >= 25) {
        display.setCursor(5, 140);
        display.setTextColor(GxEPD_BLACK);
        display.println(String(percent_A) + "%");
      }

      if (percent_B >= 25) {
        display.setCursor(105, 140);
        display.setTextColor(GxEPD_RED);
        display.println(String(percent_B) + "%");
      }
  
      //Push changes to the display
      display.update();
  
      Serial.println("Drawing Done");
    }
  
    //Put the chip to sleep. 
    ESP.deepSleep(6e7); // 20e6 is 20 microseconds
    
}

void text_vis(int countA, int countB) {
  display.fillScreen(GxEPD_WHITE);
  display.drawRect(0, 0, 154, 154, GxEPD_BLACK);

  display.setRotation(1);
  display.setTextColor(GxEPD_BLACK);

  display.setCursor(10, 40);
  display.println("2018");
  display.setCursor(120, 130);
  display.println("2018");

  const GFXfont* f2 = &FreeSansBold24pt7b;  
  display.setFont(f2);
  
  display.setCursor(0, 40);
  display.println("1000");
  display.setCursor(50, 120);
  display.println("9999");

 
  Serial.println("Text Vis");
  display.update();
  ESP.deepSleep(6e7); // 20e6 is 20 microseconds
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  EEPROM.begin(512);
  display.init();

  pinMode(BUTTON_A, INPUT);
  pinMode(BUTTON_B, INPUT);
   
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
   
  Serial.println("Connected to the WiFi network");

  client.setServer(mqttServer, mqttPort);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("TestESP8266Client")) {
      Serial.println("connected");
    }
    else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }


  client.subscribe("iobits/DataResponse");
  client.subscribe("iobits/DataResponsePoints");
  client.setCallback(callback);

  
  //configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");
  configTime(timezone, 0, "pool.ntp.org");  
  setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 0);  
  while(time(nullptr) <= 100000) {
        Serial.print("Connecting to time server...");
        delay(1000);
      }
}

time_t last_pub = 0;
void conditional_publish(char * channel, char * message, time_t time) {
  if (time - last_pub > PUBLISH_WAIT) {
    Serial.println("Sending request...");
    client.publish(channel, message);
    last_pub = time;
  }
}

void loop() {
  if (!client.connected()) {
    Serial.println("WARNING: client disconnected");
  }
  client.loop();

  //delay(1000);

  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  char buff[30];
  strftime(buff, 20, "%Y-%m-%d %H:%M:%S", p_tm);
  //sprintf(buff, "%ld", now);
  
  if(digitalRead(BUTTON_A) == LOW) {   
      char message[35] = "A,A,";
      strcat(message, buff);
      Serial.println("Button A is Low");
      client.publish("iobits/ButtonPress", message);
  } else if(digitalRead(BUTTON_B) == LOW) {
      char message[25] = "A,B,";
      strcat(message, buff);
      Serial.println("Button B is Low");
      client.publish("iobits/ButtonPress", message);
  }  else if(digitalRead(BUTTON_B) == HIGH) {
      //Serial.println("Button B is High");
  }  else if(digitalRead(BUTTON_A) == HIGH) {
      //Serial.println("Button A5 is High");
  }

  conditional_publish("iobits/DataRequest", "A", now);
   //delay(60000);

}
