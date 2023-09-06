# Setup

1. [Setup Instructions](#setup-instructions)
   1. [Solder wires to ribbon cable](#solder-wires-to-ribbon-cable)
   2. [Connect the sign to an Arduino](#connect-the-sign-to-an-arduino)
      1. [Breadboard](#breadboard)
      2. [Other setup images](#other-setup-images)
   3. [Program Arduino with code](#program-arduino-with-code)

## Setup Instructions

In brief:

- Take apart the unit and remove the back-of-unit logic board.
- Attach power supply to AC to DC converter
- Solder wires onto back of DMX PCB
- Attach 3 of the ribbon cable wires (DIN, CLK, LOAD) to Arduino (11, 13, 5, respectively)
- Attach 1 of the ribbon cable wires (ISET) to power supply (with optional resistor to limit current - see [datasheet][AS1100] section 8.6)
- Ensure everything has a common ground (Arduino, power supply, DMX PCB, power converter)
- Download Arduino IDE and install required libraries (Adafruit_GFX and [Code](./Code/) [manual install]).
- Copy [example code](./Code/Examples/Scrolling%20Text%20Demo.ino) to sketch and upload
- Profit?

Now verbose:

