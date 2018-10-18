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
#include <Wire.h>
#include "Adafruit_MCP23017.h"

extern "C" {
  #include "gpio.h"
}

extern "C" {
  #include "user_interface.h"
}

//---------------Customization Info----------------//

char my_name = 'A';
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

#define BUTTON_A 0
#define BUTTON_B 1 
#define BUTTON_UNDO 2 
#define BUTTON_NEW_LOGFILE 3 
#define BUTTON_SWITCH_VIS 4 

#define MCP_CONNECTION_1 5
#define MCP_CONNECTION_2 4 

#define NUMBER_OF_VIS_OPTIONS 5

#define CHIP_SELECT 13 // default: 5

#define DEVICE_OFFSET 0
#define DEVICE_LENGTH 1

#define PUBLISH_WAIT 10
#define BUTTON_PRESS_WAIT 10
#define PUSH_WAIT 2
#define MAX_ATTEMPTS 20

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
const char* device_id = "IOBITClient" + my_name;

Adafruit_MCP23017 mcp;

struct data_point {
  char button;
  struct tm* time_stamp;
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
time_t last_button_press = 0;
bool awake = false;
bool connected_to_wifi = false;
bool connected_to_mqtt = false;
bool connected_to_timeserver = false;
int wifi_connection_attempts = 0;
int mqtt_connection_attempts = 0;
int time_connection_attempts = 0;

GxIO_Class io(SPI, CHIP_SELECT, DC, RST);
GxEPD_Class display(io, RST, BUSY);

WiFiClient espClient;
PubSubClient client(espClient);
int number_of_expected_messages = 0;

void connect_to_wifi() {
  wifi_connection_attempts = 0;
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED  && wifi_connection_attempts < MAX_ATTEMPTS) {
    delay(500);
    Serial.println("Connecting to WiFi..");
    wifi_connection_attempts++;
  }
  if ( WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to the WiFi network");
    connected_to_wifi = true;
  }
  
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
}

void initialize_display() {
  display.init();

  if (display_type == "1.54bwr") {
    display.setRotation(1);
  }
  randomSeed(analogRead(0));
}

void initialize_memory() {
  EEPROM.begin(512);
}

void initialize_input() {
  pinMode(MCP_CONNECTION_1,INPUT);
  
  mcp.begin();

  mcp.setupInterrupts(true,false,LOW);

  mcp.pinMode(BUTTON_A, INPUT);
  mcp.pullUp(BUTTON_A, HIGH);  // turn on a 100K pullup internally
  mcp.setupInterruptPin(BUTTON_A,FALLING);
  
  mcp.pinMode(BUTTON_B, INPUT);
  mcp.pullUp(BUTTON_B, HIGH);  // turn on a 100K pullup internally
  mcp.setupInterruptPin(BUTTON_B,FALLING);
  
  mcp.pinMode(BUTTON_UNDO, INPUT);
  mcp.pullUp(BUTTON_UNDO, HIGH);  // turn on a 100K pullup internally
  mcp.setupInterruptPin(BUTTON_UNDO,FALLING);
  
  mcp.pinMode(BUTTON_NEW_LOGFILE, INPUT);
  mcp.pullUp(BUTTON_NEW_LOGFILE, HIGH);  // turn on a 100K pullup internally
  mcp.setupInterruptPin(BUTTON_NEW_LOGFILE,FALLING);
  
  mcp.pinMode(BUTTON_SWITCH_VIS, INPUT);
  mcp.pullUp(BUTTON_SWITCH_VIS, HIGH);  // turn on a 100K pullup internally
  mcp.setupInterruptPin(BUTTON_SWITCH_VIS,FALLING);
}

void connect_to_mqtt() {
  if (!connected_to_wifi) {
    Serial.println("Cannot connect to wifi - therefor will not attempt connecting to MQTT server");
    connected_to_mqtt = false;
    return;
  }
  mqtt_connection_attempts = 0;
  client.setServer(mqttServer, mqttPort);
  WiFi.mode(WIFI_STA);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect(device_id)) {
      Serial.println("connected");
      connected_to_mqtt = true;
    }
    else {
      Serial.println("failed with state ");
      Serial.print(client.state());
      delay(2000);
      connected_to_mqtt = false;
    }
    if (mqtt_connection_attempts >= MAX_ATTEMPTS) {
      Serial.println("Failed to connect to the MQTT Sever");
      break;
    }
  }

  client.subscribe("iobits/DataResponse");
  client.subscribe("iobits/DataResponsePoints");
  client.setCallback(message_callback);
}

void connect_to_time_server() {
   if (!connected_to_wifi) {
    Serial.println("Cannot connect to wifi - therefor will not attempt connecting to Time server");
    connected_to_mqtt = false;
    return;
  } 
  //configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");
  configTime(timezone, 0, "pool.ntp.org");  
  setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 0);  
  while(time(nullptr) <= 100000) {
        Serial.print("Connecting to time server...");
        delay(1000);
      }
  Serial.println("Connected");
}

void wakeup()
{
  Serial.println("Waking Up");
  wifi_fpm_do_wakeup();
  wifi_fpm_close();
  wifi_set_opmode(STATION_MODE);
  wifi_station_connect();
  connect_to_mqtt();
  connect_to_time_server();
  awake = true;
}

void setup() {
  Serial.begin(115200);
  delay(300);
  connect_to_wifi();
  initialize_memory();
  initialize_input();
  awaken_callback();
  //connect_to_mqtt();
  //connect_to_time_server();
}

void loop() {
  if (awake == false)  {
    Serial.println("Loop Initializing wake up");
    awaken_callback();
  }
  client.loop();
  time_t now = time(nullptr);
  //detect_button_press(now);
  conditional_publish("iobits/DataRequest", &my_name, now);
}
