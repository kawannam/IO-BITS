/*
 * Goal 1: Have 5 buttons wake the esp32 from deepsleep 
 *         upon wakeup have a corresponding led light up
 *         
 * Goal 2: Update screen on wakeup
 * 
 * Goal 3: Get the time of the last button press to display on the screen
 * 
 * Goal 4: Connect to Server
 */
//---------------Include Libraries-----------------//
#include <esp_deep_sleep.h>
#include <GxEPD.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
#include <WiFi.h>
#include <PubSubClient.h>
//------------------------------------------------//

//---------------Fonts----------------------------//
#include <Fonts/FreeSans7pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
//#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSansBold30pt7b.h>
#include <Fonts/FreeSansBold36pt7b.h>

const GFXfont* f7b = &FreeSans7pt7b;
const GFXfont* f9b = &FreeSansBold9pt7b;
const GFXfont* f9 = &FreeSans9pt7b;
//const GFXfont* f24b = &FreeSansBold24pt7b;
const GFXfont* f30b = &FreeSansBold30pt7b;
const GFXfont* f36b = &FreeSansBold36pt7b;
//-------------------------------------------------//

//---------------Customization Info----------------//
#define DISPLAY_TYPE '1.5bwy'

const char* ssid = "***";
const char* password = "***";

char my_name = 'A';

const int timezone = 0 * 3600;
const int dst = 0;
//-------------------------------------------------//

//---------------E-Paper Display------------------//

// Configs/Includes based on which screen we are using
#if DISPLAY_TYPE == '1.5bwy'
#include <GxGDEW0154Z17/GxGDEW0154Z17.h>
#define DISPLAY_HEIGHT 152
#define DISPLAY_WIDTH 152
const String display_type = "1.5bwy";
#endif
#if DISPLAY_TYPE == '1.5bwr'
#include <GxGDEW0154Z04/GxGDEW0154Z04.h>
#define DISPLAY_HEIGHT 200
#define DISPLAY_WIDTH 200
const String display_type = "1.5bwr";
#endif
#if DISPLAY_TYPE == '4.2'
#include <GxGDEW042Z15/GxGDEW042Z15.h>
#define DISPLAY_HEIGHT 300
#define DISPLAY_WIDTH 400
const String display_type = "4.2";
#endif

// Pins screen is connected to
#define BUSY 4
#define RST 16
#define DC 17
#define CS 5
#define CLK 18
#define DIN 23

#define CHIP_SELECT 13 // default: 5

// Defining the display variable
//GxIO_Class io(SPI, CHIP_SELECT, DC, RST);
//GxEPD_Class display(io, RST, BUSY);

GxIO_Class io(SPI, /*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16); // arbitrary selection of 17, 16
GxEPD_Class display(io, /*RST=*/ 16, /*BUSY=*/ 4); // arbitrary selection of (16), 4
//-------------------------------------------------//

//--------------Pins-------------------------------//
#define BUTTON_PIN_BITMASK 0x8F00000000 // pins 32, 33, 34, 35, and 39 will wakeup the chip
#define BUTTON_A 32
#define BUTTON_B 33
#define BUTTON_UNDO 34
#define BUTTON_NEW_LOGFILE 35 
#define BUTTON_SWITCH_VIS 39
//-------------------------------------------------//

//--------------Networking-------------------------//
#define PUBLISH_WAIT 10

const char* mqttServer = "test.mosquitto.org";
const int mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);
const char* device_id = "IOBITClient" + my_name;
int number_of_expected_messages = 0;
//-------------------------------------------------//

//--------------Message Parsing-------------------//
#define DATA_COUNT_A_OFFSET 2
#define DATA_COUNT_B_OFFSET 7
#define DATA_COUNT_LENGTH 4
#define DATA_NUMBER_OF_POINTS_OFFSET 12
#define DATA_NUMBER_OF_POINTS_LENGTH 3
#define MAX_NUMBER_OF_DATA_POINTS 100
//-------------------------------------------------//

//--------------Data Structures--------------------//
struct data_point {
  char button;
  struct tm* time_stamp;
};
struct coord {
  int x;
  int y;
};
//-------------------------------------------------//

//--------------Data------------------------------//
data_point points[MAX_NUMBER_OF_DATA_POINTS];
int count_A = 0;
int count_B = 0;
time_t last_pub = 0;
time_t last_push = 0;
time_t last_button_press = 0;
//-------------------------------------------------//

void print_wakeup_reason() {
  esp_deep_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_deep_sleep_get_wakeup_cause();
  Serial.println("\nEXT1 Test");
  switch (wakeup_reason)
  {
    case 1  : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case 2  : {
      Serial.println("Wakeup caused by external signal using RTC_CNTL "); 
      
      uint64_t wakeup_pin_mask = esp_sleep_get_ext1_wakeup_status();
      if (wakeup_pin_mask != 0) {
          int pin = __builtin_ffsll(wakeup_pin_mask) - 1;
          switch (pin)
          {
            case BUTTON_A:
                Serial.println("BUTTON_A has been pressed");
                break;
            case BUTTON_B:
               Serial.println("BUTTON_B has been pressed");
               break;
            case BUTTON_UNDO:
               Serial.println("BUTTON_UNDO has been pressed");
               break;
            case BUTTON_NEW_LOGFILE:
               Serial.println("BUTTON_NEW_LOGFILE has been pressed");
               break;
            case BUTTON_SWITCH_VIS:
               Serial.println("BUTTON_SWITCH_VIS has been pressed");
               break;
            default:
              break;
          }
            
        } else {
            printf("Wake up from GPIO\n");
        }
     
      break;
    }
    case 3  : Serial.println("Wakeup caused by timer"); break;
    case 4  : Serial.println("Wakeup caused by touchpad"); break;
    case 5  : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.println("Wakeup was not caused by deep sleep"); break;
  }
}

void display_word(const char name[], const GFXfont* f)
{
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(0, 0);
  display.println();
  display.println(name);
  display.update();
  delay(100);
}

void time_to_string(time_t timestamp, char time_string[]) {
  struct tm* p_tm = localtime(&timestamp);
  strftime(time_string, 30, "%Y-%m-%d %H:%M:%S", p_tm);
}

void setup() {
  Serial.begin(115200);
  delay(1000); //Take some time to open up the Serial Monitor

  //Connecting Networks
  Serial.println("Connecting Networks");
  connect_to_wifi();
  connect_to_time_server();
  connect_to_mqtt();

  //Getting time right now
  Serial.println("Getting time right now");
  char time_string[30];
  time_t now = time(nullptr);
  time_to_string(now, time_string);

  //Updating Display
  Serial.println("Updating Display");
  display.init();
  display_word(time_string, &FreeMonoBold9pt7b);

  //Respond to wakeup cause
  Serial.println("Respond to wakeup cause");
  print_wakeup_reason();

  //Enabling Wakeup
  Serial.println("Enable Wakeup");
  esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK,ESP_EXT1_WAKEUP_ANY_HIGH);

  //Going to sleep now
  Serial.println("Going to sleep now");
  delay(1000);
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

void loop() {
  // put your main code here, to run repeatedly:
}

