#ifndef DISPLAY_H
#define DISPLAY_H

#include <Wire.h>
#include "SSD1306Wire.h"
// OLED display object (I2C)
SSD1306Wire display(0x3C, SDA, SCL);


// OLED Display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

void DisplayText2(String text1, int x1, int y1,String text2,int x2,int y2) {
    display.clear();
    display.drawString(x1, y1, text1);
    display.drawString(x2, y2, text2);
    display.display();
}

void DisplayText(String text, int x, int y) {
    display.clear();
    display.drawString(x, y, text);
    display.display();
}

#endif // DISPLAY_H