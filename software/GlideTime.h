
#ifndef GLIDETIME_H_
#define GLIDETIME_H_

#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <Bounce2.h>
#include <EEPROM.h>
#include "GlideTimeConfig.h"
#include "consts.h"


extern const int button_pins_array[];

// Need forward declaration of GlideTimeConfig.
//class GlideTimeConfig;



class GlideTimeInit {
private:
  GlideTimeConfig gtConfig;
  GlideTimeMain* gtMain;
  setInputPins(){
    // relies on #defines in somewhat poor form
    pinMode(BUTTON_1_PIN, INPUT_PULLUP);
    pinMode(BUTTON_2_PIN, INPUT_PULLUP);
    pinMode(BUTTON_3_PIN, INPUT_PULLUP);
    pinMode(BUTTON_4_PIN, INPUT_PULLUP);
    pinMode(BUTTON_5_PIN, INPUT_PULLUP);
  }

public:
  Adafruit_PCD8544* display;
  U8G2_FOR_ADAFRUIT_GFX dispFonts; //u8g2_for_adafruit_gfx;

  /// Initialize with the display object
  GlideTimeInit(Adafruit_PCD8544& _display){
    display = &_display;
    gtConfig.display = &_display;
    dispFonts.begin(*display);
  }

  /// Call Init once from "setup" routine of Arduino
  /// Setup device pins appropriately as per input and output
  /// Restore permanent state of configuration options from EEPROM:
  ///  - Timer calibration
  ///  - LCD contrast
  void initialize(GlideTimeMain* _gtMain){
    gtMain = _gtMain;
    setInputPins();
    gtConfig.set_GlideTimeMainObj(gtMain);
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



class GlideTimeState
{
public:
  // timer intervals and history
  ulong_t time_intervals_A[FLIGHT_HISTORY_MAX]; // max of 50 entries
  ulong_t time_intervals_B[FLIGHT_HISTORY_MAX]; // max of 50 entries
  ulong_t time_intervals_A_length = 0; // how many actual time entries have been recorded
  ulong_t time_intervals_B_length = 0;
  ulong_t time_last_b_press = 0;
  ulong_t round_time_start = 0;
  // intervalA is basically "flight time", interval_B is dropped time
  bool intervalA_started = false; // otherwise, interval_B has started
  int intervalA_i_disp_start = 0;
  ulong_t t_now;
  ulong_t screen_refresh_last_time;
  // The accuracy of the millis timer is not accurate and needs calibration.
  // For a calibration run, calculate the % off
  // Use that to determine a ratio of how many ms until an error of 1 ms occurs
  // This value is used within the loop to correct "t_now"
  ulong_t time_last_correction = 0;
  ulong_t time_error_accum = 0;

  void reset(){
/*
    for( int i=0; i<FLIGHT_HISTORY_MAX; i++ ){
      time_intervals_A[i] = 0;
      time_intervals_B[i] = 0;
    }
*/
    time_intervals_A_length = 0;
    time_intervals_B_length = 0;
    time_last_b_press = 0;
    round_time_start = 0;
    intervalA_started = false;
    intervalA_i_disp_start = 0;
    time_last_correction = 0;
    time_error_accum = 0;
  }
};





class GlideTimeButtons
{
  Bounce buttons[NUMBER_OF_BUTTONS];
public:
  GlideTimeButtons(){
    for(int i=0; i<NUMBER_OF_BUTTONS; i++){
      buttons[i].attach(button_pins_array[i]);
      buttons[i].interval(BUTTON_DEBOUNCE_TIME);
    }
  }

  void update(){
    for(int i=0; i<NUMBER_OF_BUTTONS; i++){
      buttons[i].update();
    }
  }

  bool rose(int button_i){
    int i = button_i;
    return buttons[i-1].fell();
  }

  bool read(int button_i){
    int i = button_i;
    return 1 - buttons[i-1].read();
  }


};



class GlideTimeMain
{
public:  // no good reason to keep these private, as they are needed outside. A refactoring is needed.
  GlideTimeState state;
  GlideTimeButtons buttons;
  GlideTimeInit& gtInit;
  String millis_to_minutes( ulong_t _ms );
  String millis_to_seconds_remainder( ulong_t _ms );
  String millis_to_minutes_seconds_str(ulong_t _ms);
  String millis_to_minutes_seconds_deciseconds_str(ulong_t _ms);
  void display_last_N(int n_last, int i_start);
  void display_current_interval(ulong_t time_ms);
  void GlideTimeMain::display_round_time();
  void display_decisecond_graphic(ulong_t time_ms);
  void handle_time();
  void flight_start_stop();
  void round_start();
  void display_update();

  GlideTimeMain(GlideTimeInit& _gtInit) : gtInit(_gtInit)
  {

  }


  void start();

};

#endif
