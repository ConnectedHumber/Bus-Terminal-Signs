/*
 * Display test. This should flash the entire display every second.
 * @alifeee
 */
#include <SPI.h>

#define LOAD_PIN 5

#define NUM_CHIPS 32

void sendCommand(uint16_t command, int n_times = 1)
{
    digitalWrite(LOAD_PIN, HIGH);

    SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE3));
    for (int i = 0; i < n_times; i++)
    {
        SPI.transfer16(command);
    }
    SPI.endTransaction();

    delayMicroseconds(1);
    digitalWrite(LOAD_PIN, LOW);
    delayMicroseconds(1);
    digitalWrite(LOAD_PIN, HIGH);
}

void setup()
{
    SPI.begin();

    pinMode(LOAD_PIN, OUTPUT);

    digitalWrite(LOAD_PIN, HIGH);
}

void loop()
{
    // display test command, from AS1100 datasheet
    sendCommand(0x0f01, NUM_CHIPS);
    delay(500);
    sendCommand(0x0f00, NUM_CHIPS);
    delay(500);
}