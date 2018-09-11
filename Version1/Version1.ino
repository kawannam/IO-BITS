#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include <GxEPD.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans7pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSansBold30pt7b.h>
#include <Fonts/FreeSansBold36pt7b.h>
#include <time.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
#include <GxIO/GxIO.cpp>

//---------------Customization Info----------------//

char my_name = 'B';
// display type: 1.5 (bwr/bwy) or 4.2
#define DISPLAY_TYPE '1.5bwy'

const int timezone = 0 * 3600;
const int dst = 0;
//------------------------------------------------//
#if DISPLAY_TYPE == '1.5bwy'
#include <GxGDEW0154Z17/GxGDEW0154Z17.cpp>
#define DISPLAY_HEIGHT 152
#define DISPLAY_WIDTH 152
const String display_type = "1.5bwy";
#endif
#if DISPLAY_TYPE == '1.5bwr'
#include <GxGDEW0154Z04/GxGDEW0154Z04.cpp>
#define DISPLAY_HEIGHT 200
#define DISPLAY_WIDTH 200
const String display_type = "1.5bwr";
#endif
#if DISPLAY_TYPE == '4.2'
#include <GxGDEW042Z15/GxGDEW042Z15.cpp>
#define DISPLAY_HEIGHT 300
#define DISPLAY_WIDTH 400
const String display_type = "4.2";
#endif

#define BUSY 12 //D6
#define RST 2 //D4
#define DC 0 //D3
#define CS 15 //D8
#define CLK 14 //D5
#define DIN 13 //D7

#define BUTTON_A 5 //D1
#define BUTTON_B 4 //D2
#define BUTTON_UNDO 5 //SDD3
#define BUTTON_NEW_LOGFILE 4 //SDD2

#define NUMBER_OF_VIS_OPTIONS 5


#define CHIP_SELECT 13 // default: 5

#define DEVICE_OFFSET 0
#define DEVICE_LENGTH 1

#define PUBLISH_WAIT 10
#define PUSH_WAIT 2

//For a data message
#define DATA_COUNT_A_OFFSET 2
#define DATA_COUNT_B_OFFSET 7
#define DATA_COUNT_LENGTH 4
#define DATA_NUMBER_OF_POINTS_OFFSET 12
#define DATA_NUMBER_OF_POINTS_LENGTH 3
#define MAX_NUMBER_OF_DATA_POINTS 100

const char* mqttServer = "test.mosquitto.org";
const int mqttPort = 1883;

const GFXfont* f7b = &FreeSans7pt7b;
const GFXfont* f9b = &FreeSansBold9pt7b;
const GFXfont* f9 = &FreeSans9pt7b;
const GFXfont* f24b = &FreeSansBold24pt7b;
const GFXfont* f30b = &FreeSansBold30pt7b;
const GFXfont* f36b = &FreeSansBold36pt7b;

struct data_point {
  char button;
  String time_stamp;
};
struct coord {
  int x;
  int y;
};

data_point points[MAX_NUMBER_OF_DATA_POINTS];
int count_A = 0;
int count_B = 0;
time_t last_pub = 0;
time_t last_push = 0;

GxIO_Class io(SPI, CHIP_SELECT, DC, RST);
GxEPD_Class display(io, RST, BUSY);

WiFiClient espClient;
PubSubClient client(espClient);
int number_of_expected_messages = 0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  EEPROM.begin(512);
  display.init();

  pinMode(BUTTON_A, INPUT);
  pinMode(BUTTON_B, INPUT);
  pinMode(BUTTON_UNDO, INPUT);
  pinMode(BUTTON_NEW_LOGFILE, INPUT);

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

  randomSeed(analogRead(0));

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

   if (display_type == "1.54bwr") {
      display.setRotation(1);
   }
}

void loop() {
  if (!client.connected()) {
    Serial.println("WARNING: client disconnected");
  }
  detect_button_press();
  client.loop();
  time_t now = time(nullptr);
  conditional_publish("iobits/DataRequest", &my_name, now);
  //button_test();
}
