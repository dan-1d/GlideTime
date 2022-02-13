//  Refactor notes for display HX1230
// -- This display has higher resolution than Nokia 5110
// -- uses software SPI for data == slow. May miss button presses (need to test to confirm)
// -- Library u8g2, if configured in full frame buffer mode, uses almost all available RAM.
// -- 
//

#include <SPI.h>
#include <U8g2lib.h>
#include "GlideTime.h"
#include "consts.h"


U8G2_HX1230_96X68_F_3W_SW_SPI display(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 8, /* reset=*/  7 );
//U8G2_HX1230_96X68_1_3W_SW_SPI display(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 8, /* reset=*/  7 );

GlideTimeInit gtInit = GlideTimeInit(display);
GlideTimeMain gtMain = GlideTimeMain(gtInit);

/////////////////////////////////////////////////////


void setup() {
//  Serial.begin(9600);
  //SPI.setClockDivider(SPI_CLOCK_DIV16);
  // ********************** TODO:  CONFIRM CLOCK_DIV NOT NEEDED *************
  //SPI.setClockDivider(SPI_CLOCK_DIV2);  // Div2 for 3.3v 8Mhz arduino part and Nokia 5110 LCD
  int vbatt_adc = analogRead(VBATT_PIN);
  float vbatt = vbatt_adc * 3.3 * 2.0 / 1024.0  + BATT_V_ADC_CORRECTION;
  display.begin();
//  dispFonts.begin(display);
  // put your setup code here, to run once:
  display.setContrast(LCD_CONTRAST);
  display.clearBuffer();
  //  display.setTextSize(1);
//  display.setCursor(0, 0);
  display.setCursor(30, 30);
  display.setFont(u8g2_font_helvR10_tf);
  display.print(String(F("GlideTime\n")) + VERSION + F("\n"));
  display.print(F("battery V=\n"));
  display.print(vbatt);
  display.print(F("\nclk corr per\n"));
  display.print(gtInit.get_config().clock_correction_increment_period());
//  display.display();
  display.sendBuffer();
  delay(2000);

  gtInit.initialize(&gtMain);
}


///
///   Main Loop
///


void loop() {

  gtMain.start();

}
