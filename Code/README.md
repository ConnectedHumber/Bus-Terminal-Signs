# Readme
This folder contains the library code for driving the BUS terminal Signs. The code is still under development and lacks comments to aid implementation. The code should be placed in a folder inside the Arduino library folder. On my Windows machine that's C:\Users\brian.000\Documents\arduino\libraries\ConnectedHumber

At this point in time the code uses bit banging to send the signals to the panel. I intend to include SPI at some point.

The library sub-classes the Adafruit_GFX library and , as a result, it is possible to use the GFX drawing and text routines.
