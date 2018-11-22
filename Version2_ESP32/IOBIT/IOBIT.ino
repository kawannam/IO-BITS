/*
 * Goal 1: Have 5 buttons wake the esp32 from deepsleep 
 *         upon wakeup have a corresponding led light up
 *         
 * Goal 2: Update screen on wakeup
 */

#include <esp_deep_sleep.h>
#include <GxEPD.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

#include <Fonts/FreeMonoBold9pt7b.h>

//---------------Customization Info----------------//
#define DISPLAY_TYPE '1.5bwy'
//------------------------------------------------//

//---------------E-Paper Display Defines----------//

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
//------------------------------------------------//

//--------------Pin Defines----------------------//



#define BUTTON_PIN_BITMASK 0x8F00000000 // pins 32, 33, 34, 35, and 39 will wakeup the chip
//------------------------------------------------//

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
            case 39:
                break;
            case 34:
              break;
            case 35:
               break;
            case 33:
               break;
            case 32:
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

void showFont(const char name[], const GFXfont* f)
{
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(0, 0);
  display.println();
  display.println(name);
  display.println(" !\"#$%&'()*+,-./");
  display.println("0123456789:;<=>?");
  display.println("@ABCDEFGHIJKLMNO");
  display.println("PQRSTUVWXYZ[\\]^_");
  display.println("`abcdefghijklmno");
  display.println("pqrstuvwxyz{|}~ ");
  display.update();
  delay(10000);
}

void setup() {
  Serial.begin(115200);
  delay(1000); //Take some time to open up the Serial Monitor

  display.init();
  showFont("FreeMonoBold9pt7b", &FreeMonoBold9pt7b);

  
 
  
  print_wakeup_reason();

  //If you were to use ext1, you would use it like
  //Serial.println(esp_deep_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK, ESP_EXT1_WAKEUP_ANY_HIGH));
  esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK,ESP_EXT1_WAKEUP_ANY_HIGH);

  //Go to sleep now

  Serial.println("Going to sleep now");
  delay(1000);
  esp_deep_sleep_start();
  Serial.println("This will never be printed");

}

void loop() {
  // put your main code here, to run repeatedly:

}
