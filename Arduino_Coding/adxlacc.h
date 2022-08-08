#define ACCSERIAL Serial3
char accchar=' ';
void accsetup() 
{
  //Serial.begin(9600);
  //Serial.println("ready");
  ACCSERIAL.begin(9600);
}
int i;
    String datause;
    String accdatablue;
long datacounter;
String Xstring;
char Xarray[8];
char XarrayFinal[8];
int index1=0;
int charcounter;
float XFloat;
int Xold;
int Xnew;
int filter1counter;
float EMA_a = 0.5;      //initialization of EMA alpha
int EMA_S = 0; 
int AccOffset=42;
unsigned long accmillis;
uint16_t AccPeriod[2];
bool AccProcessed=true;
unsigned long AccTime[2];
void processAcc(){
  
  XFloat = Xstring.toFloat();
  Xold=Xnew;
  Xnew=int(100*XFloat);//convert to int
  Xnew=-Xnew;
    if (abs(Xold-Xnew)>500 && filter1counter<2) {//filter any unrealistic spikes
      Xnew=Xold;
      filter1counter++;
    }
    else filter1counter=0;
  Xnew=Xnew+AccOffset; //apply offset value    
  EMA_S = (EMA_a*Xnew) + ((1-EMA_a)*EMA_S);//apply exponential moving average filter
  //calculate period
  AccPeriod[0] = (uint32_t) (720 / sqrt((double) (EMA_S+121)/452));
  //calculate RPM
  
  

//  AccTime[1] = AccTime[0];
//  AccPeriod[1] = AccPeriod[0];
//  AccTime[0]=micros();
  AccProcessed=true;
  //Serial.println(EMA_S);
}

void readaccvalue() {
  //if (millis()-accmillis>10){
  if(ACCSERIAL.available())
  {
    accchar=ACCSERIAL.read();
    
    //detect 'x' and seperate the value behind it
    if (accchar=='X') {
      Xstring=""; 
      charcounter=-2;
      }
    charcounter++;
    if (accchar=='G') {
      charcounter=-99;
    }
    if (Xstring.length()<8 & charcounter>0) {
      Xstring+=accchar; 
      Xarray[charcounter]=accchar;
    }
    //when lined end, convert to float
    if (accchar=='\n') {
      //AccProcessed=false;
      processAcc();
    }

  }
}
