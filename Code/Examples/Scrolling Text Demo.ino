
/*
 * Test program for bus sign library 
 * 
 */
#include <Adafruit_GFX.h>
#include <CH_AS1100.h>


// just 3 digital ports to control. Any you see fit will do - bit banged
#define DATA_PIN 25
#define CLK_PIN 26
#define LOAD_PIN 27

// bottom panel
#define DATA_PIN_2  18
#define CLK_PIN_2 19
#define LOAD_PIN_2 21

#define NUM_CHIPS 12


int ledState=LOW;

Panel topRow=Panel(DATA_PIN,CLK_PIN,LOAD_PIN,NUM_CHIPS);
Panel bottomRow=Panel(DATA_PIN_2,CLK_PIN_2,LOAD_PIN_2,NUM_CHIPS);


void flashLED(int delay_ms,int numTimes){
// flashes the onboard built in led
// an indicator
 bool on=true;
  for (int t=0;t<numTimes;t++)
    {
      digitalWrite(LED_BUILTIN,on);
      delay(delay_ms);
	    on=!on;
    }
}


void showText(Panel & p,char * msg)
{       
     p.setCursor(0, 0);
     p.setTextColor(1);  
     p.setTextSize(1); // pixel size multiplier
     p.println(msg);
     p.display();
}

void setup(){

  Serial.begin(115200);
  while(!Serial) yield();

  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW); // off

  topRow.begin();
  bottomRow.begin();
    
  flashLED(200,3);

  showText(topRow,(char *)"Connected");
  showText(bottomRow,(char *)"Humber");
}

void loop()
{
   topRow.scrollRows(1,true);     // wrap scroll left to right
   topRow.display();
   bottomRow.scrollRows(-1,true); // wrap scroll right to left
   bottomRow.display();
   delay(100);
}
