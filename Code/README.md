# Code

This folder contains the library code for driving the BUS terminal Signs.

For example code see [./Examples](./Examples/).

For documentation see the [code file](./CH_AS1100.cpp) or [online documentation](https://ConnectedHumber.github.io/Bus-Terminal-Signs).

## Using code

The code should be placed in a folder inside the Arduino library folder. On my Windows machine that's `C:\Users\brian.000\Documents\arduino\libraries\ConnectedHumber`.

Alternatively, a symbolic link can be used. This way, code changes in the cloned repository are reflected in the Arduino library area. On Windows, you can do this using

```bash
mklink /D %USERPROFILE%\Documents\Arduino\libraries\ConnectedHumber %USERPROFILE%\Documents\GitHub\Bus-Terminal-Signs\Code
```

## Protocols

At this point in time the code uses bit-banging to send the signals to the panel. I intend to include SPI at some point.

## Notes

Note that the panels can be programmed when the display is off (see `displayOn()`)

Commands that alter pixels do not change the display. You must call `Panel.display()` to update the panel, after setting the pixels.

## Includes

Your code needs these includes:

```c
#include <arduino.h>      # may not be needed in the Arduino IDE
#include <CH_AS1100.h>
```

## Pixel array

```c
uint8_t *pixels[8]; 
```

Each AS1100 chip controls 8 digit lines (rows) and 6 segment lines (columns). The six segment lines are stored in the pixel array hence it is an array of uint8_t. In order to accommodate variable length panels the array is organised as 8 rows of arrays of uint8_t of which only the 6 bottom bits are used - corresponding to the 6 segment outputs of the AS1100.

This arrangement has the advantage that the pixel array can be sent to the chips without alteration to speed up the `sendPixels()` method.

## `Adafruit_GFX`

The library subclasses the Adafruit_GFX library and, as a result, it is possible to use the GFX drawing and text routines in addition to hardware specific routines.

NOTE: Adafruit routines often expect `uint16_t` not `int` data types, so you may need to cast `int`s to `uint16_t` like this:-

```c
Panel p(...);
p.drawLine((uint16_t) x0,(uint16_t) y0,(uint16_t) x1,uint(16_t) y1,(uint16_t) color);
```

...or use uint16_t in your code.

Note the Adafruit_GFX routines expect a color argument. Since the panel LEDs can only be on or off this library sets the LED on if color > 0 and off if 0.
