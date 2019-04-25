
#ifndef GLIDETIMECONF_H_
#define GLIDETIMECONF_H_

#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Bounce2.h>
#include <EEPROM.h>



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
#define EEPROM_ADDR_CALIBRATION_DATA 0x10




class GlideTimeConfig {
  Bounce debouncer1;
  Bounce debouncer2;
  ulong_t clock_cal_millis_per_correction; // number of millis between corrections
  int clock_cal_direction;  // + is add to running correction; - is subtract.
  stateRestoreClockCalibration();
  stateRestoreLcdContrast();

private:
  set_clock_cal();

public:
  Adafruit_PCD8544* display;  // TODO: move all common structures into base new base class and derive from that

  GlideTimeConfig(){
    debouncer1.attach(BUTTON_1_PIN);
    debouncer2.attach(BUTTON_2_PIN);
    debouncer1.interval(button_debounce_time);
    debouncer2.interval(button_debounce_time);
    set_clock_cal();
  }

  // Apply a +1 or -1 clock correction factor every n millis. Returns n.
  ulong_t clock_correction_increment_period(){
    return clock_cal_millis_per_correction;
  }

  // returns +1 or -1 clock correction depending on calibration results
  int clock_correction_direction(){
    return clock_cal_direction;
  }


  configureClockCalibration(){
    // wait for 1-second markers from Serial
    Serial.begin(115200);
    // Read serial for 60 1-second intervals
    Serial.write("start\n");
    ulong_t begin_ms = millis();
    for(uint_t i=0; i < 61; i++){
//      Serial.write("Syncing");
      while( Serial.available() == 0 );
      //while( Serial.read() != -1 );
      Serial.read();
      display->clearDisplay();
      display->setCursor(5,20);
      display->print(i);
      display->display();
    }

    ulong_t end_ms = millis();
    display->clearDisplay();
    display->setCursor(0,10);
    display->print("start:");
    display->println(begin_ms);
    display->print("end:");
    display->println(end_ms);
    display->print("diff: ");
    display->println(end_ms-begin_ms);
    display->display();
    // Calculate ms error and save to eeprom
    ulong_t ms_diff_per_minute = 60000 - (end_ms-begin_ms);
    EEPROM.put(EEPROM_ADDR_CALIBRATION_DATA, ms_diff_per_minute);
    set_clock_cal();
  }


  enterConfigScreen(){
    display->clearDisplay();
    display->println("Configuration");
    display->println("1:clock calib");
    display->display();
    /// Wait for buttons to be released
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
        display->println("Syncing...");
        display->display();
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
    gtConfig.display = display;
  }

  setInputPins(){
    // relies on #defines in somewhat poor form
    pinMode(BUTTON_1_PIN, INPUT_PULLUP);
    pinMode(BUTTON_2_PIN, INPUT_PULLUP);
    pinMode(BUTTON_3_PIN, INPUT_PULLUP);
    pinMode(BUTTON_4_PIN, INPUT_PULLUP);
    pinMode(BUTTON_5_PIN, INPUT_PULLUP);
  }
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
      gtConfig.enterConfigScreen();
    }
  }

  GlideTimeConfig& get_config(){
    return gtConfig;
  }

};

#endif
