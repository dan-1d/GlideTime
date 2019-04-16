# GlideTime
Software and hardware to build a timer for F3K and related radio controlled glider competitions

# Schematic and PCB
Schematic is provided in the "hardware" directory

# Design Notes
- The board has optional features and a few redundant components: there are two power switches and two power connectors to provide a few different options.
- When components arrive, I'll provide more detail in the bill of materials. Parts are relatively standard.
- I left the backlight disconnected from the LCD, but there's a resistor available if one wishes to use it. It may work with just a solder-bridge, but I wasn't sure what current the LEDs would pull.
- There are two pinouts for the Nokia 5110 LCD module. The most common one has its through-holes near the center of the board. I put the Sparkfun (and Adafruit, maybe) version at the top.

# Build Notes
- Solder on the LCD last! Otherwise, if there's an error or bad solder joint elsewhere, it's very difficult to correct if they are under the LCD.
- You may wish to only use header pins on the arduino that are used in the Schematic. This provides empty pads on the Arduino for future modification and expansion
- If you wish to power the LCD from the FTDI, you must connect GND of the FTDI header to the pad on the PCB directly under it.


