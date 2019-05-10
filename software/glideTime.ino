//
// TODO: battery voltage seems to be reading low
//

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include "GlideTime.h"
#include "consts.h"

// Hardware SPI (faster, but must use certain hardware pins):
// SCK is LCD serial clock (SCLK) - this is pin 13 on Arduino Uno
// MOSI is LCD DIN - this is pin 11 on an Arduino Uno
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
/// FIRST PROTOTYPE - Deprecated
//Adafruit_PCD8544 display = Adafruit_PCD8544(10, 8, 9);
// FIRST PCB V.1.0 configuration of pins

Adafruit_PCD8544 display = Adafruit_PCD8544(9, 8, 7);
GlideTimeInit gtInit = GlideTimeInit(display);
GlideTimeMain gtMain = GlideTimeMain(gtInit);

/////////////////////////////////////////////////////


void setup() {
//  Serial.begin(9600);
  //SPI.setClockDivider(SPI_CLOCK_DIV16);
  SPI.setClockDivider(SPI_CLOCK_DIV2);  // Div2 for 3.3v 8Mhz arduino part and Nokia 5110 LCD
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
