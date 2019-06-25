# GlideTime
Software and hardware to build a timer for F3K and related radio controlled glider competitions

# Schematic and PCB
Schematic is provided in the "hardware" directory. The schematic and PCB source is a shared project through EasyEDA.
https://easyeda.com/dangebhardt/f3k-timer

# Case
https://cad.onshape.com/documents/c915eae26da13888ba1db9f7/w/73070782390be25dc2b44e4e/e/b4bf00f5ed530a94afac5f37

Others are welcome to submit 3D models themselves

# Misc. Design Notes
- The board has optional features and a few redundant components: there are two power switches and two power connectors to provide a few different options.
  - The bottom slide switch is far more robust and is used for the "v1" release.
- Backlight of LCD is disconnected, but can be connected per your specific Nokia 5110. There are several versions that require connection of the backlight to GND or to VCC, and sometimes through a resistor.
- There are two pinouts for the Nokia 5110 LCD module. The most common one has its through-holes near the center of the board. I put the Sparkfun (and Adafruit, maybe) version at the top.

# Build Notes
- Solder on the LCD last! Otherwise, if there's an error or bad solder joint elsewhere, it's very difficult to correct if they are under the LCD.
- You may wish to only use header pins on the arduino that are used in the Schematic. This provides empty pads on the Arduino for future modification and expansion
- You must connect GND of the FTDI header to Ground (GND) of the rest of the board. Note, the pad directly under the header is not connected to ground in the v.1.0 PCB. You must then use a short jumper to ground that pad.


