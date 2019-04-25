//
// TODO: battery voltage seems to be reading low
//

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <U8g2_for_Adafruit_GFX.h>

#include "GlideTime.h"



// Hardware SPI (faster, but must use certain hardware pins):
// SCK is LCD serial clock (SCLK) - this is pin 13 on Arduino Uno
// MOSI is LCD DIN - this is pin 11 on an Arduino Uno
// pin 5 - Data/Command select (D/C)qqqq``
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
/// FIRST PROTOTYPE - Deprecated
//Adafruit_PCD8544 display = Adafruit_PCD8544(10, 8, 9);
// FIRST PCB V.1.0 configuration of pins
Adafruit_PCD8544 display = Adafruit_PCD8544(9, 8, 7);
U8G2_FOR_ADAFRUIT_GFX dispFonts; //u8g2_for_adafruit_gfx;

GlideTimeInit gtInit = GlideTimeInit(display);


///
/// State variables
///
// button state and debouncing
uint_t b_time_last = 0; // last value of the b_time reading
uint_t b_time_previous_button_state = 0;
uint_t b_aux_last = 0;
uint_t b_aux_previous_button_state = 0;
//button debouncing time variables
ulong_t b_time_debounce_start = 0;
ulong_t b_aux_debounce_start = 0;
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

/////////////////////////////////////////////////////


void setup() {
  Serial.begin(9600);
  //SPI.setClockDivider(SPI_CLOCK_DIV16);
  SPI.setClockDivider(SPI_CLOCK_DIV2);  // Div2 for 3.3v 8Mhz arduino part and Nokia 5110 LCD
  int vbatt_adc = analogRead(VBATT_PIN);
  float vbatt = vbatt_adc * 3.3 * 2.0 / 1024.0;
  display.begin();
  dispFonts.begin(display);
  // put your setup code here, to run once:
  display.setContrast(LCD_CONTRAST);
  display.clearDisplay();
  //  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(String("GlideTime\n") + VERSION);
  display.println("battery V=");
  display.println(vbatt);
  display.println("clk corr per");
  display.println(gtInit.get_config().clock_correction_increment_period());
  display.display();
  delay(2000);

  Serial.print("test before initialize");
  gtInit.initialize();
}



/////////////////////////////////////////////////////

String millis_to_minutes( ulong_t _ms ) {
  ulong_t secs = _ms / 1000;
  ulong_t minutes = secs / 60;
  String mm = String("") + minutes;
  return mm;
}

String millis_to_seconds_remainder( ulong_t _ms ) {
  ulong_t secs = _ms / 1000;
  ulong_t sec_remainder = secs % 60;
  return String("") + sec_remainder;
}

String millis_to_minutes_seconds_str(ulong_t _ms) {
  ulong_t secs = _ms / 1000;
  ulong_t minutes = secs / 60;
  ulong_t sec_remainder = secs % 60;
  //  ulong_t deciseconds_remainder = (_ms - secs * 1000 ) / 100;
  String mm_ss = String("") + minutes + ":" + sec_remainder;
  return mm_ss;
}

String millis_to_minutes_seconds_deciseconds_str(ulong_t _ms) {
  ulong_t secs = _ms / 1000;
  ulong_t minutes = secs / 60;
  ulong_t sec_remainder = secs % 60;
  ulong_t deciseconds_remainder = (_ms - secs * 1000 ) / 100;
  String mm_ss_dot_ds = String("") + minutes + ":" + sec_remainder + "." + deciseconds_remainder;
  return mm_ss_dot_ds;
}

void display_last_N(int n_last, int i_start) {
  uint_t line_num = 0;
  int j_intervals = i_start;
  if ( j_intervals == 0 ) {
    j_intervals = (int)time_intervals_A_length;
  }
  for (int i = j_intervals; i > (int)j_intervals - n_last; i-- ) {
    if ( i <= 0 ) {
      break;
    }
    ulong_t t_a = time_intervals_A[i - 1];

    int curX = i < 10 ? 40 : 35;
    display.setCursor(curX, line_num * 8);
    display.print(i);
    display.drawFastVLine(46, 0, 38, 0xFFFF);
    display.setCursor(48, line_num * 8);
    display.print(millis_to_minutes_seconds_deciseconds_str(t_a).c_str());
    //    display.print(millis_to_minutes_seconds_str(t_a));


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

void display_current_interval(ulong_t time_ms) {
  //  display.setCursor(0, 0);
  //  display.setTextSize(2);
  dispFonts.setFont(u8g2_font_helvR14_tf);  // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
  dispFonts.setCursor(0, 14);
  dispFonts.print(millis_to_minutes(time_ms));
  dispFonts.setCursor(0, 28);
  dispFonts.print(String(":") + millis_to_seconds_remainder(time_ms));
  //  dispFonts.print(millis_to_minutes_seconds_deciseconds_str(time_ms));
  //  dispFonts.print(millis_to_minutes_seconds_str(time_ms));
  //  display.println(millis_to_minutes_seconds_str(time_ms));
  //  display.setTextSize(1);
}

void display_round_time() {
  display.setCursor(0, 40);
  if ( round_time_start == 0 ) {
    display.print("Ready psh Btn");
  } else {
    String elapsed_str = String("Round: ") + millis_to_minutes_seconds_str(t_now - round_time_start);
    display.print( elapsed_str );
  }
}

void display_decisecond_graphic(ulong_t time_ms) {
  ulong_t secs = time_ms / 1000;
  ulong_t sec_remainder = secs % 60;
  ulong_t deciseconds_remainder = (time_ms - secs * 1000 ) / 100;
  uint8_t line_length_max = 20;
  uint8_t current_length = deciseconds_remainder * 20 / 10;
  display.drawFastHLine(0, 30, current_length, 0xFFFF);
  display.drawFastVLine(line_length_max, 30, 4, 0xFFFF);
}


///
///   Main Loop
///


// The accuracy of the millis timer seems to be off.
// For a calibration run, calculate the % off
// Use that to determine a ratio of how many ms until an error of 1 ms occurs
// This value is used within the loop to correct "t_now"
ulong_t time_last_correction = 0;
ulong_t time_error_accum = 0;

void loop() {

  /// Read new state of inputs: time and buttons
  t_now = millis() + time_error_accum;

  // Correct for time skew.
  // add one to error correction every 300 millis
  if (t_now - time_last_correction > gtInit.get_config().clock_correction_increment_period() ) {
    time_error_accum +=  gtInit.get_config().clock_correction_direction();
    time_last_correction = t_now;
  }

  int b_time = 1 - digitalRead(BUTTON_1_PIN);
  int b_aux = 1 - digitalRead(BUTTON_2_PIN);

  /// Handle button presses
  ///  update global state variables accordingly
  ///

  /// Button "Flight Time"
  bool state_transition_b_time = false;
  if ( b_time != b_time_last ) { // detect edge and start counter
    b_time_debounce_start = t_now;
    b_time_last = b_time;
  }
  //check if timer reaches threshold and that this is a pending state transition
  if ( b_time != b_time_previous_button_state &&
       t_now - b_time_debounce_start > button_debounce_time )
  {
    b_time_previous_button_state = b_time;
    state_transition_b_time = true;
  }

  /// Button "Aux"
  bool state_transition_b_aux = false;
  if ( b_aux != b_aux_last ) { // detect edge and start counter
    b_aux_debounce_start = t_now;
    b_aux_last = b_aux;
  }
  //check if timer reaches threshold and that this is a pending state transition
  if ( b_aux != b_aux_previous_button_state &&
       t_now - b_aux_debounce_start > button_debounce_time )
  {
    b_aux_previous_button_state = b_aux;
    state_transition_b_aux = true;
  }

  ///
  /// Handle state transitions
  ///

  /// Aux button pressed
  ///  start round timer if no flight time has starated,
  if ( state_transition_b_aux == true &&
       !intervalA_started &&
       round_time_start == 0 )
  {
    round_time_start = t_now;
  }
  ///  display other flight times in history by setting intervalA_i_disp_start to another starting index
  else if ( state_transition_b_aux == true && b_aux == 1 && time_intervals_A_length > 1)
  {
    intervalA_i_disp_start -= FLIGHT_HISTORY_DISPLAY_LENGTH;
    if ( intervalA_i_disp_start < 0 ) {
      intervalA_i_disp_start = (int)time_intervals_A_length;
    }
  }

  /// Flight timer pressed
  if ( state_transition_b_time == true && b_time == 1 ) //rising edge
  {
    // interval start or stop
    if ( intervalA_started == false ) {
      // Flight started
      intervalA_started = true;
      if ( time_intervals_A_length > 0 ) {
        // first interval needs to be A
        float time_elapsed = t_now - time_last_b_press;
        time_intervals_B[time_intervals_B_length] = time_elapsed;
        // TODO: check bounds of time_intervals_B_length
        time_intervals_B_length++;
      }
      /// start round timer if it hasn't already been started via AUX button
      if ( round_time_start == 0 ) {
        round_time_start = t_now;
      }
      /// Record the time of the last timer button press
      time_last_b_press = t_now;
    } else {
      // Flight ended
      intervalA_started = false;
      float time_elapsed = t_now - time_last_b_press;
      time_intervals_A[time_intervals_A_length] = time_elapsed;
      if ( intervalA_i_disp_start == (int)time_intervals_A_length ) {
        intervalA_i_disp_start = (int)time_intervals_A_length + 1;
      }
      time_intervals_A_length++;
      // TODO: check bounds of time_intervals_A_length

      /// Record the time of the last timer button press
      time_last_b_press = t_now;
    }
  }

  if ( t_now - screen_refresh_last_time > screen_refresh_period ) {
    display.clearDisplay();
    display_last_N(FLIGHT_HISTORY_DISPLAY_LENGTH, intervalA_i_disp_start); //display last 5 times
    display_round_time();
    if ( intervalA_started == true ) {
      display_current_interval(t_now - time_last_b_press);
      display_decisecond_graphic(t_now - time_last_b_press);
    }
    screen_refresh_last_time = t_now;
    display.display();
  }


}
