#include <Wire.h>
#include "SSD1306Wire.h"
#include "block_font.h"

SSD1306Wire  display(0x3c, 4, 5);
char timerBuffer[6];
//For debugging only
uint32_t speed = 100;

uint32_t mins = 00;
uint32_t secs = 00;
uint32_t lastMin = mins-1;
uint32_t lastSec = secs;
uint32_t tmin = 20;
int32_t tsec = 00;


void setup() {
  display.init();
  display.setFont(ArialMT_Plain_24);
  display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  Serial.begin(9600);
  pinMode(10, INPUT_PULLUP);
  pinMode(14, OUTPUT);
}

void loop(){
  sprintf(timerBuffer, "%02d:%02d", tmin, tsec);
  display.drawString(64, 32, timerBuffer);
  display.display();

  secs = millis()/speed;
  secs = secs%60;
  mins = millis()/(60*speed);

  //Second Change
  if (lastSec != secs){
    if (lastSec == 00 && tsec == 00){
      tmin -= 1;
    }
    display.clear();
    lastSec = sec;
    tsec -= 1;
    if (tsec == -1){
      tsec = 59;
    }
  }

  //Minute Change
  // if (lastMin != mins){
  //   lastMin = mins;
  //   tmin -= 1;
  // }

  //Button Press
  if (!digitalRead(10)) {
    display.clear();
    tsec = 00;
    tmin = 20;
    lastSec = 00;
    lastMin = mins-1;
  }

  //Beep
  analogWrite(14, (tsec%60==0)*30);
}

