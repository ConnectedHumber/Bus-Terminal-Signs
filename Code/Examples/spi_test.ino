
/*
 * SPI test
 * Probs pins 11 (Data) and 13 (Clock) with an oscilloscope to see the SPI signal
 * @alifeee
 */
#include <SPI.h>

void setup()
{
    SPI.begin();
}

void loop()
{
    SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE3));
    SPI.transfer16(0x0e02);
    SPI.endTransaction();
    delay(100);
}
