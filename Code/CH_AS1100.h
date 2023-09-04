/**
 * Bus sign class
 * modified from Paul Richards work May 2020
 *
 *
 *
 */
#pragma once

#include <arduino.h>
#include <Adafruit_GFX.h>

class Panel : public Adafruit_GFX
{
public:
  Panel(int dataPin, int clkPin, int LoadPin, int NumChips);
  ~Panel();

  bool begin();                // initialises CLK,LOAD,DATA signal states
  void display(void);          // called to send pixels to the display
  void fillDisplay(int state); // state=1 leds on, state=0 leds off. Call display() after
  void clearDisplay(void);
  void invertDisplay();

  void drawPixel(int16_t x, int16_t y, uint16_t color) override; // essential for GFX
  // void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,uint16_t color);

  void setClockMode(int m);
  void displayTest(bool state); // all leds turned on if state is true, off if false
  void setBinaryMode();
  void setScan(int totaldigits);
  void setIntensity(int level, int chip);   // sets intensity of one or all chips (-1 default)
  void setIndividualIntensity(int chips[]); // provide an array of intensities, one per chip

  void displayOn(int b); // 1=on, 0=off. Entire matrix is turned on/off
  void sendPixels();
  void setPixel(int x, int y, uint8_t on); // pixels can only be on or off
  uint8_t getPixel(int x, int y);

  void scrollRow(int dir, int row, bool wrap);
  void scrollRows(int dir, bool wrap);
  void scrollColumn(int dir, int col, bool wrap);
  void scrollColumns(int dir, bool wrap);

  void dumpPixels();

private:
  void load();
  void clk();
  void write16(int d);
  void writeDigit(int digit, uint8_t d); // used by display()

  void showCell(int x, int y, int val);
  void sendCmd(int cmdData);

  int _loadPin = 0;
  int _clkPin = 0;
  int _dataPin = 0;

  // hardware related
  int _numChips = 0; // assigned in constructor to allow for variable length panels
  int _numColumns = 0;

  uint8_t *pixels[8]; // each chip has 8 rows and 6 segments
};
