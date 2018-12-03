
//--------------Pins-------------------------------//
#define BUTTON_PIN_BITMASK 0x8F00000000 // pins 32, 33, 34, 35, and 39 will wakeup the chip
#define BUTTON_A 32
#define BUTTON_B 33
#define BUTTON_UNDO 34
#define BUTTON_NEW_LOGFILE 35 
#define BUTTON_SWITCH_VIS 39
//-------------------------------------------------//

#define NUMBER_OF_VIS_OPTIONS 5

void detect_wakeup_reason() {
  esp_deep_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_deep_sleep_get_wakeup_cause();
  Serial.println("\nEXT1 Test");
  switch (wakeup_reason)
  {
    case 1  : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case 2  : {
      Serial.println("Wakeup caused by external signal using RTC_CNTL "); 
      respond_to_button_press(esp_sleep_get_ext1_wakeup_status());
      break;
    }
    case 3  : Serial.println("Wakeup caused by timer"); break;
    case 4  : Serial.println("Wakeup caused by touchpad"); break;
    case 5  : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.println("Wakeup was not caused by deep sleep"); break;
  }
}

void respond_to_button_press(uint64_t wakeup_pin_mask) {
  if (wakeup_pin_mask != 0) {
      int pin = __builtin_ffsll(wakeup_pin_mask) - 1;
      
      switch (pin)
      {
        case BUTTON_A: {
           Serial.println("BUTTON_A has been pressed");
           notify_button_press('A');
           break; 
        }
        case BUTTON_B: {
           Serial.println("BUTTON_B has been pressed");
           notify_button_press('B');
           break;
        }
        case BUTTON_UNDO: {
           notify_button_press('U');
           break;
        }
        case BUTTON_NEW_LOGFILE: {
           Serial.println("BUTTON_NEW_LOGFILE has been pressed");
           notify_button_press('N');
           break;
        }
        case BUTTON_SWITCH_VIS:
        {
           Serial.println("BUTTON_SWITCH_VIS has been pressed");
           notify_button_press('S');
           vis_option = (vis_option + 1) % NUMBER_OF_VIS_OPTIONS;
           break;
        }
        default:
          break;
      }
        
    } else {
        printf("Wake up from GPIO\n");
    }
}

