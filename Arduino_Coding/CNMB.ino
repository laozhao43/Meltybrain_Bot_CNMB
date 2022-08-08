/*SpinDoctor - Software for sensorless translational drift
Copyright (C) 2020  AmbientChaos

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.*/
#include <FastLED.h>
#define LED_PIN     23
#define NUM_LEDS    8
CRGB leds[NUM_LEDS];

#include "DShot.h"

#include "Watchdog.h"
#include "adxlacc.h"
#include "Melty.h"
#include "IRsensor.h"
#include "bluedata.h"



//define pins
//#define TELEM 0
//#define RX_CPPM 2
#define GREEN 4
#define RED 6
#define ESC_1 9 //FTM0_CH2
#define ESC_2 10 //FTM0_CH3
#define ACCEL_SCL 19
#define ACCEL_SDA 18
#define HWSERIAL Serial1

//define states
#define STATE_IDLE 0
#define STATE_DRIVE 1
#define STATE_JUKE 2
#define STATE_MELTY 3
#define STATE_MAX_SPIN 4
#define STATE_CALIBRATE 5
byte state = STATE_IDLE;
byte prevState = STATE_IDLE;

unsigned long blinkTimer = millis();
bool blinkBlock = false;
unsigned long curmillis = millis();
unsigned long premillis = millis();
int ledState=LOW;

void setup() {
  
  
  bluesetup();
  accsetup() ;
  IRsensorSetup();
  //StartCPPM(RX_CPPM);
  Serial1.begin(115200); // open Serial1 for ESC telemetry
  IBus.begin(Serial1);
  //Serial1.clear();
  readCalibration();
  
  
  //setup ESC DShot out
  pinMode(ESC_1, OUTPUT);
  digitalWriteFast(ESC_1, LOW);
  pinMode(ESC_2, OUTPUT);
  digitalWriteFast(ESC_2, LOW);
  delay(500);
  setupDshotDMA();
  dshotOut(0, 1); //arm motor
  delay(100);
  dshotOut(0, 2); //arm motor
  delay(100);
  //setup watchdog
  watchdogSetup();

  state = STATE_IDLE;
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  
  leds[4] = CRGB(30, 255, 100);
  
    leds[4] = CRGB(0, 0, 0);
    
  FastLED.show();
  //FastLED.setMaxRefreshRate(0, true); 
  
}
float accvalue;

void loop() {
  IRreadloop();
  readaccvalue() ;
  bluesend();
  
    //Serial.println(EMA_a);
    //altSerial.print(EMA_a);
    //bluesend();
//    Serial1.print('1');
//    delay(1);
//    Serial1.print('2');
//    delay(1);
//    Serial1.print('3');
  
  
//  if (IRval==1) leds[4] = CRGB(0, 0, 0);
//  else if (IRval==0) leds[4] = CRGB(30, 255, 100);
  //curmillis=millis();
  if (millis()-premillis>3){
    
    premillis=millis();
    FastLED.show();
  }
  
  IBus.loop();
  readReceiver();
  feedWatchdog();
  

    //readall();
    stateChange(); //check for state changes
  

  //if (telemNew && !telemProcessed) processTelemetry(); // check for new ESC telemetry

  switch (state) {
    case STATE_IDLE:
      //fast blink green status light to warn of improper startup switches or lost signal
      
      leds[0] = CRGB(30, 255, 100);
      setMotor(0, 1);
      setMotor(0, 2);
      //Serial.println("idle");
      break;

//    case STATE_DRIVE:
//      //green LED solid to show drive mode
//      //red LED solid with throttle high to show juke maneuver on entering melty mode (if 2 wheels)
//      drive();
//      
//      break;

//    case STATE_JUKE:
//      // red light while executing juke maneuver
//      jukeFinished = false;
//      jukeStartTime = millis();
//      juke();
//      if (jukeFinished) state = STATE_MELTY;
//      break;
    
    case STATE_MELTY:
      //green light indicates 0 degree heading/forward
      //red light indicates commanded direction of travel
      
      if (throt < 100) { // drive mode if throttle is off
        drive();
        
        //Serial.println("drive");
        if (millis()-blinkTimer>300){
          blinkTimer=millis();
          if (ledState==HIGH){
          ledState = LOW;
          leds[0] = CRGB(0, 0, 0);
          leds[6] = CRGB(0, 0, 0);
          break;
          }
          if (ledState==LOW){
          ledState = HIGH;
          leds[0] = CRGB(255, 50, 100);
          }
        }
        break;
      }
      leds[0] = CRGB(255, 50, 100);
      maxSpin = false;
      calibrating = false;
      runMelty();
      //Serial.println("melty");
      break;
/*
    case STATE_MAX_SPIN:
      //LEDs blinking to show max spin mode
      maxSpin = true;
      calibrating = false;
      runMelty();
      if (millis() - blinkTimer > 150) { // cycle melty LEDs at ~3Hz
        blinkTimer = millis();
        blinkBlock = !blinkBlock;
      }
      if (blinkBlock) {
        digitalWriteFast(GREEN, LOW);
        digitalWriteFast(RED, LOW);        
      }
      break;

    case STATE_CALIBRATE:
      //red LED solid heading to show calibration mode
      maxSpin = false;
      calibrating = true;
      ailer = 500;
      elev = 500;
      runMelty();
      digitalWriteFast(RED, digitalReadFast(GREEN));
      break;

    default:
      break;
*/
  }
}

void stateChange() {
  //check switches and throttle to determine program state to transition to
  prevState = state;
  //safe state if transmitter connection is lost
  if (signalLost()) state = STATE_IDLE;
  //drive if both switches off
  //else if (flap >650 ) state = STATE_DRIVE;
  //melty if flap switch is on and throttle is up
  
  else if (flap > 650) state = STATE_MELTY;
  //max spin if both switches on
  /*
  else if (flap > 550 && gear > 550) state = STATE_MAX_SPIN;
  //calibrate mode if gear switch is on
  else if (flap < 450 && gear > 550) state = STATE_CALIBRATE;

  //check state transitions
  //idle can only exit to drive when both switches off and throttle is down
  if (prevState == STATE_IDLE && state == STATE_DRIVE && throt > 150) state = STATE_IDLE;
  //idle can only exit to drive mode
  else if (prevState == STATE_IDLE && state != STATE_DRIVE) state = STATE_IDLE;
  //entering melty mode with throttle high initiates a juke maneuver
  else if (prevState == STATE_DRIVE && state == STATE_MELTY && throt > 500) state = STATE_JUKE;
  //entering melty mode with rudder high flips the controks for inverted driving
  else if (prevState == STATE_DRIVE && state == STATE_MELTY && rudd > 750) flipped = flipped < 0 ? 1 : -1;
  //Max spin mode cannot be entered at low throttle to prevent accidents
  else if (prevState == STATE_MELTY && state == STATE_MAX_SPIN && throt < 250) state = STATE_MELTY;
  //max spin mode can only exit to melty mode or drive (or idle)
  else if (prevState == STATE_MAX_SPIN && state != STATE_MAX_SPIN) {
    if (state == STATE_MELTY || state == STATE_DRIVE || state == STATE_IDLE);
    else state = STATE_MAX_SPIN;
  }
  //calibrate mode can only exit to drive mode (or idle), saving calibration to EEPROM
  else if (prevState == STATE_CALIBRATE && state != STATE_CALIBRATE) {
    if (state == STATE_DRIVE || state == STATE_IDLE) writeCalibration();
    else state = STATE_MAX_SPIN;
  }
  else if (state == STATE_CALIBRATE && elev >= 850) periodOffset = 0;
  */
}
