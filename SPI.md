# SPI

Serial Peripheral Interface ([SPI]) is used to communicate to the [AS1100].

Thusly described is my coming to understand SPI a bit more, and setting it up on the Arduino with the [Arduino SPI library].

[SPI]: https://en.wikipedia.org/wiki/Serial_Peripheral_Interface
[AS1100]: ./Datasheets/AS1100_DS000273_1-00.pdf
[Arduino SPI library]: https://www.arduino.cc/reference/en/language/functions/communication/spi/

## Setup

Oscilloscope connected to Arduino, and both to a common ground.

![Picture of total setup, including oscilloscope, Arduino, and breadboard.](images/SPI_setup_zoomout.png)

![Zoom of Arduino, showing clock to pin 13, data to pin 11, load to pin 5, and ground to ground pin.](images/SPI_setup_arduino.png)

The breadboard is just used for common ground. Other elements here are for the bus sign, which is out of the scope of this file.

![Zoom of breadboard, showing common ground connections](images/SPI_setup_common-ground.png)

## Code

This code uses the [Arduino SPI library] to repeatedly send an SPI message.

The SPI settings come from the [AS1100 datasheet][AS1100], with help on choosing them from the [Arduino SPI library]. They are tabulated here

### SPI settings

| Setting | Value | Reason |
| --- | --- | --- |
| Frequency | 10000000 (10 MHz) | The [datasheet][AS1100] (section 6, table 3) says 10 ns is the minimum clock cycle time, so 10 MHz is thus the maximum frequency. |
| Bit-significance | `MSBFIRST` | The [AS1100] expects the MSB to be first (section 8.1, table 4) |
| SPI Mode | `SPI_MODE3` | This mode has the clock HIGH when no data is being sent, and the data is read on the RISING clock signal. The [datasheet][AS1100] requires (section 8.1) the opposite polarity (clock to be LOW when idling), but the electronics use an inverter (as per [Hardware.md#ribbon-cable](Hardware.md#ribbon-cable)), so we must invert it. |

```c
/*
 * SPI test
 */
#include <SPI.h>

#define DATA_PIN 11
#define CLK_PIN 13

void setup(){
  SPI.begin();
}

void loop()
{
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE3));
  SPI.transfer16(0x0e02);
  SPI.endTransaction();
  delay(100);
}
```

## Oscilloscope trace

Here is the result: a pretty nice SPI signal. The peak to peak is around 5 V, a better sign than [before](../code%20information.md#oscilloscope).

![Screenshot of oscilloscope, showing 16 clock pulses (channel 1), and the data trace (channel 2).](scope.BMP)

Now we can use this instead of bit banging to communicate with the screens.
