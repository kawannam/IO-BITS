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
#include <GxEPD2_32_BW.h>
#include <GxEPD2_32_3C.h>


//Time
#include <time.h>

//Wifi
#include <WiFi.h>
#include <PubSubClient.h>

//


//Fonts
#include <Fonts/FreeSans7pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSansBold30pt7b.h>
#include <Fonts/FreeSansBold36pt7b.h>
//------------------------------------------------//

//---------------Customization Info----------------//
//Indicate the exact screen type
#define DISPLAY_TYPE '1.5bw'

//Internet credentials 


//Device specific char
char my_name = 'A';

//Set Timezone - Currently set for MST
const int timezone = -7 * 3600;
const int dst = 0;
//time_t current_time;
struct tm current_time_tm;
//-------------------------------------------------//

//--------------Data Structures--------------------//
struct data_point {
  char button;
  struct tm timestamp;
};
struct coord {
  int x;
  int y;
};
struct streaks {
  int A;
  int B;
  int Total;
};
//-------------------------------------------------//

//--------------Public Constants and Defines-------//
#define BUTTON_PIN_BITMASK 0x9700000000 // pins 32, 33, 34, 35, and 39 will wakeup the chip
#define STATUS_LIGHT 27
#define MAX_NUMBER_OF_CONNECTION_TRIES 10
//#define STATUS_LIGHT 13
//-------------------------------------------------//

//--------------Public Variables-------------------//
bool connected_to_wifi = false;
bool connected_to_mqtt = false;

//-------------------------------------------------//

//Temp
data_point points[100];
int count_A = 0;
int count_B = 0;
int current_number_of_points = 0;


void setup() {
  pinMode(STATUS_LIGHT, OUTPUT);
  digitalWrite(STATUS_LIGHT, HIGH);
  
  Serial.begin(115200);
  delay(100); //Take some time to open up the Serial Monitor

  digitalWrite(STATUS_LIGHT, LOW);
  

  //Respond to wakeup cause
  Serial.println("Detect wakeup cause");
  detect_wakeup_reason();

  //Initiallize button interupts
  Serial.println("Initialize Button Interupts");
  button_setup();

  setup_display();

  //Connecting Networks
  Serial.println("Connecting Networks");
  
  if (!connect_to_wifi()) {
    sleep();
    return;
  }
  
  if (!connect_to_mqtt()) {
    sleep();
    return;
  }

  
  digitalWrite(STATUS_LIGHT, HIGH);
  do {
    Serial.println("RESPOND TO BUTTON PRESSES");
    respond_to_button_press();
  
    //Request Data
    Serial.println("REQUEST DATA");
    if (!request_data()) {
      sleep();
      return;
    }

    char buff[30];
    for ( int i = 0; i < 100; i++) {
      time_to_string(points[i].timestamp, buff);
      Serial.println("Index: " + String(i) + " Button " + String(points[i].button) + " Time: " + buff);
    }
  
    //Updating Display
    Serial.println("Updating Display");
    update_vis();
    
  } while (button_press_check());
  
  sleep();
}

void sleep() {
//Enabling Wakeup
  Serial.println("Enable Wakeup");
  esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK,ESP_EXT1_WAKEUP_ANY_HIGH);

   //Going to sleep now
  Serial.println("Going to sleep now");
  digitalWrite(STATUS_LIGHT, LOW);
  delay(1000);
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

void loop() {
}

