#include <Wire.h>
#include <SSD1306Wire.h>
#include "image.h"
SSD1306Wire  display(0x3c, 4, 5);

void setup() {
  // put your setup code here, to run once:
  display.init();
  display.setFont(ArialMT_Plain_24);
  display.flipScreenVertically();
}

void loop() {
  // put your main code here, to run repeatedly:
  display.drawXbm(0,0,128,64,epd_bitmap_twenny_2);
  display.display();
}
