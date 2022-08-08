
int IRval=0;
unsigned long IRmillis1=millis();
unsigned long IRmillis2=millis();
unsigned long IRperiod;
unsigned long IRperiodold;
bool newIR=false;

void IRtimer(){
  
  
  IRperiod=millis()-IRmillis1;
  if (IRperiod<40||IRperiod>500){
    IRperiod=IRperiodold;
  }
  newIR=true;
  IRmillis1=millis();
  IRperiodold=IRperiod;
  //if (abs(IRperiod-AccPeriod)>100) IRperiod=AccPeriod
}
void IRsensorSetup(){

  pinMode(19, INPUT);
  attachInterrupt(digitalPinToInterrupt(19), IRtimer, FALLING);
  newIR=true;
}

//void IRprocess(){
//  
//}
void IRreadloop(){

//  
//  IRval=digitalRead(19);
//  IRprocess();
//    delay(10);
//    if (IRperiod>50){
//    Serial.println(IRperiod);
//    }
  
  
}
