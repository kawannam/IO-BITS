/*
 * Goal 1: Have 5 buttons wake the esp32 from deepsleep 
 *         upon wakeup have a corresponding led light up
 */

#include <esp_deep_sleep.h>

#define BUTTON_PIN_BITMASK 0x8F00000000 // pins 32, 33, 34, 35, and 39 will wakeup the chip
#define PURPLE 23
#define BLUE 22
#define GREEN 21
#define ORANGE 18
#define YELLOW 19

void print_wakeup_reason() {
  esp_deep_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_deep_sleep_get_wakeup_cause();
  
  Serial.println("");
  Serial.println("");
  Serial.println("EXT1 Test");
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
               digitalWrite(PURPLE, HIGH);
               delay(5000);
               digitalWrite(PURPLE, LOW);
                break;
            case 34:
               digitalWrite(BLUE, HIGH);
               delay(5000);
               digitalWrite(BLUE, LOW);
              break;
            case 35:
               digitalWrite(GREEN, HIGH);
               delay(5000);
               digitalWrite(GREEN, LOW);
               break;
            case 33:
               digitalWrite(ORANGE, HIGH);
               delay(5000);
               digitalWrite(ORANGE, LOW);
               break;
            case 32:
               digitalWrite(YELLOW, HIGH);
               delay(5000);
               digitalWrite(YELLOW, LOW);
               break;
            default:
              digitalWrite(YELLOW, HIGH);
              delay(5000);
              digitalWrite(YELLOW, LOW);
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

void setup() {
  Serial.begin(115200);
  delay(1000); //Take some time to open up the Serial Monitor

  pinMode(PURPLE, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(ORANGE, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);
 
  
  print_wakeup_reason();

  //If you were to use ext1, you would use it like
  //Serial.println(esp_deep_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK, ESP_EXT1_WAKEUP_ANY_HIGH));
  esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK,ESP_EXT1_WAKEUP_ANY_HIGH);

  //Go to sleep now

  Serial.println("Going to sleep now");
  delay(100);
  esp_deep_sleep_start();
  Serial.println("This will never be printed");

}

void loop() {
  // put your main code here, to run repeatedly:

}
