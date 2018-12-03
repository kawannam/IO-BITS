//---------------Fonts----------------------------//
const GFXfont* f7b = &FreeSans7pt7b;
const GFXfont* f9b = &FreeSansBold9pt7b;
const GFXfont* f9 = &FreeSans9pt7b;
//const GFXfont* f24b = &FreeSansBold24pt7b;
const GFXfont* f30b = &FreeSansBold30pt7b;
const GFXfont* f36b = &FreeSansBold36pt7b;
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

GxIO_Class io(SPI, /*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16); // arbitrary selection of 17, 16
GxEPD_Class display(io, /*RST=*/ 16, /*BUSY=*/ 4); // arbitrary selection of (16), 4
//-------------------------------------------------//
void setup_display() {
  display.init();
  if (display_type == "1.54bwr") {
    display.setRotation(1);
  }
  randomSeed(analogRead(0));
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

void update_vis() {
    Serial.println("All messages recieved - deciding on vis");
    switch (vis_option) {
      case 0: 
        dotted_week();
        break;
      case 1:
        tug_of_war();
        break;
      case 2:
        plain_text();
        break;
      case 3:
        streaks_and_valleys();
        break;
      default:
        stacked_bars();
        break;
    }
    display.update();
}


