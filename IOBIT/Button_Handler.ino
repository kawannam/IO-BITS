/*
 * Button_Handler: responds to the button presses.
 * 
 * If you have a different esp32 board and have to change 
 * the pins you are using change the defines here (also see 
 * display to change e-paper pins). Note that if you change
 * the button pins you must change the BUTTON_PIN_BITMASK.
 * (The left most bit is pin 39/VP, the rest are in desending
 * order. 1 for wakeup 0 for ignore. For extra detail watch
 * https://www.youtube.com/watch?v=r75MrWIVIw4&t=719s)
 * 
 * void detect_wakeup_reason() - will print the reason the device
 * wokeup to serial. If it sees that it was a button press 
 * it will call respond_to_button_press
 * 
 * void respond_to_button_press(uint64_t wakeup_pin_mask) - 
 * calls the corresponding action based on which button was
 * pressed
 */
 
//--------------Pins-------------------------------//
#define PIN_A 32
#define PIN_B 33
#define PIN_UNDO 34
#define PIN_NEW_LOGFILE 35 
#define PIN_SWITCH_VIS 39
//-------------------------------------------------//

//--------------Buttons----------------------------//
#define BUTTON_A 0
#define BUTTON_B 1
#define BUTTON_UNDO 2
#define BUTTON_NEW_LOGFILE 3 
#define BUTTON_SWITCH_VIS 4

#define NUMBER_OF_BUTTONS 5
char BUTTONS[] = {'A', 'B', 'U', 'N', 'S'};
int presses[NUMBER_OF_BUTTONS];
//-------------------------------------------------//


void button_setup() {
  /*pinMode(PIN_A, INPUT);
  pinMode(PIN_B, INPUT);
  pinMode(PIN_UNDO, INPUT);
  pinMode(PIN_NEW_LOGFILE, INPUT);
  pinMode(PIN_SWITCH_VIS, INPUT);
  
  attachInterrupt(digitalPinToInterrupt(PIN_A), queue_button_press_A, RISING);
  attachInterrupt(digitalPinToInterrupt(PIN_B), queue_button_press_B, RISING);
  attachInterrupt(digitalPinToInterrupt(PIN_UNDO), queue_button_press_UNDO, RISING);
  attachInterrupt(digitalPinToInterrupt(PIN_NEW_LOGFILE), queue_button_press_NEW_LOGFILE, RISING);
  attachInterrupt(digitalPinToInterrupt(PIN_SWITCH_VIS), queue_button_press_SWITCH_VIS, RISING);*/
}


void detect_wakeup_reason() {
  esp_deep_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_deep_sleep_get_wakeup_cause();
  Serial.println("\nEXT1 Test");
  switch (wakeup_reason)
  {
    case 1  : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case 2  : {
      Serial.println("Wakeup caused by external signal using RTC_CNTL ");
      uint64_t wakeup_pin_mask = esp_sleep_get_ext1_wakeup_status();
      if (wakeup_pin_mask != 0) queue_button_press(__builtin_ffsll(wakeup_pin_mask) - 1);
      else Serial.println("Wake up from unknown GPIO");
      break;
    }
    case 3  : Serial.println("Wakeup caused by timer"); break;
    case 4  : Serial.println("Wakeup caused by touchpad"); break;
    case 5  : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.println("Wakeup was not caused by deep sleep"); break;
  }
}

void  queue_button_press_A() { presses[BUTTON_A]++; }
void  queue_button_press_B() { presses[BUTTON_B]++; }
void  queue_button_press_UNDO() { presses[BUTTON_UNDO]++; }
void  queue_button_press_NEW_LOGFILE() { presses[BUTTON_NEW_LOGFILE]++; }
void  queue_button_press_SWITCH_VIS() { presses[BUTTON_SWITCH_VIS]++; }

void queue_button_press(int pin) {
    switch (pin)
    {
      case PIN_A: {
         Serial.println("PIN_A has been pressed");
         //notify_button_press('A');
         queue_button_press_A();
         break; 
      }
      case PIN_B: {
         Serial.println("PIN_B has been pressed");
         //notify_button_press('B');
         queue_button_press_B();
         break;
      }
      case PIN_UNDO: {
         //notify_button_press('U');
         queue_button_press_UNDO();
         break;
      }
      case PIN_NEW_LOGFILE: {
         Serial.println("PIN_NEW_LOGFILE has been pressed");
         queue_button_press_NEW_LOGFILE();
         //notify_button_press('N');
         break;
      }
      case PIN_SWITCH_VIS:
      {
         Serial.println("PIN_SWITCH_VIS has been pressed");
         //notify_button_press('S');
         queue_button_press_SWITCH_VIS();
         switch_visualization();
         break;
      }
      default:
        break;
    }
}

void respond_to_button_press() {
  for (int i = 0; i < NUMBER_OF_BUTTONS; i++) {
    for (int j = 0; j < presses[i]; j++) {
      notify_button_press(BUTTONS[i]);
    }
    presses[i] = 0;
  }

}

