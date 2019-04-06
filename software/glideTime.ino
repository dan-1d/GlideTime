/**
*  GlideTime
*  Daniel Gebhardt, 2019
*  GPL 3.0 license. See included LICENSE file.
*/

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

typedef unsigned long ulong_t;
typedef unsigned int uint_t;


// Hardware SPI (faster, but must use certain hardware pins):
// SCK is LCD serial clock (SCLK) - this is pin 13 on Arduino Uno
// MOSI is LCD DIN - this is pin 11 on an Arduino Uno
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(10, 8, 9);


void setup() {
  Serial.begin(9600);
//  SPI.setClockDivider(SPI_CLOCK_DIV16);
  display.begin();
  // put your setup code here, to run once:
  display.setContrast(60);
  display.clearDisplay();
//  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("GlideTime\n  v.0.0.1");
  display.display();
  delay(2000);


  pinMode(2,INPUT_PULLUP);
  pinMode(3,INPUT_PULLUP);
  
}

ulong_t screen_refresh_period = 100;
ulong_t screen_refresh_last_time = 0;
uint_t button_debounce_time = 20;  //hold time of rising edge. 'high' required for this long before triggering

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
ulong_t time_intervals_A[100]; // max of 100 entries
ulong_t time_intervals_B[100]; // max of 100 entries
ulong_t time_intervals_A_length = 0; // how many actual time entries have been recorded
ulong_t time_intervals_B_length = 0;
ulong_t time_last_b_press = 0;
ulong_t round_time_start = 0;

// intervalA is basically "flight time", interval_B is dropped time
bool intervalA_started = false; // otherwise, interval_B has started

ulong_t t_now;

/////////////////////////////////////////////////////


String millis_to_minutes_seconds_str(ulong_t _ms){
  ulong_t secs = _ms / 1000;
  ulong_t minutes = secs / 60;
  ulong_t sec_remainder = secs % 60;
  ulong_t deciseconds_remainder = (_ms - secs*1000 ) / 100;
  String mm_ss_dot_ds = String("")+minutes+":"+sec_remainder+"."+deciseconds_remainder;
/*
  if( centiseconds_remainder % 10 == 0 ){
    mm_ss_dot_ms += "0";  // add trailing decimal "0" in 100ths position for display continuity
  }
*/
  return mm_ss_dot_ds;
}

void display_last_N(int n){
//  display.clearDisplay();
//  display.setCursor(40, 0);
  uint_t line_num = 0;
  for(int i=time_intervals_A_length; i > (int)time_intervals_A_length - n; i-- ){ // cast to integer, since compiler interpreted this subtraction as unsigned.. which is problematic
    if( i<=0 ){
      break;
    }
    display.setCursor(50, line_num*8);
    line_num++;
    ulong_t t_a = time_intervals_A[i-1];
    display.print(millis_to_minutes_seconds_str(t_a));
  }
}

void display_current_interval(ulong_t time_ms){
  display.setCursor(0,0);
//  display.setTextSize(2);  //2 seems to big when using the right column for times
  display.println(millis_to_minutes_seconds_str(time_ms));
//  display.setTextSize(1);
}

void display_round_time(){
  display.setCursor(0, 40);
  if( round_time_start ==0 ){
    display.print("Ready psh Btn"); 
  }else{
    String elapsed_str = String("Round: ") + millis_to_minutes_seconds_str(t_now - round_time_start);
    display.print( elapsed_str );
  }
}


///
///   Main Loop
///

void loop() {

  /// Read new state of inputs: time and buttons
  t_now = millis();
  int b_time = 1-digitalRead(2);
  int b_aux = 1-digitalRead(3);

  /// Handle button presses
  ///  update global state variables accordingly
  ///

  /// Button "Flight Time"
  bool state_transition_b_time = false;
  if( b_time != b_time_last ){ // detect edge and start counter
      b_time_debounce_start = t_now;
      b_time_last = b_time;
  }
  //check if timer reaches threshold and that this is a pending state transition
  if( b_time != b_time_previous_button_state && 
      t_now - b_time_debounce_start > button_debounce_time )
  {
    b_time_previous_button_state = b_time;
    state_transition_b_time = true;
  }

  /// Button "Aux"
  bool state_transition_b_aux = false;
  if( b_aux != b_aux_last ){ // detect edge and start counter
      b_aux_debounce_start = t_now;
      b_aux_last = b_aux;
  }
  //check if timer reaches threshold and that this is a pending state transition
  if( b_aux != b_aux_previous_button_state && 
      t_now - b_aux_debounce_start > button_debounce_time )
  {
    b_aux_previous_button_state = b_aux;
    state_transition_b_aux = true;
  }

  ///
  /// Handle state transitions
  ///

  ///  start round timer if no flight time has starated,
  if( state_transition_b_aux == true &&
    !intervalA_started && 
    round_time_start == 0 )
  {
    round_time_start = t_now;
  }

  /// Flight timer pressed
  if( state_transition_b_time==true && b_time == 1 )   //rising edge
  {
    // interval start or stop
    if( intervalA_started == false ){
      // Flight started
      intervalA_started = true;
      if( time_intervals_A_length > 0 ){
        // first interval needs to be A
        float time_elapsed = t_now - time_last_b_press;
        time_intervals_B[time_intervals_B_length] = time_elapsed;
        // TODO: check bounds of time_intervals_B_length
        time_intervals_B_length++;
      }
      /// start round timer if it hasn't already been started via AUX button
      if( round_time_start == 0 ){
        round_time_start = t_now;
      }
      /// Record the time of the last timer button press
      time_last_b_press = t_now;
    }else{
      // Flight ended
      intervalA_started = false;
      float time_elapsed = t_now - time_last_b_press;
      time_intervals_A[time_intervals_A_length] = time_elapsed;
      time_intervals_A_length++;
      // TODO: check bounds of time_intervals_A_length
      
      /// Record the time of the last timer button press
      time_last_b_press = t_now;
    }
  }

  if( t_now - screen_refresh_last_time > screen_refresh_period ){
    display.clearDisplay();
    display_last_N(5); //display last 5 times
    display_round_time();
    if( intervalA_started == true ){
      display_current_interval(t_now - time_last_b_press);    
    }
    screen_refresh_last_time = t_now;
    display.display();
  }
  

}
