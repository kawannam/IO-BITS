/* 
 *  IOBIT: this is the main file (also known as tab) for this
 *  ardino project. This project is a simple technology assisted
 *  self-logging tool. The device will wakeup on a button press
 *  send a timestamp to the server and update the visualization
 *  displayed on the screen before going back into deepsleep.
 *  
 *  I am using an ESP32 dev board, a 1.54" 3 colour e-paper 
 *  display (should also works with 4.2 3 colour displays),
 *  5 push buttons, and a lipo battery. The pins for other
 *  esp32 boards may vary
 *  
 *  Created by Kendra Wannamaker 2018, working with
 *  Dr. Wesley Willett and Dr. Marian Dörk.
 *  
 *  This file:
 *  - Imports all needed libraries
 *  - Declares all global variables
 *  - Defines all data structures
 *  - Contains setup() which is called upon wakeup from
 *    deepslee and is used as the main driver for the 
 *    application
 *  - Contains loop() which is never called
 *
 */

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

//Fonts
#include <Fonts/FreeSans7pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
//#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSansBold30pt7b.h>
#include <Fonts/FreeSansBold36pt7b.h>
//------------------------------------------------//

//---------------Customization Info----------------//
//Indicate the exact screen type
#define DISPLAY_TYPE '1.5bwy'

//Internet credentials 
const char* ssid = "***";
const char* password = "***";

//Device specific char
char my_name = 'A';

//Set Timezone - Currently set for MST
const int timezone = -6 * 60;
const int dst = 2;
//-------------------------------------------------//

//--------------Data Structures--------------------//
struct data_point {
  char button;
  time_t timestamp;
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

