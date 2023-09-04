
/*
 * SPI test
 *
 */
#include <SPI.h>

#define DATA_PIN 11
#define CLK_PIN 13

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
