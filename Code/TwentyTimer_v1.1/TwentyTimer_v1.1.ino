#include <Wire.h>
#include <SSD1306Wire.h>
#include "images.h"
#include <EEPROM.h>

SSD1306Wire  display(0x3c, 4, 5);

//DEBUG
uint32_t speed = 1000;

//LOGO
uint8_t randomLogo = 0;

//TIME VARS
char timerBuffer[6];
uint32_t setMins = 20;
uint32_t setSecs = 20;
uint32_t mins = 0;
uint32_t secs = 0;
uint32_t tmins = setMins;
uint32_t tsecs = 0;
uint32_t lastMin = tmins-1;
uint32_t lastSec = -1;
uint32_t sinceLastEvent = 0;
uint32_t tmp = sinceLastEvent;
bool twenny = false;

//CLICR
uint32_t clics = 0;

//CONTROL VARS
bool paused = false;
int state = 0;
int lastState = 1;
int stateOffset = 4;
/*
states: 
0: SLEEP/LOGO
1: 2020 MENU
2: TIMER MENU
3: ADHD COOKIE MENU
stateOffset: 2020 TIMER
stateOffset+1: TIMER
stateOffset+2: ADHD COOKIE
stateOffset+3: SECRET PONG MENU
*/

//GAME VARS
bool running = false;
uint8_t screen = 0;
#define UP_BUTTON 12
#define DOWN_BUTTON 10
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
void drawCourt();
unsigned long PADDLE_RATE = 26;
unsigned long CPU_PADDLE_RATE = 20;
unsigned long BALL_RATE = 20;
const uint8_t PADDLE_HEIGHT = 14;
uint8_t ball_x = 64, ball_y = 32;
uint8_t ball_dir_x = 1, ball_dir_y = 1;
unsigned long ball_update;
unsigned long paddle_update;
unsigned long cpu_paddle_update;
const uint8_t CPU_X = 12;
uint8_t cpu_y = 16;
const uint8_t PLAYER_X = 115;
uint8_t player_y = 16;
bool setUpGame = true;
//SCORE
uint32_t playerPoints = 0;
uint32_t cpuPoints = 0;
char score[5];
uint32_t difficulty = 1;


void setup() {
  //EEPROM
  EEPROM.begin(4);

  //DISPLAY
  display.init();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  display.flipScreenVertically();

  //PINS
  pinMode(10, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  pinMode(14, OUTPUT);
  digitalWrite(UP_BUTTON,1);
  digitalWrite(DOWN_BUTTON,1);

}

void loop() {
  top:

  //--------------------------------------------- STATE 0: SLEEP/LOGO --------------------------------------------- 

  if (state == 0){
    if (millis()-sinceLastEvent<5*speed){
      display.clear();
      switch(randomLogo){
        case 0:
          display.drawXbm(0,0,twennyW,twennyH,xbm_twenny);
          break;
        case 1:
          display.drawXbm(0,0,twennyW,twennyH,xbm_twenny);
          break;
        case 2:
          display.drawXbm(0,0,twennyW,twennyH,xbm_twenny);
          break;
        case 3:
          display.drawXbm(0,0,twennyW,twennyH,xbm_twenny);
          break;
        case 4:
          display.drawXbm(0,0,twennyW,twennyH,xbm_twenny);
          break;
        case 5:
          display.drawXbm(0,0,twennyW,twennyH,xbm_twenny);
          break;
        case 6:
          display.drawXbm(0,0,twennyW,twennyH,xbm_twenny_o_o);
          break;
        case 7:
          display.drawXbm(0,0,twennyW,twennyH,xbm_twenny_FF);
          break;
        case 8:
          display.drawXbm(0,0,twennyW,twennyH,xbm_twenny_x_x);
          break;
        case 9:
          display.drawXbm(0,0,twennyW,twennyH,xbm_twenny_peace);
          break;
        case 10:
          display.drawXbm(0,0,twennyW,twennyH,xbm_twenny_skull);
          break;
        case 11:
          display.drawXbm(0,0,twennyW,twennyH,xbm_twenny_dude);
          break;
      }
      display.display();
    }
    else {
      display.clear();
      if (randomLogo == 10){
        display.drawXbm(0,0,twennyW,twennyH,xbm_inv_twenny_skull);
      } else if (randomLogo == 11){
        display.drawXbm(0,0,twennyW,twennyH,xbm_inv_twenny_dude);
      } else {
        display.drawXbm(0,0,twennyW,twennyH,xbm_inv_twenny);
      }
      display.display();
      if (millis()-sinceLastEvent>10*speed){
        sinceLastEvent = millis();
        randomLogo = random(0,12);
      }
    }

    while(!digitalRead(12) && !digitalRead(10)){
      while(!digitalRead(12) || !digitalRead(10)){
        delay(10);
        if (millis()-tmp>3000) {
          display.clear();
          state = stateOffset+3;
          goto top;
        }
      }
    }
    if (!digitalRead(12) ^ !digitalRead(10)){
      state = lastState;
      sinceLastEvent = millis();
      //find better solution to black screen after time out
      paused = false;
      while(!digitalRead(12) || !digitalRead(10)){
        delay(10);
      }
    }
  } 

  //--------------------------------------------- STATE 1 TO stateOffset-1: CYCLE MENU --------------------------------------------- 

  else if (state >=1 && state < stateOffset){
    display.clear();
    if (state == 1) {
        display.drawXbm(0,0,twennyW,twennyH,xbm_menu_2020_timer2);
        //  display.drawXbm(0,0,twennyW,twennyH,xbm_menu_2020_timer1);
    } else if (state == 2) {
      display.drawXbm(0,0,twennyW,twennyH,xbm_menu_timr);
    } else if (state == 3) {
      display.drawXbm(0,0,twennyW,twennyH,xbm_menu_clicr);
    }
    if (millis()-tmp>750){
      display.drawXbm(118,25,9,14,xbm_right_arrow_w9_h14);
      if (millis()-tmp>1500){
        tmp = millis();
      }
    }
    display.display();

    //EXIT TO SLEEP
    if (!digitalRead(10) || !digitalRead(12)){
      delay(50);
      tmp = millis();

      while(!digitalRead(12) && !digitalRead(10)){
          delay(10);
          if (millis()-tmp>1000) {
            display.clear();
            state = 0;
            goto top;
          }
          while(!digitalRead(10) || !digitalRead(12));
      }

      if (!digitalRead(12)){
        state = (state-1)+stateOffset;
        sinceLastEvent = millis();
        //find better solution to black screen after time out
        paused = false;
        while(!digitalRead(12) || !digitalRead(10)){
          delay(10);
        }
        goto top;
      }

      if (!digitalRead(10)){
        state = (state+1)%stateOffset;
        if (state == 0){
          state = 1;
        }
        while(!digitalRead(12) || !digitalRead(10)){
          delay(10);
        }
      }
    }
    
  }

  //--------------------------------------------- STATE stateOffset: 2020 TIMER --------------------------------------------- 
  else if (state == stateOffset){
    setMins = 20;
    setSecs = 20;

    //UPDATE DISPLAY
    display.clear();
    drawNumber(0,0,4,tmins/10,0);
    drawNumber(30,0,4,tmins%10,0);
    drawNumber(73,0,4,tsecs/10,0);
    drawNumber(103,0,4,tsecs%10,0);
    drawDots(4, 0);
    display.display();

    secs = ((millis()-sinceLastEvent)/speed)%60;
    mins = (millis()-sinceLastEvent)/speed*60;

    //20MIN OVER
    if (tmins == 0 && tsecs == 0){
      if (!twenny){
        tmins = 0;
        tsecs = setSecs;
        twenny = !twenny;
      } else {
        tmins = setMins;
        tsecs = 0;
        twenny = !twenny;
      }
      sinceLastEvent = millis();
      analogWrite(14, 150);
      delay(50);
      analogWrite(14, 0);
      delay(50);
      analogWrite(14, 20);
      delay(50);
      analogWrite(14, 0);
    }

    //TIME CALC
    if (lastSec != secs){
      lastSec = secs;
      if (tsecs==0 && lastSec==secs){
        tmins -= 1;
      }
      tsecs = tsecs-1;
      if (tsecs == -1){
        tsecs = 59;
      }
      display.clear();
    }

    //BUTTON PRESS
    if (!digitalRead(12) || !digitalRead(10) || paused){
      delay(50);
      if (!digitalRead(12) && !digitalRead(10)){
        tmp = millis();
        while(!digitalRead(12) && !digitalRead(10)) {
          if (millis()-tmp>1000){
            state = state-stateOffset+1;
            goto top;
          }
        }
        while(!digitalRead(12) || !digitalRead(10));
      }

      //RESET BUTTON
      if (!digitalRead(10)) {
        display.clear();
        tsecs = 00;
        tmins = setMins;
        lastSec = 00;
        lastMin = mins-1;
        sinceLastEvent = millis();
        while (digitalRead(10)){
          delay(10);
        }
      }
      //PLAY/PAUSE TOGGLE
      do {
        int weightAnimation;
        bool wasPaused = paused;
        if (!digitalRead(12)) {
          weightAnimation = 4;
          paused = !paused;
          tmp = millis()-sinceLastEvent;
          sinceLastEvent = millis();
          while (!digitalRead(12)){
            delay(10);
          }
        }

        if (!digitalRead(12) && !digitalRead(10)){
            tmp = millis();
            while(!digitalRead(12) && !digitalRead(10)) {
              if (millis()-tmp>1000){
                state = state-stateOffset;
                goto top;
              }
            }
          }

        if (paused) {
          display.clear();
          drawNumber(0,0,weightAnimation,tmins/10,0);
          drawNumber(30,0,weightAnimation,tmins%10,0);
          drawNumber(73,0,weightAnimation,tsecs/10,0);
          drawNumber(103,0,weightAnimation,tsecs%10,0);
          drawDots(weightAnimation, 0);
          display.display();
          if (weightAnimation > 1) {weightAnimation -= 1;}

          if (!digitalRead(10)) {
            display.clear();
            tsecs = 0;
            tmins = setMins;
            lastSec = 0;
            lastMin = mins-1;
            sinceLastEvent = millis();
            while (digitalRead(10)){
              delay(10);
            }
          }
          if (millis()-sinceLastEvent>20*speed){
            state = 0;
            goto top;
          }
          delay(25);
        }
        if (wasPaused == true && paused == false){
          sinceLastEvent = millis()-tmp;
          lastSec = tsecs;
        }
      } while (paused);
    }
    //BEEP
    //analogWrite(14, (tsecs%60==0)*30);
  }

  //--------------------------------------------- STATE stateOffset+1: BORING TIMER --------------------------------------------- 
  else if (state == stateOffset+1) {
    setMins = 0;
    setSecs = 0;
    running = false;
    int selDigit = 0;

    timer:
    //SET TIMER
    tmins = setMins;
    tsecs = setSecs;

    while (!running) {
      display.clear();
      //SELECT FIRST DIGIT:
      switch (selDigit) {
        case 0:
          drawNumber(0,0,4,tmins/10,0);
          drawNumber(30,0,4,tmins%10,1);
          drawNumber(73,0,4,tsecs/10,1);
          drawNumber(103,0,4,tsecs%10,1);
          break;
        case 1:
          drawNumber(0,0,4,tmins/10,1);
          drawNumber(30,0,4,tmins%10,0);
          drawNumber(73,0,4,tsecs/10,1);
          drawNumber(103,0,4,tsecs%10,1); 
          break;
        case 2:
          drawNumber(0,0,4,tmins/10,1);
          drawNumber(30,0,4,tmins%10,1);
          drawNumber(73,0,4,tsecs/10,0);
          drawNumber(103,0,4,tsecs%10,1);
          break;
        case 3:
          drawNumber(0,0,4,tmins/10,1);
          drawNumber(30,0,4,tmins%10,1);
          drawNumber(73,0,4,tsecs/10,1);
          drawNumber(103,0,4,tsecs%10,0); 
          break;
      }
      drawDots(4, 1);
      display.display();

      if (!digitalRead(10) || !digitalRead(12)){
        delay(50);
        tmp = millis();
        if (!digitalRead(12) && !digitalRead(10)){
          while(!digitalRead(10) && !digitalRead(12)) {
            if (millis()-tmp > 1000){
              state = state-stateOffset+1;
              goto top;
            }
          }
          while(!digitalRead(10) || !digitalRead(12)) {
            delay(10);
          }
          selDigit = (selDigit-1)%4;
          if (selDigit==-1){
            selDigit = 3;
          }
        }

        if (!digitalRead(12)) {
          while (!digitalRead(12)) {
            delay(10);
          }
          selDigit = selDigit + 1;
          if (selDigit==4) {
            selDigit = 0;
            setMins = tmins;
            setSecs = tsecs;
            running = true;
            goto timer;
          }
        } else if (!digitalRead(10)) {
          switch (selDigit) {
            case 0:
              tmins = (tmins+10)%100;
              break;
            case 1:
              tmins = (tmins+1)%100;
              break;
            case 2:
              tsecs = (tsecs+10)%60;
              break;
            case 3:
              tsecs = (tsecs+1)%60;
              break;
          }
          while (!digitalRead(10)){
            delay(10);
          }
        }
        
      }
    } 
    while (running) {
      //DRAW TIME
      display.clear();
      drawNumber(0,0,4,tmins/10,0);
      drawNumber(30,0,4,tmins%10,0);
      drawNumber(73,0,4,tsecs/10,0);
      drawNumber(103,0,4,tsecs%10,0);
      drawDots(4, 0);
      display.display();

      secs = ((millis()-sinceLastEvent)/speed)%60;
      mins = (millis()-sinceLastEvent)/speed*60;

      //TIMER DONE
      if (tmins == 0 && tsecs == 0){
        tmp = millis();
        while (millis()-tmp<60000 && digitalRead(12) && digitalRead(10)){
          //BEEP
          if (millis()%1000<50){
            analogWrite(14,100);
          }
          analogWrite(14,0);
        }

        if (!digitalRead(10)) {
          //RESET
          tmins = setMins;
          tsecs = setSecs;

        } else if (!digitalRead(12)) {
          //EXIT TO SET TIMER
          running = false;
          while(!digitalRead(12)){
            delay(10);
          }
          goto timer;
        }

        while(!digitalRead(12) || !digitalRead(10)){
          delay(10);
        }
      
      }

      //SOME LAZY CODING...
      //TIME CALC
      if (lastSec != secs){
        lastSec = secs;
        if (tsecs==0 && lastSec==secs){
          tmins -= 1;
        }
        tsecs = tsecs-1;
        if (tsecs == -1){
          tsecs = 59;
        }
        display.clear();
      }

      //BUTTON PRESS
      if (!digitalRead(12) || !digitalRead(10) || paused){
        delay(50);
        if (!digitalRead(12) && !digitalRead(10)){
          tmp = millis();
          while(!digitalRead(12) && !digitalRead(10)) {
            if (millis()-tmp>1000){
              state = state-stateOffset+1;
              goto top;
            }
          }
          while(!digitalRead(12) || !digitalRead(10));
          running = false;
          goto timer;
        }

        //RESET BUTTON
        if (!digitalRead(10)) {
          display.clear();
          tsecs = setSecs;
          tmins = setMins;
          lastSec = 0;
          lastMin = mins-1;
          sinceLastEvent = millis();
          while (digitalRead(10)){
            delay(10);
          }
        }
        //PLAY/PAUSE TOGGLE
        do {
          int weightAnimation;
          bool wasPaused = paused;
          if (!digitalRead(12)) {
            weightAnimation = 4;
            paused = !paused;
            tmp = millis()-sinceLastEvent;
            sinceLastEvent = millis();
            while (!digitalRead(12)){
              delay(10);
            }
          }

          if (paused) {
            display.clear();
            drawNumber(0,0,weightAnimation,tmins/10,0);
            drawNumber(30,0,weightAnimation,tmins%10,0);
            drawNumber(73,0,weightAnimation,tsecs/10,0);
            drawNumber(103,0,weightAnimation,tsecs%10,0);
            drawDots(weightAnimation, 0);
            display.display();
            if (weightAnimation > 1) {weightAnimation -= 1;}

            if (!digitalRead(10)) {
              display.clear();
              tsecs = 0;
              tmins = setMins;
              lastSec = 0;
              lastMin = mins-1;
              sinceLastEvent = millis();
              while (digitalRead(10)){
                delay(10);
              }
            }
            if (millis()-sinceLastEvent>20*speed){
              state = 0;
              goto top;
            }
            delay(25);
          }
          if (wasPaused == true && paused == false){
            sinceLastEvent = millis()-tmp;
            lastSec = tsecs;
          }
        } while (paused);
      }

    }


  }

  //--------------------------------------------- STATE stateOffset+2: CLICR --------------------------------------------- 

  else if (state == stateOffset+2){
    tmp = millis();   
    bool clic1 = false;
    bool clic2 = false;
    bool both = false;
    bool aUnlock = false;
    char buffer[20];

    byte byte1 = EEPROM.read(0); //Least significant byte
    byte byte2 = EEPROM.read(1);
    byte byte3 = EEPROM.read(2);
    byte byte4 = EEPROM.read(3); //Most significant byte

    clics = byte4;
    clics = clics << 8;
    clics += byte3;
    clics = clics << 8;
    clics = byte2;
    clics = clics << 8;
    clics += byte1;

    int acheivementNum = log2(clics);

    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);

    while (true) {
      //DISPLAY CONTENT
      display.clear();
      if (!aUnlock) {
        //DISPLAY CLICS
        sprintf(buffer, "CLICS: %012d", clics);
        display.drawString(64, 32, buffer);
      } else {
        //DISPLAY ACHEIVEMENT + IMG

        //CAN BE OPTIMIZED A LOT!!!!!
        spintf(buffer, "ACHEIVEMENT %02d", acheivementNum);
        switch(acheivementNum){
          case 6:
            buffer = buffer + "achvDesc"
            break;
        }
        display.drawString(64, 40, buffer)
      }
      display.display();

      //ACHEIVEMENTS
      if (acheivementNum != int(log2(clics))) {
        acheivementNum = int(log2(clics));
        //ACHEIVEMETS START AT 2^6
        if (acheivementNum>=6){
          aUnlock = true;
          sinceLastEvent = millis();
        }
      }

      if (millis()-sinceLastEvent > 1500) { //TIMEOUT OF ACHEIVEMENT DISPLAY
        aUnlock = false;
      }


      if (!digitalRead(10) && !clic1) {
        clics += 1;
        clic1 = true;
      }
      if (!digitalRead(12) && !clic2) {
        clics += 1;
        clic2 = true;
      }

      if (digitalRead(10)) {
        clic1 = false;
      }
      if (digitalRead(12)) {
        clic2 = false;
      }
      if (clic1 && clic2 && !both) {
        both = true;
        tmp = millis();
      } 
      if ((!clic1 || !clic2) && both){
        both = false;
      }

      if (both && millis()-tmp > 1000) {
        display.clear();
        byte4 = clics >> 24;
        byte3 = clics >> 16;
        byte2 = clics >> 8;
        byte1 = clics;

        EEPROM.write(0, byte1);
        EEPROM.commit();
        EEPROM.write(1, byte2);
        EEPROM.commit();
        EEPROM.write(2, byte3);
        EEPROM.commit();
        EEPROM.write(3, byte4);
        EEPROM.commit();

        state = state-stateOffset+1;
        goto top;
      }
    }

  }


  //--------------------------------------------- STATE stateOffset+3: PONG --------------------------------------------- 

  else if (state == stateOffset+3){
    setUpGame = true;
    int exitTimer = 0;
    if (!running) {
          setUpGame = true;
          switch(screen) {
            case 0:
              display.drawXbm(0,0,128,64,xbm_menu_pong1);
              break;
            case 1:
              display.drawXbm(0,0,128,64,xbm_menu_pong2);
              break;
            case 2:
              display.drawXbm(0,0,128,64,xbm_menu_pong3);
              break;
            case 3:
              display.drawXbm(0,0,128,64,xbm_menu_pong4);
              break;
            case 4:
              display.drawXbm(0,0,128,64,xbm_menu_pong5);
              break;
            case 5:
              display.drawXbm(0,0,128,64,xbm_menu_pong6);
              break;
            case 6:
              display.drawXbm(0,0,128,64,xbm_menu_pong7);
              break;
          }
          display.display();
      if (millis()-sinceLastEvent>5000){
        display.clear();
        sinceLastEvent = millis();
        screen = (screen+1)%7;
      }
    }
    if (!digitalRead(12) || !digitalRead(10)){
      delay(50);
    //EXIT
      tmp = millis();
      while(!digitalRead(12) && !digitalRead(10)){
        while(!digitalRead(12) || !digitalRead(10)){
          delay(10);
          if (millis()-tmp>3000) {
            state = 1;
            sinceLastEvent = millis();
            goto top;
          }
        }
      }
      if (!digitalRead(12) ^ !digitalRead(10)){
        display.clear();
        running = true;
      }
    }
    //RUNNING PONG
    while (running){
      //* original core logic (basic pong, no score, dumb cpu) by shveytank on github: https://github.com/shveytank/Arduino_Pong_Game
      if (setUpGame) {
        unsigned long start = millis();
        display.clear();
        drawCourt();

        display.display();

        ball_update = millis();
        paddle_update = ball_update;
        cpu_paddle_update = paddle_update;
        cpuPoints = 0;
        playerPoints = 0;
        ball_x = 64;
        ball_y = 32;

        sprintf(score, "%02d %02d", cpuPoints, playerPoints);
        setUpGame = !setUpGame;
      }

      bool update = false;
      unsigned long time = millis();

      static bool up_state = false;
      static bool down_state = false;
      
      up_state |= (digitalRead(UP_BUTTON) == LOW);
      down_state |= (digitalRead(DOWN_BUTTON) == LOW);
      if (up_state && down_state) {
        exitTimer += 1;
        if (exitTimer > 100000){
          display.clear();
          running = false;
        }
      } else if (up_state ^ down_state || !(up_state && down_state)) {
        exitTimer = 0;
      }

      difficulty = playerPoints/5+1;
      BALL_RATE = 19-(difficulty-1);
      PADDLE_RATE = 26-(difficulty-1);
      CPU_PADDLE_RATE = 24-(difficulty-1);

      if (playerPoints == 69 || cpuPoints == 69){
        display.clear();
        display.drawString(64,32, "stop it. get some help.");
        display.display();
        delay(3000);
        state = 1;
        sinceLastEvent = millis();
        goto top;
      }


      if(time > ball_update) {
          uint8_t new_x = ball_x + ball_dir_x;
          uint8_t new_y = ball_y + ball_dir_y;

          // Check if we hit the vertical walls
          if(new_x == 0 || new_x == 127) {
              display.setColor(BLACK);
              display.drawString(64, 6, score);
              display.setColor(WHITE);
              if (new_x == 0) {
                playerPoints += 1;
              } else {
                cpuPoints += 1;
              }
              sprintf(score, "%02d %02d", cpuPoints, playerPoints);
              display.drawString(64, 6, score);
              ball_update += 500;
              new_x = 64;
              new_y = random(10,54);
              ball_dir_x = -ball_dir_x;
          }

          // Check if we hit the horizontal walls.
          if(new_y == 0 || new_y == 63) {
              ball_dir_y = -ball_dir_y;
              new_y += ball_dir_y + ball_dir_y;
          }

          // Check if we hit the CPU paddle
          if(new_x == CPU_X && new_y >= cpu_y && new_y <= cpu_y + PADDLE_HEIGHT) {
              ball_dir_x = -ball_dir_x;
              new_x += ball_dir_x + ball_dir_x;
          }

          // Check if we hit the player paddle
          if(new_x == PLAYER_X
            && new_y >= player_y
            && new_y <= player_y + PADDLE_HEIGHT)
          {
              ball_dir_x = -ball_dir_x;
              new_x += ball_dir_x + ball_dir_x;
          }
          display.setColor(BLACK);
          display.setPixel(ball_x, ball_y);
          display.setColor(WHITE);
          display.setPixel(new_x, new_y);

          display.drawString(64, 6, score);
          display.drawRect(49, 0, 31, 13);
          display.drawLine(64,0,64,12);

          ball_x = new_x;
          ball_y = new_y;

          ball_update += BALL_RATE;

          update = true;
      }

      if(time > cpu_paddle_update) {
          cpu_paddle_update += CPU_PADDLE_RATE;

          // CPU paddle
          display.setColor(BLACK);
          display.drawLine(CPU_X, cpu_y, CPU_X, cpu_y+PADDLE_HEIGHT);
          display.setColor(WHITE);
          const uint8_t half_paddle = PADDLE_HEIGHT >> 1;
          if(cpu_y + half_paddle > ball_y) {
              cpu_y -= 1;
          }
          if(cpu_y + half_paddle < ball_y) {
              cpu_y += 1;
          }
          if(cpu_y < 1) cpu_y = 1;
          if(cpu_y + PADDLE_HEIGHT > 62) cpu_y = 62 - PADDLE_HEIGHT;
          display.drawLine(CPU_X, cpu_y, CPU_X, cpu_y+PADDLE_HEIGHT);
      }

      if(time > paddle_update) {
          paddle_update += PADDLE_RATE;
          // Player paddle
          display.setColor(BLACK);
          display.drawLine(PLAYER_X, player_y, PLAYER_X, player_y+PADDLE_HEIGHT);
          display.setColor(WHITE);
          if(up_state) {
              player_y -= 1;
          }
          if(down_state) {
              player_y += 1;
          }
          up_state = down_state = false;
          if(player_y < 1) player_y = 1;
          if(player_y + PADDLE_HEIGHT > 62) player_y = 62 - PADDLE_HEIGHT;

          display.drawLine(PLAYER_X, player_y, PLAYER_X, player_y+PADDLE_HEIGHT);

          update = true;
      }

      if(update) 
          display.display();
    } 
  }
}

void drawCourt() {
    display.drawRect(0, 0, 128, 64);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                            //
//                                                                                                            //
//                                        DON'T GO BEYOND THIS POINT                                          //
//                                            NOTHING GOOD AHEAD                                              //
//                                                                                                            //
//                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//---------------------------------------------- GREYS BLOCK FONT ----------------------------------------------
void drawDigit(int x, int y, int weight, bool s1, bool s2, bool s3, bool s4, bool s5, bool s6, bool s7) {
  if (s1){
  display.fillRect(x+4-weight,y+4-weight,17+weight*2,weight*2);
  }if (s2){
  display.fillRect(x+4-weight,y+24+4-weight,17+weight*2,weight*2);
  }if (s3){
  display.fillRect(x+4-weight,y+56+4-weight,17+weight*2,weight*2);
  }if (s4){
  display.fillRect(x+4-weight,y+4-weight,weight*2,24+weight*2);
  }if (s5){
  display.fillRect(x+17+4-weight,y+4-weight,weight*2,24+weight*2);
  }if (s6){
  display.fillRect(x+4-weight,y+24+4-weight,weight*2,32+weight*2);
  }if (s7){
  display.fillRect(x+17+4-weight,y+24+4-weight,weight*2,32+weight*2);
  }
}

void drawHollowDigit(int x, int y, int weight, bool s1, bool s2, bool s3, bool s4, bool s5, bool s6, bool s7) {
  if (s1){
  display.drawRect(x+4-weight,y+4-weight,17+weight*2,weight*2);
  }if (s2){
  display.drawRect(x+4-weight,y+24+4-weight,17+weight*2,weight*2);
  }if (s3){
  display.drawRect(x+4-weight,y+56+4-weight,17+weight*2,weight*2);
  }if (s4){
  display.drawRect(x+4-weight,y+4-weight,weight*2,24+weight*2);
  }if (s5){
  display.drawRect(x+17+4-weight,y+4-weight,weight*2,24+weight*2);
  }if (s6){
  display.drawRect(x+4-weight,y+24+4-weight,weight*2,32+weight*2);
  }if (s7){
  display.drawRect(x+17+4-weight,y+24+4-weight,weight*2,32+weight*2);
  }
}

void drawNumber(int x, int y, int weight, int num, bool hollow){
  if (!hollow){
    switch(num){
      case 0:
        drawDigit(x, y, weight, 1, 0, 1, 1, 1, 1, 1);
        return;
      case 1:
        drawDigit(x, y, weight, 0, 0, 0, 0, 1, 0, 1);
        return;
      case 2:
        drawDigit(x, y, weight, 1, 1, 1, 0, 1, 1, 0);
        return;
      case 3:
        drawDigit(x, y, weight, 1, 1, 1, 0, 1, 0, 1);
        return;
      case 4:
        drawDigit(x, y, weight, 0, 1, 0, 1, 1, 0, 1);
        return;
      case 5:
        drawDigit(x, y, weight, 1, 1, 1, 1, 0, 0, 1);
        return;
      case 6:
        drawDigit(x, y, weight, 0, 1, 1, 1, 0, 1, 1);
        return;
      case 7:
        drawDigit(x, y, weight, 1, 0, 0, 0, 1, 0, 1);
        return;
      case 8:
        drawDigit(x, y, weight, 1, 1, 1, 1, 1, 1, 1);
        return;
      case 9:
        drawDigit(x, y, weight, 1, 1, 1, 1, 1, 0, 1);
        return;
    }
  } else {
    switch(num){
      case 0:
        drawHollowDigit(x, y, weight, 1, 0, 1, 1, 1, 1, 1);
        return;
      case 1:
        drawHollowDigit(x, y, weight, 0, 0, 0, 0, 1, 0, 1);
        return;
      case 2:
        drawHollowDigit(x, y, weight, 1, 1, 1, 0, 1, 1, 0);
        return;
      case 3:
        drawHollowDigit(x, y, weight, 1, 1, 1, 0, 1, 0, 1);
        return;
      case 4:
        drawHollowDigit(x, y, weight, 0, 1, 0, 1, 1, 0, 1);
        return;
      case 5:
        drawHollowDigit(x, y, weight, 1, 1, 1, 1, 0, 0, 1);
        return;
      case 6:
        drawHollowDigit(x, y, weight, 0, 1, 1, 1, 0, 1, 1);
        return;
      case 7:
        drawHollowDigit(x, y, weight, 1, 0, 0, 0, 1, 0, 1);
        return;
      case 8:
        drawHollowDigit(x, y, weight, 1, 1, 1, 1, 1, 1, 1);
        return;
      case 9:
        drawHollowDigit(x, y, weight, 1, 1, 1, 1, 1, 0, 1);
        return;
    }
  }
}

void drawDots(int weight, bool hollow){
  if (!hollow) {
    display.fillRect(60+4-weight,12+4-weight,weight*2,8+weight*2);
    display.fillRect(60+4-weight,36+4-weight,weight*2,8+weight*2);
  }
  else {
    display.drawRect(60+4-weight,12+4-weight,weight*2,8+weight*2);
    display.drawRect(60+4-weight,36+4-weight,weight*2,8+weight*2);
  }
}
