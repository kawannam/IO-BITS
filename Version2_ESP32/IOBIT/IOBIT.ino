//---------------Libraries------------------------//
//Sleep
#include <esp_deep_sleep.h>

//Display
#include <GxEPD.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>

//Time
#include <time.h>

//Wifi
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
//------------------------------------------------//

//---------------Customization Info----------------//
#define DISPLAY_TYPE '1.5bwy'

const char* ssid = "***";
const char* password = "***";

char my_name = 'A';

const int timezone = 0 * 3600;
const int dst = 0;
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

//--------------Public Constants and Defines-------//
#define BUTTON_PIN_BITMASK 0x8F00000000 // pins 32, 33, 34, 35, and 39 will wakeup the chip
//-------------------------------------------------//

//--------------Public Variables-------------------//
WiFiClient espClient;
PubSubClient client(espClient);
RTC_DATA_ATTR int vis_option = 0;
//-------------------------------------------------//


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

  //Respond to wakeup cause
  Serial.println("Respond to wakeup cause");
  detect_wakeup_reason();

  //Request Data
  request_data();

  //Updating Display
  Serial.println("Updating Display");
  setup_display();
  update_vis();

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
}

