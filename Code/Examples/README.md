# Arduino code examples

Code which works with the setup described in [setup.md](../../Setup.md).

## `scrolling_text.ino`

[CODE](./scrolling_text.ino)

This displays `"Makerspace Hull :)"` scrolling across the screen. For some reason, it breaks if I remove (the redundant) `bottomPanel`. I should figure this out (for code simplification case).

## `Scrolling Text Demo.ino`

[CODE](./Scrolling%20Text%20Demo.ino)

The old scrolling text demo code.

## `display_test.ino`

[CODE](./display_test.ino)

This toggles the "display test" signal (section 8.8 of [AS1100 datasheet]) every half a second, to test the display.

This code should be rewritten so that it clears the display before beginning, as memory from before remains on the screen. Alternatively, it could remain as is (as the datasheet specifically describes that the display test returns to normal if turned off).

[AS1100 datasheet]: https://github.com/ConnectedHumber/Bus-Terminal-Signs/blob/master/Datasheets/AS1100_DS000273_1-00.pdf

## `spi_test.ino`

[CODE](./spi_test.ino)

This repeatedly sends an SPI signal, so that it can be viewed on an oscilloscope, and ensured that it's the correct signal that the [AS1100][AS1100 datasheet] desires.

[See more info about SPI](../SPI/README.md)
