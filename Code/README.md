# Code

This folder contains the library code for driving the BUS terminal Signs. The code is still under development and lacks comments to aid implementation. The code should be placed in a folder inside the Arduino library folder. On my Windows machine that's `C:\Users\brian.000\Documents\arduino\libraries\ConnectedHumber`

At this point in time the code uses bit-banging to send the signals to the panel. I intend to include SPI at some point.

The library subclasses the Adafruit_GFX library and, as a result, it is possible to use the GFX drawing and text routines in addition to hardware specific routines.

The `CH_AS1100.h` and `CH_AS1100.cpp` need to be installed in your Arduino library folder. I created a ConnectedHumber folder and placed these in there.

Note that the panels can be programmed when the display is off (see `displayOn()`)

## Includes

Your code needs these includes:

```c
#include <arduino.h>      # may not be needed in the Arduino IDE
#include <Adafruit_GFX.h>
#include <CH_AS1100.h>
```

## constructor

```c
Panel(int dataPin,int clkPin,int LoadPin, int NumChips);
```

The panels consist of a number of AS1100 chips, one per LED sub-panel (normally one character). The full panel has 192 LEDs controlled by 32 AS1100 chips.

It also allocates memory for the pixel (LED) data array

## `begin()`

```c
bool begin();
```

`begin()` initializes CLK, LOAD, DATA signal states. Sets up the panel ready to use.
  
## `display()`

```c
void display(void);   // called to send pixels to the display
```

Sends data to the AS1100 chips to light up the LEDs. Currently, this is done by bit banging. SPI comes later.

## `fillDisplay(state)`

```c
void fillDisplay(int state);
```

Note `state=1` turns all LEDs on, `state=0` turns all LEDs off.

## `clearDisplay()`

```c
void clearDisplay(void);
```

Just a convenience wrapper for `fillDisplay(0)`

## `invertDisplay()`

```c
void invertDisplay();
```

Inverts the pixel on/off states.

## `drawPixel(x,y,color)`

```c
void drawPixel(int16_t x, int16_t y, uint16_t color) override; // essential for GFX 
```

This is an override for the Adafruit_GFX library `drawPixel()` method. It allows the Adafruit_GFX library to draw into our pixel array. See `setPixel()` and `getPixel()`.

## `setClockMode(mode)`

```c
void setClockMode(int m);
```

The AS1100 chips can be driven by an external clock (See AS1100 data sheet). This is called by begin().

## `displayTest(state)`

```c
void displayTest(boolean state);
```

This is a LED test. Regardless of programming, this turns on all LEDs if state=true and off if state=false.

It's useful to check that you don't have any burned out LEDs.

## `setBinaryMode()`

```c
void setBinaryMode();
```

Sets the panels into binary mode which allows us to address each pixel separately. The AS1100 is basically intended for 7 segment displays. This is called during begin().

## `setScan(digits)`

```c
void setScan(int totaldigits);
```

Used to configure the AS1100 number of digit lines (8). This is used by begin() so you don't need to call it.

## `setIntensity(level,chip)`

```c
void setIntensity(int level,int chip);
```

Sets the intensity of one AS1100 chip (character) or ALL chips if chip=-1. Note you cannot set the intensity of individual LEDs.

The range of level is 0-32. `begin()` sets the initial intensity to 5.

## `setIndividualIntensity(chipArray)`

```c
void setIndividualIntensity(int chips[]); // provide an array of intensities, one per chip
```

This allows you to set the intensity of each character in one go. You provide an array of intensity values.

Note that the code does not check if the length of chips is correct

## `displayOn(state)`

```c
void displayOn(int state
```

The entire matrix is turned on when state=1 and off when state=0. The panel can still be programmed whilst the display is off it just won't appear till you turn the display on again.

## `sendPixels()`

```c
void sendPixels();
```

Called by `display()` to send the pixel array to the panel. `display()` is just a convenience wrapper to make the code more readable. Other LCD panels, like the SD1306, is `display()` as a command to send the pixel buffer to the display.

## `setPixel(x,y,on)`

```c
void setPixel(int x, int y,uint8_t on); // pixels can only be on or off
```

This does all the necessary jiggery pokery to set segment bits in the pixel array for the AS1100 chip corresponding to the X position.

Is on=1 the LED will be on after display() is called. If zero then the LED will be off

## `getPixel(x,y)`

```c
uint8_t getPixel(int x,int y);
```

returns the pixel state at `x`, `y` as 0 (off) or 1 (on)

## `scrollRow(dir,row,wrap)`

```c
void scrollRow(int dir,int row,bool wrap);
```

Scrolls a row of pixels using `getPixel()` and `setPixel()`. This needs to be reinvented to shift bit patterns in the pixel array to speed it up. You need to call display() to show changes.

Pixels are shifted by one position on each call.

```c
dir   1 means left to right, -1 means right to left
row   0-7, the row to be scrolled
wrap  if true wrap the scrolling
```

## `scrollRows(dir,wrap)`

```c
void scrollRows(int dir,bool wrap);
```

Scrolls all rows at the same time.

## `scrollColumn(dir,col,wrap)`

```c
void scrollColumn(int dir,int col,bool wrap);
```

Scrolls a column of pixels up or down (The Matrix effect - only smaller)

```c
dir   1 means left to right, -1 means right to left
row   0-7, the row to be scrolled
wrap  if true wrap the scrolling
```

## `scrollColumns(dir,wrap)`

```c
void scrollColumns(int dir,bool wrap);
```

Scrolls all columns up or down one pixel on each call.

## `dumpPixels()`

```c
void dumpPixels();
```

A debugging method - outputs the contents of the pixel array to the serial monitor port as a series of 1's and spaces. Spaces represent LEDs in the off state. It looks like this :-

![Screenshot of output from `dumpPixels`](/pics/dumpPixels.jpg)
  
## Pixel array

```c
uint8_t *pixels[8]; 
```

Each AS1100 chip controls 8 digit lines (rows) and 6 segment lines (columns). The six segment lines are stored in the pixel array hence it is an array of uint8_t. In order to accommodate variable length panels the array is organised as 8 rows of arrays of uint8_t of which only the 6 bottom bits are used - corresponding to the 6 segment outputs of the AS1100.

This arrangement has the advantage that the pixel array can be sent to the chips without alteration to speed up the `sendPixels()` method.

## `Adafruit_GFX`

The driver library subclasses the Adafruit_GFX library thus allowing you to use the routines to write text or draw on the panel.

NOTE: Adafruit routines often expect `uint16_t` not `int` data types, so you may need to cast `int`s to `uint16_t` like this:-

```c
Panel p(...);
p.drawLine((uint16_t) x0,(uint16_t) y0,(uint16_t) x1,uint(16_t) y1,(uint16_t) color);
```

...or use uint16_t in your code.

Note the Adafruit_GFX routines expect a color argument. Since the panel LEDs can only be on or off this library sets the LED on if color > 0 and off if 0.
