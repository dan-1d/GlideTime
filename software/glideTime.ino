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
/*
uint_t b_time_last = 0; // last value of the b_time reading
uint_t b_time_previous_button_state = 0;
uint_t b_aux_last = 0;
uint_t b_aux_previous_button_state = 0;
//button debouncing time variables
ulong_t b_time_debounce_start = 0;
ulong_t b_aux_debounce_start = 0;
*/
/*
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
*/
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


///
///   Main Loop
///

GlideTimeMain gtMain;

void loop() {

  gtMain.start();

}
