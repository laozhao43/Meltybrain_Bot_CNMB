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

//#include <CPPM-RX.h>
#include "FlySkyIBus.h"
bool newInput = false;
uint16_t throt = 0;
uint16_t rudd = 500;
uint16_t elev = 500;
uint16_t ailer = 500;
uint16_t gear = 500;
uint16_t flap = 500;

unsigned long failSafeTimer;
unsigned long failSafeSec=0;


uint16_t thOld=0;

//limit throttle pulse widths to 1000-2000us
//int readChannel(byte channelInput, int minLimit, int maxLimit, int defaultValue){
//  uint16_t ch =IBus.readChannel(channelInput);
//  //if (ch<1520&&ch>1480)  return defaultValue;
//  return map(ch, 950, 2050, minLimit, maxLimit);
//}   
int ch1v, ch2v, ch3v, ch4v, ch5v, ch6v, chnv;

void readReceiver(){
  ch1v = IBus.readChannel(0);
  ch2v = IBus.readChannel(1);
  ch3v = IBus.readChannel(2);
  ch4v = IBus.readChannel(3);
  ch5v = IBus.readChannel(4);
  ch6v = IBus.readChannel(5);
  chnv = IBus.readChannel(10);
  throt=max(1000, min(2000, ch3v))-1000;
  elev=max(1000, min(2000, ch2v))-1000;
  ailer=max(1000, min(2000, ch1v))-1000;
  rudd=max(1000, min(2000, ch4v))-1000;
  flap = max(1000, min(2000, ch5v))-1000;
  gear=max(1000, min(2000, ch6v))-1000;

  if (millis() - failSafeTimer > 1000) {
    failSafeTimer = millis();
    if (thOld==throt){
      failSafeSec++;
    }
    else{
      failSafeSec=0;
    }
    thOld=throt;
    Serial.println(failSafeSec);
  }
}

bool signalLost(){
  if (flap<650) return true;
  else if (failSafeSec>10) return true;
  else if (flap>650) return false;
  
  
  
}
//void readReceiver()
//{
//  noInterrupts();
// newInput = true;
//  
//  //get receiver inputs as a percentage of max (assuming 1000-2000 us pulse range)
//  throt = (return_THROT()-1000);
//  rudd = (return_RUDD()-1000);
//  elev = (return_ELEV()-1000);
//  ailer = (return_AILER()-1000);
//  gear = (return_GEAR()-1000);
//  flap = (return_FLAP()-1000);
//  interrupts();
//}
