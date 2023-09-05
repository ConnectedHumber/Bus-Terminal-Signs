/*
 * Scrolling text
 * @alifeee
 */
#include <CH_AS1100.h>

// top panel
#define LOAD_PIN 7

// bottom panel
#define LOAD_PIN_2 5

// panel length
#define NUM_CHIPS 32

Panel topRow = Panel(LOAD_PIN, NUM_CHIPS);
Panel bottomRow = Panel(LOAD_PIN_2, NUM_CHIPS);

void showText(Panel &p, char *msg)
{
    p.setCursor(0, 0);
    p.setTextColor(1);
    p.setTextSize(1); // pixel size multiplier
    p.println(msg);
    p.display();
}

void setup()
{
    topRow.begin();
    bottomRow.begin();

    showText(topRow, (char *)"Hull Makerspace");
    showText(bottomRow, (char *)"@alifeee");
}

void loop()
{
    topRow.scrollRows(1, true); // wrap scroll left to right
    topRow.display();
    bottomRow.scrollRows(-1, true); // wrap scroll right to left
    bottomRow.display();
}
