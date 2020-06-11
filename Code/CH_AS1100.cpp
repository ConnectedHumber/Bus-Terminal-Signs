// library converted from 
// A tool to help with BUS LED Display . currently driving one 7219 ic connected too 4 7-seg cc digirs.
// Paul Richards ee@sosgez.co.uk 

// 28 May 2020 04:00

//#define SEND_PIXELS_OLD	// for debugging

// This version : arduino pins wired to Bus Display directly. I've added no logic hardware.

#include <arduino.h>
#include <CH_AS1100.h>

#define ROWS_PER_CHIP 9
#define COLS_PER_CHIP 6
#define CLK_PULSE_WIDTH 1   // 1 microseconds, duration of MARK
#define LOAD_PULSE_WIDTH 2  // 3 microseconds, duration of MARK
#define DATA_PULSE_WIDTH 1  // 1 microseconds, duration of MARK, atually this isn't a pulse, it ensures that CLK occurs within the data pulse.

#define NEXT_PULSE_DELAY 1       // 1 microseconds, duration of SPACE following MARK

#define INVERTED

Panel::Panel(int dataPin,int clkPin,int loadPin, int numChips ) : Adafruit_GFX(numChips*6,9){
  Serial.print("Panel init");
  
  _dataPin=dataPin;
  _clkPin=clkPin;
  _loadPin=loadPin;
  _numChips=numChips;
  
  _numColumns=numChips*COLS_PER_CHIP;


  pinMode(dataPin,OUTPUT);
  pinMode(clkPin,OUTPUT);
  pinMode(loadPin,OUTPUT);
  

  digitalWrite(_dataPin,LOW);
  
  // clk and load are inverted on the control board
  #ifdef INVERTED
	digitalWrite(_clkPin,HIGH);
	digitalWrite(_loadPin,HIGH);  
  #else
	digitalWrite(_clkPin,LOW);
	digitalWrite(_loadPin,LOW);
  #endif
}


Panel::~Panel(){
	for (int row=0;row<ROWS_PER_CHIP;row++)
		delete(pixels[row]);
}


void Panel::dumpPixels(){
int row,col;
for (row=0;row<ROWS_PER_CHIP-1;row++){
	Serial.print("\nrow "+String(row)+" ");
	for (col=0;col<_numColumns;col++) 
		{
		if (getPixel(col,row)>0)	Serial.print(1);
		else Serial.print(" ");
		}
	}
}
void Panel::showCell(int x,int y,int val){
	Serial.print("row:"); Serial.print(y);
	Serial.print(" col:");Serial.print(x);
	Serial.print(" val:");Serial.println(val);
}

boolean Panel::begin(){
  // addressing cells syntax is pixels[row][chip]
 
  for (int row=0;row<ROWS_PER_CHIP;row++)
	pixels[row]=new uint8_t[_numChips];	// only last 6 bits of interest
  
  // initialise pin states etc
  // some delays needed (why??)

  setClockMode(2); // reset the clock to internal
  setClockMode(0);
  setBinaryMode();
  setScan(8); 

  setIntensity(5,-1);	// start low.
  
  delay(500); // for debugging
  
  clearDisplay();
   
  sendPixels();
  
  displayOn(true);
  
  
  return true;
}

void Panel::load(){
// load pulse causes data to be loaded and displayed if display is on
 #ifdef INVERTED
  digitalWrite(_loadPin,LOW);  // transfer from shift register to display drivers buffer
  delayMicroseconds(LOAD_PULSE_WIDTH);
  digitalWrite(_loadPin,HIGH);
  delayMicroseconds(NEXT_PULSE_DELAY);
 #else
  digitalWrite(_loadPin,HIGH);  // transfer from shift register to display drivers buffer
  delayMicroseconds(LOAD_PULSE_WIDTH);
  digitalWrite(_loadPin,LOW);
  delayMicroseconds(NEXT_PULSE_DELAY);
#endif 
}

void Panel::clk(){
	// clock is inverted on panel
#ifdef INVERTED
  digitalWrite(_clkPin,LOW);  
  delayMicroseconds(CLK_PULSE_WIDTH);
  digitalWrite(_clkPin,HIGH);
  delayMicroseconds(NEXT_PULSE_DELAY);
#else
  digitalWrite(_clkPin,HIGH);  
  delayMicroseconds(CLK_PULSE_WIDTH);
  digitalWrite(_clkPin,LOW);
  delayMicroseconds(NEXT_PULSE_DELAY);
#endif
}

void Panel::writeDigit(int digit,uint8_t d){
	//d=d & 0xFF;	// mask off trash	
	int dd = (digit+1) << 8;
	write16(0x0000 | dd | d );
}

void Panel::write16(int d){
  // first 4 bits are don't care so we send zeros
  // caller must call load() if this is the last write16
  //Serial.print("write16 word:0x");Serial.println(d,HEX);
  int mask = 0x0800;
  digitalWrite(_dataPin,LOW); // send leading 0b0000
  delayMicroseconds(1);
  clk();
  clk();
  clk();
  clk();
  for(int i=0;i<12;i++)
  { // then the lower 12 bit word
    digitalWrite(_dataPin,(d & mask)>0 ? HIGH : LOW);
	delayMicroseconds(NEXT_PULSE_DELAY);	// allow data to settle
    clk();
    mask >>= 1;
  }
  digitalWrite(_dataPin,LOW); // end WITH 0 - EASIER TO DEBUG
}

void Panel::drawPixel(int16_t x,int16_t y,uint16_t color){
	// link to Adafruit_GFX library
	// leds are on or off - no colour control
	// led colours of zero are off the rest on
	// we cannot control the brightness of individual
	// pixels
	//Serial.println("drawPixel x:"+String(x)+" y:"+String(y)+" color:"+String(color));
	uint8_t on=color>0?1:0;

	setPixel(x,y,on);
}


void Panel::display(void) {
	// send all pixels to the display
 	sendPixels();
}
 
void Panel::fillDisplay(int state){
	// sets all pixels to same colour
	int row,chip;
	
	state=state>0?0x3F:0;	// LEDs are ON or OFF
	
   for (row=0;row<ROWS_PER_CHIP-1;row++){
    for (chip=0;chip<_numColumns;chip++){
		pixels[row][chip]=(uint8_t)state;	
	  }
	}
}

void Panel::clearDisplay(void){
   // convenience function
   fillDisplay(0);
}
 
void Panel::invertDisplay(){
	// inverts the state of all pixels
	int row,chip;
    for (row=0;row<ROWS_PER_CHIP;row++)  
		for (chip=0;chip<_numChips;chip++) 
			pixels[row][chip]=(! pixels[row][chip]) & 0x3F;
}
 
void Panel::setIntensity(int level,int chipNum=-1){
 // panel only supports 16 intensity levels
 // if chipNum>-1 only affects that chip
 if (level<0 && level>15) return;		// ignore 
 if (chipNum==-1)	// set all chips 
	{
		for (int c=0;c<_numChips;c++) write16(0x0A00+ level);	// the chip whose brightness needs setting
		load();
		return;
	}
 
 // we are setting one chip use no-op to step past chips not to be changed
 
 if (chipNum>0)
	{
	for (int c=0;c<chipNum-1;c++) write16(0);		// NO OP
	write16(0x0A00+ level);	// the chip whose brightness needs setting
	load();
	}
	
}

void Panel::setIndividualIntensity(int chips[]){
    // chip[s is an array of intensity values, one per chip
	for (int i=0;i<_numChips;i++) write16(0x0A00+ (chips[i] & 15));
	load();
}

uint8_t Panel::getPixel(int col,int row){ // humans like x,y order
  // array is in [row][col] order
  //
  if (col>_numColumns || col<0) return 0;
  if (row>7 || row<0) return 0;
	
  int chip=col/6;
  uint8_t mask=1<<(col%6);
  return (pixels[row][chip] & mask)?1:0;
}

void Panel::setPixel(int col,int row,uint8_t value){
  // value is reduced to 1 or 0 in drawPixel
  //Serial.println("setPixel col:"+String(col)+" row:"+String(row)+" value:"+String(value));
  
  if (col>_numColumns || col<0) return;
  if (row>7 || row<0) return;
	
  int chip=col/6;
  uint8_t mask=1<<(col%6);
  uint8_t chipBits=pixels[row][chip];
  
  if ((chipBits & mask) && (value==0)) pixels[row][chip]=chipBits & mask;
  else pixels[row][chip]=chipBits  | mask;
  
  return;
}


void Panel::sendPixels(){

  //int start=millis();
  
  int digit,chip;
  uint8_t seg;
  
  // moving down the digits (rows)
  for(digit=0;digit<8;digit++){
	//working from the righthand column as that needs to be sent first
	for(chip=_numChips-1;chip>=0;chip--) writeDigit(digit,pixels[7-digit][chip]);
	load();
  } // digit
	  
  //int end=millis();
  //int taken=end-start;
  //Serial.print("sendPixels time:");Serial.println(taken);
}



void Panel::sendCmd(int data){
// workhorse for many register updates below
// only used when all registers need to contain the same value
 for (int chip=0;chip<_numChips;chip++) write16(data);  
 load();
}

void Panel::setClockMode(int m){
	sendCmd(0x0E00 + (m & 3));
}

void Panel::displayOn(int b){
  sendCmd(0x0C00 + (b & 1));
}
void Panel::displayTest(bool state){
	// panel is turned on if state is non-zero, otherwise off
	if (state) sendCmd(0x0F01);
	else sendCmd(0x0F00);
}

void Panel::setBinaryMode(){
  sendCmd(0x0900);
}

void Panel::setScan(int totaldigits){
  sendCmd(0x0B00 + ((totaldigits-1)&7));  // 7 means 8 blocks of 8
}


void Panel::scrollRow(int dir,int row,bool wrap=false){
	// scrolls one horizontal column only
	// dir>0 means LEFT, dir<0 means RIGHT
	uint8_t prev=0;
    int col=0;
  
	if (dir>0) { // LEFT
		if (wrap) prev=getPixel(col,row);
		while(col<_numColumns) {
			setPixel(col,row,getPixel(col+1,row));
			col++;
		}
		setPixel(col,row,prev);
	}
	else{ // RIGHT
		col=_numColumns-1; // zero based
		
		if (wrap) prev=getPixel(col,row);
		while(col>0) {
		setPixel(col,row,getPixel(col-1,row));
			col--;
			}
		setPixel(col,row,prev);
	}
}

void Panel::scrollRows(int dir,bool wrap=false){
  for (int row=0;row<ROWS_PER_CHIP;row++)
	scrollRow(dir,row,wrap);
}

void Panel::scrollColumn(int dir,int col,bool wrap=false){
	// scrolls one vertical column only
	// dir>0 means UP, dir<0 means down

	uint8_t prev=0;
	int row=0;
    
	if (dir>0) { // up
		if (wrap) prev=getPixel(col,row);	
		while(row<ROWS_PER_CHIP) setPixel(col,row,getPixel(col,++row));
		setPixel(col,row,prev);
	}
	else{ // down
		row=ROWS_PER_CHIP;
		
		if (wrap) prev=getPixel(col,row);
		while(row>0) setPixel(col,row,getPixel(col,--row));
		setPixel(col,row,prev);
	}
}

void Panel::scrollColumns(int dir,bool wrap=false){
  for (int col=0;col<_numColumns;col++)	scrollColumn(dir,col,wrap);
}
