#ifndef GLIDETIMECONF_H_
#define GLIDETIMECONF_H_

//#include <U8g2lib.h>
#include <Bounce2.h>
#include <EEPROM.h>
#include "GlideTime.h"
#include "consts.h"

class GlideTimeMain;


class GlideTimeConfig {
private:
  GlideTimeMain* gtMain;
  Bounce debouncer1;
  Bounce debouncer2;
  ulong_t clock_cal_millis_per_correction; // number of millis between corrections
  int clock_cal_direction;  // + is add to running correction; - is subtract.
  stateRestoreClockCalibration();
  stateRestoreLcdContrast();

private:
  set_clock_cal();

public:
  U8G2* display;  // TODO: move all common structures into base new base class and derive from that

  GlideTimeConfig(){
    debouncer1.attach(BUTTON_1_PIN);
    debouncer2.attach(BUTTON_2_PIN);
    debouncer1.interval(BUTTON_DEBOUNCE_TIME);
    debouncer2.interval(BUTTON_DEBOUNCE_TIME);
    set_clock_cal();
  }

  // Set the GlideTimeMain object to use for calibration routine
  void set_GlideTimeMainObj(GlideTimeMain* _gtMain){
    gtMain = _gtMain;
  }

  // Apply a +1 or -1 clock correction factor every n millis. Returns n.
  ulong_t clock_correction_increment_period(){
    return clock_cal_millis_per_correction;
  }

  // returns +1 or -1 clock correction depending on calibration results
  int clock_correction_direction(){
    return clock_cal_direction;
  }

  void configureClockCalibration();



  void enterConfigScreen(){
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
    // TODO: Make this menu more elaborate.
    //  Now:  button 1 starts calibration, button 2 ends it.
    while(true){
      debouncer1.update();
      debouncer2.update();
      int button1State = 1 - debouncer1.read();
      int button2State = 1 - debouncer2.read();
      if( button2State == 1 ){
        break;  /// Exit setup if button 2 pressed
      }
      if( debouncer1.rose() ){
        /// Synchronize to serial input reference clock
        display->println(F("Syncing..."));
        display->display();
        configureClockCalibration();
      }
    }
  }
};

#endif
