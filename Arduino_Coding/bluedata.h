//#include "adxlacc.h"
//unsigned long bluemillis;
void bluesetup() {
  //Serial.begin(9600);
  //Serial.begin(9600);
  //Serial.println("AltSoftSerial Test Begin");
  //Serial1.begin(9600);
}
float a=1;
int b=2;
char c='1';
//String Stringd='12345';
unsigned long bluemillis;
unsigned long bluemillis2;
void bluesend() {

  if (millis()-bluemillis>200){
    if (i==0) accdatablue=String(EMA_S)+String(' ')+String(IRperiod);
    
    if (i<sizeof(accdatablue)&&millis()-bluemillis2>2){
      
      Serial1.print(accdatablue[i]);
      
      i++;
      bluemillis2=millis();
    }
    if (i==sizeof(accdatablue)&&millis()-bluemillis2>2) {     
      i=0;
      Serial1.print('\n');
      bluemillis=millis();
      bluemillis2=millis();
    
}
  }
}
