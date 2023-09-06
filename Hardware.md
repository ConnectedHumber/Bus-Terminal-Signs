# Hardware

The units were connected to a server via an Ethernet daisy chain. The server broke and so these units were destined for the skip.

The unit consists of a number of LED sub-panels which plug into the main LED panel - this makes it easy to change an LED sub-panel if one breaks. Each LED sub-panel is a block of single colour yellow LEDs arranged in a matrix of 24 LEDs wide by 9 high.

Don't get too excited about using the LED submodules on their own because the pins are on a 2 mm spacing. That's OK for using Dupont cables.

The sign has a hinged construction for maintenance access.

![Picture of unit](pics/Panel%20Opened%20for%20Maintenence.jpg)

The LED control panels use two AS1100 chips per LED sub-panel providing 2 text characters per panel. We are able to drive these panels as if they were a simple matrix.

## Control modules

The unit power and Ethernet cables feed through a filter PCB (shown at right on this picture) then on to the 150W 5V DC power supply for the LED panels and to a separate AC-DV 5V for the CPU.

This picture shows the control modules:

![Picture of control modules](pics/Control%20Modules.png)

## LED sub-panel matrix

This diagram shows how the pins on the LED sub panels are connected to the LEDs.

![Picture of LED Matrix](pics/LED%20matrix.jpg)

## Ribbon cable

![Picture of ribbon cable](images/setup_ribbon-cable.png)

The ribbon cable which connects the interface to the LED panel has these designations.

See [pin designation table in README.md](./README.md#pin-designation).

The LED panel inverts the CLK and LOAD signals using a hex Schmitt inverter, so you need to invert them in code OR use a Schmitt inverter.

I assume the CLK and LOAD signals are fed through a Schmitt to reduce noise as clocking is done on the rising/falling edges. The data signal is usually stable when CLK and LOAD switch state.

The IDC connectors are rather small (does anyone know the part number?) so I removed the IDC connector and soldered the wires to a 2.54 mm female header to allow me to plug it onto a prototype interface PCB using male 2.54 mm header pins.

## Using an Arduino to drive the panels

The BUS Terminal LED board uses 5V and is connected, via a ribbon cable, to a driver PCB. There are several solutions to using these with Arduinos. See [Setup.md](./Setup.md).

### If using a 3V3 Arduino device

It is possible to drive the panel by hardwiring a 3V3 device pins directly to the chips. The 3V3 is just enough for the 5V logic to operate. However, this solution is not recommended for a robust solution.

I have used a level shifter on the pins of an ESP32 (3v3) to get the full voltage swing. One nice thing about using a level shifter is that you can adjust the output signal voltage swing by changing the HV side of the shifter - I mention this because during my experiments I managed to wind the signals down to 3V5 and still drive the LED panel through the ribbon cable.

The jury is still out on the reliability of driving the panels with 5V0 signal swings because I experienced dropout (missing LEDs). I'll update this when I've done more work on this.

### Using a 5V Arduino

A 5V device can drive the LEDs via the ribbon cable which normally connect the interface board to the LED driver PCB.
