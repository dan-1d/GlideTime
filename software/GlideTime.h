
#ifndef GLIDETIMECONF_H_
#define GLIDETIMECONF_H_

#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Bounce2.h>



//Global Defines
typedef unsigned long ulong_t;
typedef unsigned int uint_t;

#define VERSION "v.1.0.1"
#define FLIGHT_HISTORY_MAX  100   // limit of statically-allocated flight time history (intervalA)
#define FLIGHT_HISTORY_DISPLAY_LENGTH  5  // number of previous flights to display at a time. Limited by LCD size and UI choices
#define LCD_CONTRAST 60 // Specific value for each particular Nokie 5110 LCD vendor. It varies depending on supplier.
#define BUTTON_1_PIN 2
#define BUTTON_2_PIN 3
#define BUTTON_3_PIN 4
#define BUTTON_4_PIN 5
#define BUTTON_5_PIN 6
#define VBATT_PIN A0
#define button_debounce_time  50  //hold time of rising edge. 'high' required for this long before triggering
#define screen_refresh_period 100
ulong_t screen_refresh_last_time = 0;



class GlideTimeConfig {
  Bounce debouncer1;
  Bounce debouncer2;
  stateRestoreClockCalibration();
  stateRestoreLcdContrast();
public:
  GlideTimeConfig(){
    debouncer1.attach(BUTTON_1_PIN);
    debouncer2.attach(BUTTON_2_PIN);
    debouncer1.interval(button_debounce_time);
    debouncer2.interval(button_debounce_time);
  }


  configureClockCalibration(){
    Serial.begin(115200);
    // wait for a 1-second marker from Serial
    int begin_ms = millis();
    // Read serial for 60 1-second intervals

    int end_ms = millis();

    // Calculate ms error and save to eeprom
  }


  enterConfigScreen(Adafruit_PCD8544& _display){
    _display.clearDisplay();
    _display.println("Configuration");
    _display.println("1: sync via Serial");
    _display.display();
    /// Wait for buttons to be released
    delay(1000);
    while(true){
      debouncer1.update();
      debouncer2.update();
      int button1State = 1 - debouncer1.read();
      int button2State = 1 - debouncer2.read();
      if( button1State == 0 && button2State == 0 ){
        break;
      }
    }
    // Manage configuration menu.
    while(true){
      debouncer1.update();
      debouncer2.update();
      int button1State = 1 - debouncer1.read();
      int button2State = 1 - debouncer2.read();
      if( button1State == 1 && button2State == 1 ){
        break;  /// Exit setup if both buttons pressed again
      }
      if( debouncer1.rose() ){
        /// Synchronize to serial input reference clock
        _display.println("Syncing...");
        _display.display();
        configureClockCalibration();
      }
    }
  }
};



class GlideTimeInit {
private:
  Adafruit_PCD8544* display;
  GlideTimeConfig gtConfig;
public:
  /// Initialize with the display object
  GlideTimeInit(Adafruit_PCD8544& _display){
    display = &_display;
  }

  setInputPins(){
    // relies on #defines in somewhat poor form
    pinMode(BUTTON_1_PIN, INPUT_PULLUP);
    pinMode(BUTTON_2_PIN, INPUT_PULLUP);
    pinMode(BUTTON_3_PIN, INPUT_PULLUP);
    pinMode(BUTTON_4_PIN, INPUT_PULLUP);
    pinMode(BUTTON_5_PIN, INPUT_PULLUP);
  }
public:
  /// Call Init once from "setup" routine of Arduino
  /// Setup device pins appropriately as per input and output
  /// Restore permanent state of configuration options from EEPROM:
  ///  - Timer calibration
  ///  - LCD contrast
  initialize(){
    setInputPins();
    /// Enter Config Screen if both button1 and button2 are pressed at bootup
    int button1State = 1 - digitalRead(BUTTON_1_PIN);
    int button2State = 1 - digitalRead(BUTTON_2_PIN);
    if( button1State == 1 && button2State == 1 ){
      gtConfig.enterConfigScreen(*display);
    }
  }

};

#endif
