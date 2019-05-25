#include "GlideTime.h"
#include "GlideTimeConfig.h"



GlideTimeConfig::set_clock_cal(){
  // calculate the number of millis to wait between adjustments.
  // ensure the millis value is positive, and save the direction to  clock_cal_direction
//  long ms_diff_per_minute;
//  EEPROM.get(EEPROM_ADDR_CALIBRATION_DATA, ms_diff_per_minute);
//  if( ms_diff_per_minute != 0 ){
//    ms_diff_per_minute = ms_diff_per_minute < 0 ? -ms_diff_per_minute : ms_diff_per_minute;
//    clock_cal_millis_per_correction = 60000 / ms_diff_per_minute;
//    clock_cal_direction = ms_diff_per_minute > 0 ? 1 : -1;  // increment or decrement depending on if clock is fast or slow
    clock_cal_millis_per_correction = DEFAULT_CLOCK_CALIBRATION_MS_PER_CORRECTION;
    clock_cal_direction = DEFAULT_CLOCK_CALIBRATION_DIRECTION > 0 ? 1 : -1;  // increment or decrement depending on if clock is fast or slow
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
  // Issue start command to host calibration system
  // Wait for ack
  // Mark start time
  // Wait for the calibration time
  // Mark end time
  
  gtMain->state.reset();
  clock_cal_millis_per_correction = 0;
  clock_cal_direction = 0;
  Serial.begin(115200);
  // Read serial for 60 1-second intervals
  Serial.write("start\n");
  Serial.flush();
  byte ack[1];
  Serial.readBytes(ack, 1);
  ulong_t begin_ms = millis();

  // Perform calibration while displaying the same data on the screen as
  // would run in the real system. This helps reduce timing error that
  // stems from the display routines updating.
  //
  gtMain->handle_time();
  gtMain->flight_start_stop();
  
//  uint_t cal_secs = 0;
  bool calDone = false;
  while( !calDone ){
//  while(cal_secs < CALIBRATION_DURATION_SECONDS){
    if(Serial.available()){
      Serial.read();
      calDone = true;
//      cal_secs++;
    }
    /*
    // First iteration, log the start time
    // If done prior to receiving first tick, may be too far off (latency of send+receive)
    if( cal_secs == 1 ){
      begin_ms = millis();
    }
    */
    // Call display routines here;
    gtMain->handle_time();
    if ( gtMain->state.t_now - gtMain->state.screen_refresh_last_time > SCREEN_REFRESH_PERIOD ) {
      gtMain->display_update();
    }
  }

  ulong_t end_ms = millis();
  // Calculate ms error and save to eeprom
  long ms_diff_per_minute = ((long)CALIBRATION_DURATION_SECONDS*1000 - (long)(end_ms-begin_ms)) / (CALIBRATION_DURATION_SECONDS/60);
  EEPROM.put(EEPROM_ADDR_CALIBRATION_DATA, ms_diff_per_minute);
  display->clearDisplay();
  display->setCursor(0,10);
  display->print("start:");
  display->println(begin_ms);
  display->print("end:");
  display->println(end_ms);
  display->print("diff: ");
  display->println(end_ms-begin_ms);
  display->print("ms/min: ");
  display->println(ms_diff_per_minute);
  display->display();
  set_clock_cal();
}
