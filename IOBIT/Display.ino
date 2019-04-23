/*
 * Display: Handles general display items, decides which
 * visualization to display, and updates the screen
 * 
 * E-Paper Display Type - The code should work on a 
 * variety of screen sizes. Based on the indicated
 * screen type in "Customization Info" this section 
 * will include the right lib and configure the consts
 * 
 * E-Paper Display Pins - This section defines the pins
 * the e-paper is connected too, if you have a different
 * board you may need to change these.
 * 
 * Fonts - makes font object for the specific vis tabs
 * to utilize
 * 
 * General Visualization Defines - misc definitions
 * 
 * void setup_display() - initializes the display, and 
 * does any screen specific setup steps
 * 
 * void switch_visualization() - changes vis_option, 
 * indicating that a different visualization should be
 * displayed. vis_option is a special type of variable
 * that is maintained throughout deepsleep (RTC_DATA_ATTR)
 * 
 * void update_vis() - based on vis_option calls the
 * coresponding vis tab and updates the screen. Addtionally,
 * it prints to serail which vis is being called
 * 
 * void display_error(const char error[], const GFXfont* f) -
 * If an error occures (i.e. cannot connect to the wifi) the
 * screen will display the error message.
 * 
 * Note to Kendra: implement utilizing error screen
 * 
 */


//---------------E-Paper Display Type--------------//
// Configs/Includes based on which screen we are using

#if DISPLAY_TYPE == '1.5bw'
#include <GxGDEP015OC1/GxGDEP015OC1.h>
#define DISPLAY_HEIGHT 200
#define DISPLAY_WIDTH 200
#define GxCOLOUR GxEPD_BLACK 
const String display_type = "1.5bwy";
#endif
#if DISPLAY_TYPE == '1.5bwy'
#include <GxGDEW0154Z17/GxGDEW0154Z17.h>
#define DISPLAY_HEIGHT 152
#define DISPLAY_WIDTH 152
#define GxCOLOUR GxEPD_RED
const String display_type = "1.5bwy";
#endif
#if DISPLAY_TYPE == '1.5bwr'
#include <GxGDEW0154Z04/GxGDEW0154Z04.h>
#define DISPLAY_HEIGHT 200
#define DISPLAY_WIDTH 200
#define GxCOLOUR GxEPD_RED
const String display_type = "1.5bwr";
#endif
#if DISPLAY_TYPE == '4.2'
#include <GxGDEW042Z15/GxGDEW042Z15.h>
#define DISPLAY_HEIGHT 300
#define DISPLAY_WIDTH 400
#define GxCOLOUR GxEPD_RED
const String display_type = "4.2";
#endif
//-------------------------------------------------//

//---------------E-Paper Display Pins--------------//
#define BUSY 4
#define RST 16
#define DC 17
#define CS 5
#define CLK 18
#define DIN 23

GxIO_Class io(SPI, CS, DC, RST);
GxEPD_Class display(io, RST, BUSY);
//-------------------------------------------------//

//---------------Fonts-----------------------------//
const GFXfont* f7b = &FreeSans7pt7b;
const GFXfont* f9b = &FreeSansBold9pt7b;
const GFXfont* f9m = &FreeMonoBold9pt7b;
const GFXfont* f9 = &FreeSans9pt7b;
const GFXfont* f18b = &FreeMonoBold18pt7b;
const GFXfont* f24b = &FreeSansBold24pt7b;
const GFXfont* f30b = &FreeSansBold30pt7b;
const GFXfont* f36b = &FreeSansBold36pt7b;
//-------------------------------------------------//

//---------------General Visualization Defines-----//
#define NUMBER_OF_VIS_OPTIONS 5

char days[7] = {'U', 'M', 'T', 'W', 'R', 'F', 'S'};
RTC_DATA_ATTR int vis_option = 3;
const int space_for_titles = 15;
//-------------------------------------------------//


void setup_display() {
  display.init();
  if ((display_type == "1.54bwr") || (display_type == "1.5bwy")) {
    display.setRotation(1);
  }
}


void switch_visualization() {
  vis_option = (vis_option + 1) % NUMBER_OF_VIS_OPTIONS;
}


void update_vis() {
    if ( current_number_of_points == 0 ) {
      no_data_display();
    } else {
      switch (vis_option) {
        case 0: 
          Serial.println("Displaying: Dotted Week Visualization");
          dotted_week();
          break;
        case 1:
          Serial.println("Displaying: Tug of War Visualization");
          tug_of_war();
          break;
        case 2:
          Serial.println("Displaying: Plain Text Visualization");
          plain_text();
          break;
        case 3:
          Serial.println("Displaying: Streaks and Valleys Visualization");
          streaks_and_valleys();
          break;
        default:
          Serial.println("Displaying: Stacked Bars Visualization");
          stacked_bars();
          break;
      }
    }
    Serial.println("Sending update command to screen");
    display.update();
    delay(100);
    Serial.println("Update command returned");
}


void display_error(const char error[], const GFXfont* f)
{
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(0, 0);
  display.println();
  display.println(error);
}





