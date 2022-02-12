//
//

#include <SPI.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_PCD8544.h>
#include <U8g2lib.h>
#include "GlideTime.h"
#include "consts.h"


U8G2_HX1230_96X68_F_3W_SW_SPI display(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 8, /* reset=*/  7 );

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

  gtInit.initialize(&gtMain);
}


///
///   Main Loop
///


void loop() {

  gtMain.start();

}
