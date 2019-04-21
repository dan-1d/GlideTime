# GlideTime
Software and hardware to build a timer for F3K and related radio controlled glider competitions

# Schematic and PCB
Schematic is provided in the "hardware" directory. The schematic and PCB source is a shared project through EasyEDA.
https://easyeda.com/dangebhardt/f3k-timer

# Case
Preliminary case designs:
https://cad.onshape.com/documents/e6fbfba96731aa58d394e063/w/f54ae2129e8294d59266f2e1/e/663b1d4d4e45ecbccb0a925a

Others are welcome to submit 3D models themselves

# Design Notes
- The board has optional features and a few redundant components: there are two power switches and two power connectors to provide a few different options.
  - The bottom slide switch is far more robust, and future iterations of this design will probably use that.
- Backlight of LCD is disconnected, but can be connected per your specific Nokia 5110. There are several versions that require connection of the backlight to GND or to VCC, and sometimes through a resistor.
- There are two pinouts for the Nokia 5110 LCD module. The most common one has its through-holes near the center of the board. I put the Sparkfun (and Adafruit, maybe) version at the top.

# Build Notes
- Solder on the LCD last! Otherwise, if there's an error or bad solder joint elsewhere, it's very difficult to correct if they are under the LCD.
- You may wish to only use header pins on the arduino that are used in the Schematic. This provides empty pads on the Arduino for future modification and expansion
- You must connect GND of the FTDI header to Ground (GND) of the rest of the board. Note, the pad directly under the header is not connected in the v.1.0 PCB.


