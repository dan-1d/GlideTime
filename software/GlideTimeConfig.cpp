#include "GlideTime.h"
#include "GlideTimeConfig.h"



GlideTimeConfig::set_clock_cal(){
  // calculate the number of millis to wait between adjustments.
  // ensure the millis value is positive, and save the direction to  clock_cal_direction
  ulong_t ms_diff_per_minute;
  EEPROM.get(EEPROM_ADDR_CALIBRATION_DATA, ms_diff_per_minute);
  if( ms_diff_per_minute != 0 ){
    ms_diff_per_minute = ms_diff_per_minute < 0 ? -ms_diff_per_minute : ms_diff_per_minute;
    clock_cal_millis_per_correction = 60000 / ms_diff_per_minute;
    clock_cal_direction = ms_diff_per_minute > 0 ? 1 : -1;  // increment or decrement depending on if clock is fast or slow
  }

/*
  Serial.begin(9600);
  Serial.print("ms_diff_per_minute=");
  Serial.println(ms_diff_per_minute);
  Serial.print("clock_cal_millis_per_correction=");
  Serial.println(clock_cal_millis_per_correction);
  Serial.flush();
*/
}


void GlideTimeConfig::configureClockCalibration(){
  gtMain->state.reset();
  clock_cal_millis_per_correction = 0;
  clock_cal_direction = 0;
  // wait for 1-second markers from Serial
  Serial.begin(115200);
  // Read serial for 60 1-second intervals
  Serial.write("start\n");
  ulong_t begin_ms = millis();

  // Perform calibration while displaying the same data on the screen as
  // would run in the real system. This helps reduce timing error that
  // stems from the display routines updating.
  //
  //
  uint_t cal_secs = 0;
  while(cal_secs < 61){
    if(Serial.available()){
      Serial.read();
      cal_secs++;
    }
    // Call display routines here;
    gtMain->handle_time();  //don't use any correction factor
    if( cal_secs == 0 ){
      gtMain->flight_start_stop();
    }
    if ( gtMain->state.t_now - gtMain->state.screen_refresh_last_time > SCREEN_REFRESH_PERIOD ) {
      gtMain->display_update();
    }
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
