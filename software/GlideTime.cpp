#include <Arduino.h>
#include "GlideTime.h"


const int button_pins_array[] = {BUTTON_1_PIN, BUTTON_2_PIN, BUTTON_3_PIN, BUTTON_4_PIN, BUTTON_5_PIN};

GlideTimeConfig::set_clock_cal(){
  // calculate the number of millis to wait between adjustments.
  // ensure the millis value is positive, and save the direction to  clock_cal_direction
  ulong_t ms_diff_per_minute;
  EEPROM.get(EEPROM_ADDR_CALIBRATION_DATA, ms_diff_per_minute);
  if( ms_diff_per_minute != 0 ){
    ms_diff_per_minute = ms_diff_per_minute < 0 ? -ms_diff_per_minute : ms_diff_per_minute;
    clock_cal_millis_per_correction = 60000 / ms_diff_per_minute;
    clock_cal_direction = ms_diff_per_minute > 0 ? 1 : 0;
  }

  Serial.begin(9600);
  Serial.print("ms_diff_per_minute=");
  Serial.println(ms_diff_per_minute);
  Serial.print("clock_cal_millis_per_correction=");
  Serial.println(clock_cal_millis_per_correction);
}
