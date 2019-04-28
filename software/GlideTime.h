
#ifndef GLIDETIMECONF_H_
#define GLIDETIMECONF_H_

#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <Bounce2.h>
#include <EEPROM.h>



//Global Defines
typedef unsigned long ulong_t;
typedef unsigned int uint_t;

#define VERSION "v.1.0.2"
#define FLIGHT_HISTORY_MAX  100   // limit of statically-allocated flight time history (intervalA)
#define FLIGHT_HISTORY_DISPLAY_LENGTH  5  // number of previous flights to display at a time. Limited by LCD size and UI choices
#define LCD_CONTRAST 60 // Specific value for each particular Nokie 5110 LCD vendor. It varies depending on supplier.
#define BUTTON_1_PIN 2
#define BUTTON_2_PIN 3
#define BUTTON_3_PIN 4
#define BUTTON_4_PIN 5
#define BUTTON_5_PIN 6
#define NUMBER_OF_BUTTONS 5
#define VBATT_PIN A0
#define BUTTON_DEBOUNCE_TIME  10  //hold time of rising edge. 'high' required for this long before triggering
#define SCREEN_REFRESH_PERIOD 100
#define EEPROM_ADDR_CALIBRATION_DATA 0x10

extern const int button_pins_array[];



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
    debouncer1.interval(BUTTON_DEBOUNCE_TIME);
    debouncer2.interval(BUTTON_DEBOUNCE_TIME);
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
  GlideTimeConfig gtConfig;
public:
  Adafruit_PCD8544* display;
  U8G2_FOR_ADAFRUIT_GFX dispFonts; //u8g2_for_adafruit_gfx;
  /// Initialize with the display object
  GlideTimeInit(Adafruit_PCD8544& _display){
    display = &_display;
    gtConfig.display = &_display;
    dispFonts.begin(*display);
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



class GlideTimeState
{
public:
  // timer intervals and history
  ulong_t time_intervals_A[50]; // max of 50 entries
  ulong_t time_intervals_B[50]; // max of 50 entries
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
private:
  GlideTimeState state;
  GlideTimeButtons buttons;
  GlideTimeInit& gtInit;
  void handle_time();
  void flight_start_stop();
  void round_start();
  void display_update();
  String millis_to_minutes( ulong_t _ms );
  String millis_to_seconds_remainder( ulong_t _ms );
  String millis_to_minutes_seconds_str(ulong_t _ms);
  String millis_to_minutes_seconds_deciseconds_str(ulong_t _ms);
  void display_last_N(int n_last, int i_start);
  void display_current_interval(ulong_t time_ms);
  void GlideTimeMain::display_round_time();
  void display_decisecond_graphic(ulong_t time_ms);



public:
  GlideTimeMain(GlideTimeInit& _gtInit) : gtInit(_gtInit)
  {
  }

  void start(){
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
};

#endif
