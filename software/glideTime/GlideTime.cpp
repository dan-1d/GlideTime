#include <Arduino.h>
#include "GlideTime.h"


const int button_pins_array[] = {BUTTON_1_PIN, BUTTON_2_PIN, BUTTON_3_PIN, BUTTON_4_PIN, BUTTON_5_PIN};



///////////////////////////
///  GLIDETIME main
////////////////////////////

void GlideTimeMain::display_update(){
  gtInit.display->clearDisplay();
  display_last_N(FLIGHT_HISTORY_DISPLAY_LENGTH, state.intervalA_i_disp_start); //display last 5 times
  display_round_time();
  if ( state.intervalA_started == true ) {
    display_current_interval(state.t_now - state.time_last_b_press);
    display_decisecond_graphic(state.t_now - state.time_last_b_press);
  }
  state.screen_refresh_last_time = state.t_now;
  gtInit.display->display();
}


void GlideTimeMain::handle_time(){
  /// Read new state of inputs: time and buttons
  state.t_now = millis() + state.time_error_accum;
  // Correct for time skew.
  // add one to error correction every "clock_correction_increment_period" millis
  ulong_t clock_correction_increment_period = gtInit.get_config().clock_correction_increment_period();
  if ( clock_correction_increment_period > 0 &&
    state.t_now - state.time_last_correction > clock_correction_increment_period )
  {
    state.time_error_accum +=  gtInit.get_config().clock_correction_direction();
    state.time_last_correction = state.t_now;
  }
}

void GlideTimeMain::flight_start_stop(){
  if ( state.intervalA_started == false ) {
    // Flight started
    state.intervalA_started = true;
    if ( state.time_intervals_A_length > 0 ) {
      // first interval needs to be A
      float time_elapsed = state.t_now - state.time_last_b_press;
      state.time_intervals_B[state.time_intervals_B_length] = time_elapsed;
      // TODO: check bounds of time_intervals_B_length
      state.time_intervals_B_length++;
    }
    /// start round timer if it hasn't already been started via AUX button
    if ( state.round_time_start == 0 ) {
      state.round_time_start = state.t_now;
    }
    /// Record the time of the last timer button press
    state.time_last_b_press = state.t_now;
  } else {
    // Flight ended
    state.intervalA_started = false;
    float time_elapsed = state.t_now - state.time_last_b_press;
    state.time_intervals_A[state.time_intervals_A_length] = time_elapsed;
    if ( state.intervalA_i_disp_start == (int)state.time_intervals_A_length ) {
      state.intervalA_i_disp_start = (int)state.time_intervals_A_length + 1;
    }
    state.time_intervals_A_length++;
    // TODO: check bounds of time_intervals_A_length

    /// Record the time of the last timer button press
    state.time_last_b_press = state.t_now;
  }
}

void GlideTimeMain::round_start(){
  state.round_time_start = state.t_now;
}


String GlideTimeMain::millis_to_minutes( ulong_t _ms ) {
  ulong_t secs = _ms / 1000;
  ulong_t minutes = secs / 60;
  String mm = String("") + minutes;
  return mm;
}

String GlideTimeMain::millis_to_seconds_remainder( ulong_t _ms ) {
  ulong_t secs = _ms / 1000;
  ulong_t sec_remainder = secs % 60;
  return String("") + sec_remainder;
}

String GlideTimeMain::millis_to_minutes_seconds_str(ulong_t _ms) {
  ulong_t secs = _ms / 1000;
  ulong_t minutes = secs / 60;
  ulong_t sec_remainder = secs % 60;
  //  ulong_t deciseconds_remainder = (_ms - secs * 1000 ) / 100;
  String mm_ss = String("") + minutes + ":" + sec_remainder;
  return mm_ss;
}

String GlideTimeMain::millis_to_minutes_seconds_deciseconds_str(ulong_t _ms) {
  ulong_t secs = _ms / 1000;
  ulong_t minutes = secs / 60;
  ulong_t sec_remainder = secs % 60;
  ulong_t deciseconds_remainder = (_ms - secs * 1000 ) / 100;
  String mm_ss_dot_ds = String("") + minutes + ":" + sec_remainder + "." + deciseconds_remainder;
  return mm_ss_dot_ds;
}

void GlideTimeMain::display_last_N(int n_last, int i_start) {
  uint_t line_num = 0;
  int j_intervals = i_start;
  if ( j_intervals == 0 ) {
    j_intervals = (int)state.time_intervals_A_length;
  }
  for (int i = j_intervals; i > (int)j_intervals - n_last; i-- ) {
    if ( i <= 0 ) {
      break;
    }
    ulong_t t_a = state.time_intervals_A[i - 1];

    int curX = i < 10 ? 40 : 35;
    gtInit.display->setCursor(curX, line_num * 8);
    gtInit.display->print(i);
    gtInit.display->drawFastVLine(46, 0, 38, 0xFFFF);
    gtInit.display->setCursor(48, line_num * 8);
    gtInit.display->print(millis_to_minutes_seconds_deciseconds_str(t_a).c_str());
    /// The comment block below uses nice fonts, but seems to tax the CPU/memory and button presses are commonly missed (!!)
    /*
        int curX = i < 10 ? 49 : 46;
      //    dispFonts.setFont(u8g2_font_helvR08_tf);
      //    dispFonts.setFont(u8g2_font_timR08_tf);
        dispFonts.setFont(u8g2_font_haxrcorp4089_tr);
        dispFonts.setCursor(curX, (1+line_num)*8);
        dispFonts.print(i);
        dispFonts.setCursor(57, (1+line_num) * 8);
        dispFonts.print(millis_to_minutes_seconds_deciseconds_str(t_a));
        display.drawFastVLine(55,0,38,0xFFFF);
    */

    line_num++;
  }
}

void GlideTimeMain::display_current_interval(ulong_t time_ms) {
  //  display.setCursor(0, 0);
  //  display.setTextSize(2);
  gtInit.dispFonts.setFont(u8g2_font_helvR14_tf);  // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
  gtInit.dispFonts.setCursor(0, 14);
  gtInit.dispFonts.print(millis_to_minutes(time_ms));
  gtInit.dispFonts.setCursor(0, 28);
  gtInit.dispFonts.print(String(":") + millis_to_seconds_remainder(time_ms));
  //  dispFonts.print(millis_to_minutes_seconds_deciseconds_str(time_ms));
  //  dispFonts.print(millis_to_minutes_seconds_str(time_ms));
  //  display.println(millis_to_minutes_seconds_str(time_ms));
  //  display.setTextSize(1);
}

void GlideTimeMain::display_round_time() {
  gtInit.display->setCursor(0, 40);
  if ( state.round_time_start == 0 ) {
    gtInit.display->print("Ready psh Btn");
  } else {
    String elapsed_str = String("Round: ") + millis_to_minutes_seconds_str(state.t_now - state.round_time_start);
    gtInit.display->print( elapsed_str );
  }
}

void GlideTimeMain::display_decisecond_graphic(ulong_t time_ms) {
  ulong_t secs = time_ms / 1000;
  ulong_t sec_remainder = secs % 60;
  ulong_t deciseconds_remainder = (time_ms - secs * 1000 ) / 100;
  uint8_t line_length_max = 20;
  uint8_t current_length = deciseconds_remainder * 20 / 10;
  gtInit.display->drawFastHLine(0, 30, current_length, 0xFFFF);
  gtInit.display->drawFastVLine(line_length_max, 30, 4, 0xFFFF);
}


void GlideTimeMain::start()
{
  while(true){
    //  millisecond-based tasks, e.g. clock error correction
    handle_time();

    //
    //  user-input
    //
    buttons.update();
    if(buttons.rose(1)){
      flight_start_stop();
    }
    // button 2 does either: starts round time or cycles flight time screens
    if(buttons.rose(2) && !state.intervalA_started && state.round_time_start == 0 ) {
      round_start();
    }else if ( buttons.rose(2) && state.time_intervals_A_length > 1){
      state.intervalA_i_disp_start -= FLIGHT_HISTORY_DISPLAY_LENGTH;
      if ( state.intervalA_i_disp_start < 0 ) {
        state.intervalA_i_disp_start = (int) state.time_intervals_A_length;
      }
    }

    //
    // Update display
    //
    if ( state.t_now - state.screen_refresh_last_time > SCREEN_REFRESH_PERIOD ) {
      display_update();
    }
  }
}
