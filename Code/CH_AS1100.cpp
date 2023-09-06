/** \mainpage Hull Bus Terminal Signs
 *
 * \section intro_sec Introduction
 *
 * Documentation for the code to use the Hull bus terminal signs.
 *
 * The most useful page to look at is the [Panel class](./classPanel.html).
 *
 * For hardware setup instructions see [README.md](https://github.com/ConnectedHumber/Bus-Terminal-Signs/tree/master/README.md) and [Setup.md](https://github.com/ConnectedHumber/Bus-Terminal-Signs/tree/master/Setup.md).
 *
 * For example code see [examples](https://github.com/ConnectedHumber/Bus-Terminal-Signs/tree/master/Code/Examples).
 *
 * \section contribs Contributors
 *
 * Paul Richards ee@sosgez.co.uk
 *
 * alifeee - alifeee.web@outlook.com
 */

// #define SEND_PIXELS_OLD	// for debugging

#include <CH_AS1100.h>
#include <arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>

#define ROWS_PER_CHIP 8 // there are 9 rows but only 8 used
#define COLS_PER_CHIP 6
#define LOAD_PULSE_WIDTH 2 // 3 microseconds, duration of MARK

#define NEXT_PULSE_DELAY 1 // 1 microseconds, duration of SPACE following MARK

/**
 * @brief initialise pins used and allocate array for pixel data
 *
 * The panels consist of a number of AS1100 chips, one per LED sub-panel (normally one character). The full panel has 192 LEDs controlled by 32 AS1100 chips.
 */

Panel::Panel(int loadPin, int numChips) : Adafruit_GFX(numChips * COLS_PER_CHIP, ROWS_PER_CHIP)
{
  _loadPin = loadPin;
  _numChips = numChips;

  _numColumns = numChips * COLS_PER_CHIP;

  pinMode(loadPin, OUTPUT);

  // load is inverted on the control board
  digitalWrite(_loadPin, HIGH);

  // addressing of cells is pixels[row][chip]
  for (int row = 0; row < ROWS_PER_CHIP; row++)
    pixels[row] = new uint8_t[numChips]; // only bits 0..5 are of interest
}

/**
 * @brief frees memory used for the pixel array
 */
Panel::~Panel()
{
  for (int row = 0; row < ROWS_PER_CHIP; row++)
    delete (pixels[row]);
}

/**
 * @brief displays the contents of the pixel array for debugging
 *
 * A debugging method - outputs the contents of the pixel array to the serial monitor port as a series of 1's and spaces. Spaces represent LEDs in the off state. It looks like this :-
 *
 * ```bash
 * row 0 1   1   1 1
 * row 1  1  1 1  1
 * row 2  1  1   1 1
 * row 3  1  1 1     1
 * ```
 */
void Panel::dumpPixels()
{
  int row, col;
  for (row = 0; row < ROWS_PER_CHIP; row++)
  {
    Serial.print("\nrow " + String(row) + " ");
    for (col = 0; col < _numColumns + 1; col++)
    {
      if (getPixel(col, row) > 0)
        Serial.print(1);
      else
        Serial.print(" ");
    }
  }
  Serial.println();
}

/**
 * @brief Prints the value of a specific cell to the serial monitor
 */

void Panel::showCell(int x, int y, int val)
{
  Serial.print("row:");
  Serial.print(y);
  Serial.print(" col:");
  Serial.print(x);
  Serial.print(" val:");
  Serial.println(val);
}

/**
 * @brief initialises the panel. Clears the display and makes the display visible
 *
 * initializes LOAD signal states. Sets up the panel ready to use.
 */
boolean Panel::begin()
{
  SPI.begin();
  // initialise pin states etc
  // some delays needed (why??)

  setClockMode(2); // reset the clock to internal
  setClockMode(0);
  setBinaryMode();
  setScan(ROWS_PER_CHIP);
  setIntensity(2, -1); // start low.
  clearDisplay();

  sendPixels();

  displayOn(1);
  return true;
}

/**
 * private function used to generate a load pulse
 *
 * This is send after an SPI signal on CLOCK/DATA to tell the chip to follow the instructions from the pulse.
 */
void Panel::load()
{
  // load pulse causes data to be loaded and displayed if display is on
  digitalWrite(_loadPin, LOW); // transfer from shift register to display drivers buffer
  delayMicroseconds(LOAD_PULSE_WIDTH);
  digitalWrite(_loadPin, HIGH);
  delayMicroseconds(NEXT_PULSE_DELAY);
}

/**
 * private function used to set turn LEDs on/off
 * This will be called once per chip and per row
 *
 * d - pixel data
 * digit - row
 */
void Panel::writeDigit(int digit, uint8_t d)
{
  // d=d & 0xFF;	// mask off trash
  int dd = (digit + 1) << 8;
  write16(0x0000 | dd | d);
}

/**
 * @brief sends a 16 bit word to the chips
 *
 * the user must call load() when finished.
 *
 * normally there will be one write16() per chip
 * if you only want to address one chip, you can send noop (0x00) a number of times, and then write16 for the chip you want to address
 * (see datasheet section 8.9)
 */
void Panel::write16(int d)
{
  // first 4 bits are don't care so we send zeros
  // caller must call load() if this is the last write16
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE3));
  SPI.transfer16(d);
  SPI.endTransaction();
}

/**
 * @brief use setPixel instead
 *
 * called by the Adafruit GFX library to set pixels in the pixel array
 * note this only sets pixels it doesn't clear pixels to a background
 * colour - effectively that allows for transparent backgrounds
 * but since we only have two colours that would just make a mess of
 * the display
 *
 * this method delegates to setPixel() after changing the colour to 1 (on) or 0 (off)
 * Any non-zero colour value is considered to mean 'on'
 */
void Panel::drawPixel(int16_t x, int16_t y, uint16_t color)
{
  // link to Adafruit_GFX library
  // leds are on or off - no colour control
  // led colours of zero are off the rest on
  // we cannot control the brightness of individual
  // pixels
  uint8_t on = color > 0 ? 1 : 0;

  setPixel(x, y, on);
}

/**
 * @brief sends the pixel buffer to the display.
 *
 * This must be called every time after editing the pixels.
 *
 * It a wrapper for sendPixels().
 */
void Panel::display(void)
{
  // send all pixels to the display
  sendPixels();
}

/**
 * @brief Sets the entire pixel buffer to off or on
 * if state is 0 the leds are all off
 * is state is non-zero the leds are all on
 */
void Panel::fillDisplay(int state)
{
  // sets all pixels to same colour
  int row, chip;

  state = state != 0 ? 0x3F : 0; // LEDs are ON or OFF

  for (row = 0; row < ROWS_PER_CHIP; row++)
  {
    for (chip = 0; chip < _numColumns; chip++)
    {
      pixels[row][chip] = (uint8_t)state;
    }
  }
}

/**
 * @brief equivalent to fillDisplay(0)
 */
void Panel::clearDisplay(void)
{
  fillDisplay(0);
}

/**
 * @brief switches LED states from off to on and vice versa
 */
void Panel::invertDisplay()
{
  int row, chip;
  for (row = 0; row < ROWS_PER_CHIP; row++)
    for (chip = 0; chip < _numChips; chip++)
      pixels[row][chip] = (!pixels[row][chip]) & 0x3F;
}

/**
 * @brief Sets the intensity of one AS1100 chip (character) or ALL chips if chip=-1.
 * Note: you cannot set the intensity of individual LEDs.
 *
 * if chipNum is -1 (default) sets ALL chips to same intensity level
 * level can be 0..16
 * levels outside this range are ignored.
 *
 * The range of level is 0-32. `begin()` sets the initial intensity to 5.
 */
void Panel::setIntensity(int level, int chipNum = -1)
{
  // panel only supports 16 intensity levels
  // if chipNum>-1 only affects that chip
  if (level < 0 && level > 15)
    return;          // ignore
  if (chipNum == -1) // set all chips
  {
    for (int c = 0; c < _numChips; c++)
      write16(0x0A00 + level); // the chip whose brightness needs setting
    load();
    return;
  }

  // we are setting one chip use no-op to push past chips not to be changed

  if (chipNum > 0)
  {
    for (int c = 0; c < chipNum - 1; c++)
      write16(0);            // NO OP
    write16(0x0A00 + level); // the chip whose brightness needs setting
    load();
  }
}

/**
 * @brief Sets all the chips' intensities with one function.
 *
 * Chips is an array of intensity levels in chip order
 * i.e chips[0] is the first chip.
 *
 * This allows you to set the intensity of each character in one go. You provide an array of intensity values.
 *
 * Note that the code does not check if the length of chips is correct
 */

void Panel::setIndividualIntensity(int chips[])
{
  // chip[s is an array of intensity values, one per chip
  for (int i = _numChips - 1; i >= 0; i--)
    write16(0x0A00 + (chips[i] & 15));
  load();
}

/**
 * @brief returns a pixel state as 1 (on) or 0 (off)
 *
 * col & row are bounds checked. If the col/row is outside the
 * led array zero is returned
 */
uint8_t Panel::getPixel(int col, int row)
{ // humans like x,y order
  // array is in [row][col] order
  if (col > _numColumns || col < 0)
    return 0;
  if (row > (ROWS_PER_CHIP - 1) || row < 0)
    return 0;

  int chip = col / COLS_PER_CHIP;
  uint8_t mask = 1 << (col % COLS_PER_CHIP);
  return (pixels[row][chip] & mask) ? 1 : 0;
}
/**
 * @brief Toggle individual pixel. You must call display() after.
 *
 * Toggle one pixel with col + row. 0 is off. 1 (or anything else) is on.
 *
 * col and row are bounds checked no action is taken if out of bounds
 *
 * This does all the necessary jiggery pokery to set segment bits in the pixel array for the AS1100 chip corresponding to the X position.
 */
void Panel::setPixel(int col, int row, uint8_t value)
{
  // value is reduced to 1 or 0 in drawPixel

  if ((col > _numColumns) || (col < 0))
    return;
  if ((row > 7) || (row < 0))
    return;

  int chip = col / 6;
  uint8_t mask = 1 << (col % 6);
  uint8_t chipBits = pixels[row][chip];

  if (value == 0)
    pixels[row][chip] = chipBits & ~mask;
  else
    pixels[row][chip] = chipBits | mask;

  return;
}

/**
 * @brief sends the pixel array to the panel chips
 *
 * called from display()
 */

void Panel::sendPixels()
{
  int digit, chip;
  uint8_t seg;

  // moving down the digits (rows) there are 8 digit lines per
  // chip

  for (digit = 0; digit < 8; digit++)
  {
    // working from the righthand column as that needs to be sent first
    for (chip = _numChips - 1; chip >= 0; chip--)
    {
      writeDigit(digit, pixels[7 - digit][chip]);
    }
    load();
  } // digit
}

/**
 * @brief sends the same command to all chips followed by load()
 */

void Panel::sendCmd(int data)
{
  // workhorse for many register updates below
  // only used when all registers need to contain the same value
  for (int chip = 0; chip < _numChips; chip++)
    write16(data);
  load();
}

/**
 * The AS1100 chips can be driven by an external clock (See AS1100 data sheet). This is called by begin().
 *
 * This funcion is also used to reset the display. See datasheet, section 8.10.
 */
void Panel::setClockMode(int m)
{
  sendCmd(0x0E00 + (m & 3));
}

/**
 * @brief Turns the panel on/off. This needs to be called to see anything.
 * Can be called to flash the display since LED state is not affected
 *
 * state 0 means off, non-zero means on
 *
 * The panel can still be programmed whilst the display is off it just won't appear till you turn the display on again.
 */

void Panel::displayOn(int state)
{
  if (state != 0)
    sendCmd(0x0C01);
  else
    sendCmd(0x0C00);
}

/**
 * @brief LED test. Regardless of programming, this turns on/off all LEDs
 *
 * on if state=true and off if state=false.
 *
 * It's useful to check that you don't have any burned out LEDs.
 */
void Panel::displayTest(bool state)
{
  // panel is turned on if state is non-zero, otherwise off
  if (state)
    sendCmd(0x0F01);
  else
    sendCmd(0x0F00);
}

/**
 * This puts the chips in binary mode which allows us to
 * control which leds are on/off using the digit and segment lines
 *
 * This allows us to address each pixel separately.
 *
 * The AS1100 is basically intended for 7 segment displays, but with this we can control a dot-matrix of pixels too.
 *
 * See datasheet section 9.3
 */
void Panel::setBinaryMode()
{
  sendCmd(0x0900);
}

/**
 * Set the scan mode of the chips - ie,. telling them how many
 * digits lines to use
 *
 * Used to configure the AS1100 number of digit lines (8). This is used by begin() so you don't need to call it.
 */

void Panel::setScan(int totaldigits)
{
  sendCmd(0x0B00 + ((totaldigits - 1) & 7)); // 7 means 8 blocks of 8
}

/**
 * @brief Scroll an entire row
 *
 * if dir>0 scroll left to right
 * if dir<0 scroll right to left
 *
 * wrap causes the scrolled row to wrap around.
 *
 * Uses `getPixel()` and `setPixel()`. This needs to be reinvented to shift bit patterns in the pixel array to speed it up. You need to call display() to show changes.
 *
 * Pixels are shifted by one position on each call.
 */
void Panel::scrollRow(int dir, int row, bool wrap = false)
{
  // scrolls one horizontal column only
  // dir>0 means LEFT, dir<0 means RIGHT
  uint8_t carry = 0;
  int col = 0;
  int lastCol = _numColumns - 1;

  if (dir > 0)
  { // LEFT to RIGHT
    if (wrap)
      carry = getPixel(lastCol, row);
    for (col = lastCol; col > 0; col--)
      setPixel(col, row, getPixel(col - 1, row));
    setPixel(0, row, carry);
  }

  if (dir < 0)
  { // RIGHT to LEFT
    if (wrap)
      carry = getPixel(0, row);
    for (col = 0; col < lastCol; col++)
      setPixel(col, row, getPixel(col + 1, row));
    setPixel(lastCol, row, carry);
  }
}

/**
 * @brief scroll ALL rows by calling scrollRow for each row
 */
void Panel::scrollRows(int dir, bool wrap = false)
{
  for (int row = 0; row < ROWS_PER_CHIP; row++)
    scrollRow(dir, row, wrap);
}

/**
 * @brief Scrolls a single column with optional wrap around
 *
 * Scrolls a column of pixels up or down (The Matrix effect - only smaller)
 *
 * ```c
 * dir   1 means left to right, -1 means right to left
 * row   0-7, the row to be scrolled
 * wrap  if true wrap the scrolling
 * ```
 */
void Panel::scrollColumn(int dir, int col, bool wrap = false)
{
  // scrolls one vertical column only
  // dir>0 means UP, dir<0 means down

  uint8_t carry = 0;
  int row = 0;
  int lastRow = ROWS_PER_CHIP - 1;

  if (dir > 0)
  { // up
    if (wrap)
      carry = getPixel(col, 0);
    for (row = 0; row < lastRow; row++)
      setPixel(col, row, getPixel(col, row + 1));
    setPixel(col, lastRow, carry);
  }
  else
  { // down
    row = ROWS_PER_CHIP;

    if (wrap)
      carry = getPixel(col, lastRow);
    for (row = lastRow; row > 0; row--)
      setPixel(col, row, getPixel(col, row - 1));
    setPixel(col, 0, carry);
  }
}

/**
 * @brief Scrolls all columns up or down one pixel on each call.
 */

void Panel::scrollColumns(int dir, bool wrap = false)
{
  for (int col = 0; col < _numColumns; col++)
    scrollColumn(dir, col, wrap);
}
