/*
Use Notepad++ to get better visual understanding of the code. Use Language -> C for better visual.
Alaway never foget to set the following fuses on ATMEGA
Fuses:

Extended: 0x05
High  : 0xDE
LOW   : 0xFF

If these fuse are not set, it might use the internal oscillator
by default and the timing could be wrong.
*/

#include <Wire.h> //for getRTCTime
#define DS3231_I2C_ADDRESS 0x68
#define waitingtime 100
#include <math.h>
int buttonState1;           
int buttonState2;           
int lastButtonState1 = LOW;
int lastButtonState2 = LOW;
unsigned long lastDebounceTime1 = 0;
unsigned long lastDebounceTime2 = 0;
unsigned long debounceDelay1 = 100; 
unsigned long debounceDelay2 = 100; 


// Display output pin assignments#define MTIEN      Display1=Display1 | (1<<0)  
#define MTIEN      Display1=Display1 | (1<<0)  
#define HALF      Display1=Display1 | (1<<1)
#define KWART   Display1=Display1 | (1<<2)
#define TWENTY    Display1=Display1 | (1<<3)
#define OVER     Display1=Display1 | (1<<4)
#define MINUTES   Display1=Display1 | (1<<5)
#define MVIJF     Display1=Display1 | (1<<6)
#define UNUSED1   Display1=Display1 | (1<<7)

#define VOOR        Display2=Display2 | (1<<0)
#define EEN       Display2=Display2 | (1<<1)
#define TWEE       Display2=Display2 | (1<<2)
#define DRIE     Display2=Display2 | (1<<3)
#define ZES      Display2=Display2 | (1<<4)
#define HVIJF     Display2=Display2 | (1<<5)
#define VIER       Display2=Display2 | (1<<6)
#define UNUSED2   Display2=Display2 | (1<<7)

#define ZEVEN    Display3=Display3 | (1<<0)
#define ACHT    Display3=Display3 | (1<<1)
#define NEGEN    Display3=Display3 | (1<<2)
#define HTIEN      Display3=Display3 | (1<<3)
#define UUR     Display3=Display3 | (1<<4)
#define TWAALF     Display3=Display3 | (1<<5)
#define ELF      Display3=Display3 | (1<<6)
#define UNUSED3   Display3=Display3 | (1<<7)


#define DRW         digitalWrite

// Sawa and paune
#define SAWA    0 // Digital 0 or PD0 pin 30
#define PAUNE     1 // Digital 1 or PD1 pin 31

//Start time @ xx:xx:xx
//int  hour=9, minute=32, second=00;

int second; //00-59;
int minute; //00-59;
int hh24; //RTC time in 24 hour mode
int hh; //To diplay 12 hour mode 

float pulse; //to simulate LED glow
static unsigned long msTick =0;  // millisecond after we increase the second counter.
char Display1=0, Display2=0, Display3=0;

// hardware constants
int LEDClockPin=6;        //PD6
int LEDDataPin=7;         //PD7
int LEDStrobePin=8;       //PB0
int FWDButtonPin=2;   //PD2
int REVButtonPin=3;     //PD3

int EnablePin = 9; //PB1 => enables the shift regisers, (may be v2 ma hardware strap to vcc)

void setup() {

  Wire.begin();
    ////////////////////////////////
  /*
  second = 00;
   minute = 30;
   hh24 = 7;
   set_time();//Set the time on the getRTCTime
  */
  ///////////////////////////////

  // initialise the hardware  
  // initialize pins as outputs:
  pinMode(LEDClockPin, OUTPUT); 
  pinMode(LEDDataPin, OUTPUT); 
  pinMode(LEDStrobePin, OUTPUT); 
  pinMode(FWDButtonPin, INPUT_PULLUP); 
  pinMode(REVButtonPin, INPUT_PULLUP); 
  pinMode(EnablePin, OUTPUT); 

  //enable the shift registers pin
  digitalWrite(EnablePin, HIGH);
  
  msTick=millis();      // Initialise the msTick counter
  getRTCTime();// Get the time once on the getRTCTime
    }

//turn off all the LEDs
void ledsoff(void) {
  Display1=0;
  Display2=0;
  Display3=0;
  DRW(SAWA,0);
  DRW(PAUNE,0);
}

//write LEDs to the shift register to ULN to display
void WriteLEDs(void) {
  // Now we write the actual values to the hardware
  shiftOut(LEDDataPin, LEDClockPin, MSBFIRST, Display3);
  shiftOut(LEDDataPin, LEDClockPin, MSBFIRST, Display2);
  shiftOut(LEDDataPin, LEDClockPin, MSBFIRST, Display1);
  digitalWrite(LEDStrobePin,HIGH);
  delay(2);
  digitalWrite(LEDStrobePin,LOW);    
}

//Calculation of the time to be displayed
void displaytime(void){

  get_time(); //from getRTCTime

  //pass RTC values into a new variable -
  //this way one variable stores time in 24 hour mode and another that can be converted to 12 hour mode
  //time is monitored 'in the background' in 24 hour mode using the RTC, but is displayed in an adjusted 12-hr format
  hh = hh24;

  //convert to 12 hr mode for 12 to 1 (instead of 24)
  if (hh24 < 1) {
    hh = 12;
  }

  if ((hh24 > 12) && (hh24 < 24)) {
    hh = hh24 - 12;
  }

  // Turn off all the LEDs 
  ledsoff();

  // display the appropriate minute counter          
  if ((minute>4) && (minute<10))  { MVIJF;           }
  if ((minute>9) && (minute<15))  { MTIEN;          }
  if ((minute>14) && (minute<20)) { KWART;          }
  if ((minute>19) && (minute<25)) { MTIEN; HALF;    }
  if ((minute>24) && (minute<30)) { MVIJF; HALF;  }
  if ((minute>29) && (minute<35)) { HALF;           }
  if ((minute>34) && (minute<40)) { MVIJF; HALF;  }
  if ((minute>39) && (minute<45)) { MTIEN; HALF;    }
  if ((minute>44) && (minute<50)) { KWART;          }
  if ((minute>49) && (minute<55)) { MTIEN;          }
  if (minute>54)                  { MVIJF;          }

  if ((minute <5))
  {
    switch (hh) {
    case 1: EEN;       break;
    case 2: TWEE;       break;
    case 3: DRIE;     break;
    case 4: VIER;      break;
    case 5: HVIJF;     break;
    case 6: ZES;       break;
    case 7: ZEVEN;     break;
    case 8: ACHT;     break;
    case 9: NEGEN;      break;
    case 10: HTIEN;     break;
    case 11: ELF;   break;
    case 12: ;   break;
    }
   UUR;
  }
  else
  if ((minute >4) && (minute < 20) )
  {
        OVER;

        switch (hh) 
      {
        case 1: EEN;      break;
        case 2: TWEE;      break;
        case 3: DRIE;    break;
        case 4: VIER;     break;
        case 5: HVIJF;    break;
        case 6: ZES;      break;
        case 7: ZEVEN;    break;
        case 8: ACHT;    break;
        case 9: NEGEN;     break;
        case 10: HTIEN;    break;
        case 11: ELF;  break;
        case 12: TWAALF;  break;
      }
  }
  
  else
  if ((minute > 19) && (minute <30))
  {
        VOOR;

        switch (hh) 
      {
        case 1: TWEE;      break;
        case 2: DRIE;    break;
        case 3: VIER;     break;
        case 4: MVIJF;    break;
        case 5: ZES;      break;
        case 6: ZEVEN;    break;
        case 7: ACHT;    break;
        case 8: NEGEN;     break;
        case 9: HTIEN;    break;
        case 10: ELF;  break;
        case 11: TWAALF;  break;
        case 12: EEN;      break;
      }
  }
  
    else
  if ((minute > 29) && (minute <35))
  {
         switch (hh) 
      {     
        case 1: TWEE;      break;
        case 2: DRIE;    break;
        case 3: VIER;     break;
        case 4: MVIJF;    break;
        case 5: ZES;      break;
        case 6: ZEVEN;    break;
        case 7: ACHT;    break;
        case 8: NEGEN;     break;
        case 9: HTIEN;    break;
        case 10: ELF;  break;
        case 11: TWAALF;  break;
        case 12: EEN;      break;
      }
  }

     else
  if ((minute > 34) && (minute <45))
  {
      OVER;
         switch (hh) 
      {     
        case 1: TWEE;      break;
        case 2: DRIE;    break;
        case 3: VIER;     break;
        case 4: HVIJF;    break;
        case 5: ZES;      break;
        case 6: ZEVEN;    break;
        case 7: ACHT;    break;
        case 8: NEGEN;     break;
        case 9: HTIEN;    break;
        case 10: ELF;  break;
        case 11: TWAALF;  break;
        case 12: EEN;      break;
      }
  }

       else
  if ((minute > 34) && (minute <45))
  {
      VOOR;
         switch (hh) 
      {     
        case 1: TWEE;      break;
        case 2: DRIE;    break;
        case 3: VIER;     break;
        case 4: HVIJF;    break;
        case 5: ZES;      break;
        case 6: ZEVEN;    break;
        case 7: ACHT;    break;
        case 8: NEGEN;     break;
        case 9: HTIEN;    break;
        case 10: ELF;  break;
        case 11: TWAALF;  break;
        case 12: EEN;      break;
      }
  }
  
  
  else
    {
      // if we are greater than 34 minutes OVER the hour then display
      // the next hour, as we will be displaying a 'to' sign
      //case 1 --> display 2 (1+1 hour) ----like this we display the hour ahead
      VOOR;
      switch (hh) 
      {
        case 1: TWEE;      break;
        case 2: DRIE;    break;
        case 3: VIER;     break;
        case 4: HVIJF;    break;
        case 5: ZES;      break;
        case 6: ZEVEN;    break;
        case 7: ACHT;    break;
        case 8: NEGEN;     break;
        case 9: HTIEN;     break;
        case 10: ELF;  break;
        case 11: TWAALF;  break;
        case 12: EEN;     break;
      }                 
    }
  
  WriteLEDs();

}

// Increment function to update the time
void incrementtime(void) {
  
  second = 0;
  minute=minute+1;
  if (minute >= 60) 
  {    minute = 0; 
    if (++hh24 > 23) {      hh24 = 0;      }
  }  
}


//**************************main loop**************************
void loop(void) {
  
  ledsoff();
  runAllTime();
  ledsoff();
   while(1)
   {
     int reading1 = digitalRead(FWDButtonPin);
  int reading2 = digitalRead(REVButtonPin);
  // heart of the timer - keep looking at the millisecond timer on the Arduino
  // and increment the seconds counter every 1000 ms
  if ( millis() - msTick > 999) {
    msTick = millis();
    second++;   
  }
  if (second == 60) 
  {
    incrementtime();
    displaytime();
  pulseLEDeveryMinute();
  }

  // When Forward button is pressed ==> 
  if (reading1 != lastButtonState1) 
  {
  lastDebounceTime1 = millis();
  }

  if ((millis() - lastDebounceTime1) > debounceDelay1) 
  {
    if (reading1 != buttonState1) 
  {
    buttonState1 = reading1;
    if (buttonState1 == HIGH) 
    {
    minute = (((minute/5)*5) +7);  //Current time + about 3 minutes (plus)
    second = 0;
    incrementtime();
    second++; 
    set_time();  //send time to getRTCTime //When you press this button the time is updated on the RTC.
    displaytime();
    pulseLEDButton();
 
    }
    }
  }


  // When Reverse button is pressed 
  if (reading2 != lastButtonState2) 
  {
  lastDebounceTime2 = millis();
  }
    if ((millis() - lastDebounceTime2) > debounceDelay2) {
    if (reading2 != buttonState2) 
  {
      buttonState2 = reading2;
      if (buttonState2 == HIGH) 
    {
    minute = (((minute/5)*5) -5); //Current time - 5 minutes (minus)
    second = 0; 
    if (minute < 0) { minute = 55;  if (--hh24 <0) hh24 = 12; } 
    incrementtime();
    second++; 
    set_time(); 
    displaytime();
    pulseLEDButton();
    }
    }
  }
  lastButtonState1 = reading1;
  lastButtonState2 = reading2;
   }

} //end of loop     

///////////////////////////////////////////////////////////////////////

//Get RTC time
void getRTCTime()
{
  //set_time();
  get_time();

}


void set_time()
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0);
  Wire.write(decToBcd(second));
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hh24));
  Wire.endTransmission();
}

void get_time()
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS); 
  Wire.write(0);//set register to 0
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 3);//get 3 bytes (seconds,minutes,hours);
  second = bcdToDec(Wire.read() & 0x7f);
  minute = bcdToDec(Wire.read());
  hh24 = bcdToDec(Wire.read() & 0x3f);
}

byte decToBcd(byte val)
{
  return ( (val/10*16) + (val%10) );
}

byte bcdToDec(byte val)
{
  return ( (val/16*10) + (val%16) );
}

void runAllTime ()
{
  ledsoff();  WriteLEDs();  EEN;                WriteLEDs();    pulseLEDStart();   delay(waitingtime);   
  ledsoff();  WriteLEDs();  TWEE;                WriteLEDs();    pulseLEDStart();   delay(waitingtime);  
  ledsoff();  WriteLEDs();  DRIE;            WriteLEDs();    pulseLEDStart();   delay(waitingtime);  
  ledsoff();  WriteLEDs();  VIER;               WriteLEDs();    pulseLEDStart();   delay(waitingtime);
  ledsoff();  WriteLEDs();  HVIJF;              WriteLEDs();    pulseLEDStart();   delay(waitingtime);
  ledsoff();  WriteLEDs();  ZES;                WriteLEDs();    pulseLEDStart();   delay(waitingtime);
  ledsoff();  WriteLEDs();  ZEVEN;              WriteLEDs();    pulseLEDStart();   delay(waitingtime);  
  ledsoff();  WriteLEDs();  ACHT;              WriteLEDs();    pulseLEDStart();   delay(waitingtime);
  ledsoff();  WriteLEDs();  NEGEN;               WriteLEDs();    pulseLEDStart();   delay(waitingtime);
  ledsoff();  WriteLEDs();  HTIEN;               WriteLEDs();    pulseLEDStart();   //delay(waitingtime);  
  //ledsoff();  WriteLEDs();  ELF;             WriteLEDs();    pulseLEDStart();   delay(waitingtime);  
  //ledsoff();  WriteLEDs();  TWAALF;             WriteLEDs();    pulseLEDStart();   delay(waitingtime);  
  //ledsoff();  WriteLEDs();  MVIJF;OVER;         WriteLEDs();    pulseLEDStart();   delay(waitingtime);  
  //ledsoff();  WriteLEDs();  MTIEN;OVER;          WriteLEDs();    pulseLEDStart();   delay(waitingtime);  
  //ledsoff();  WriteLEDs();  KWART;OVER;       WriteLEDs();    pulseLEDStart();   delay(waitingtime);  
  //ledsoff();  WriteLEDs();  TWENTY;OVER;        WriteLEDs();    pulseLEDStart();   delay(waitingtime);
  //ledsoff();  WriteLEDs();  TWENTY; MVIJF;OVER;   WriteLEDs();    pulseLEDStart();   delay(waitingtime);  
  //ledsoff();  WriteLEDs();  HALF; OVER;           WriteLEDs();    pulseLEDStart();   delay(waitingtime);  
  //ledsoff();  WriteLEDs();  TWENTY; MVIJF; VOOR;      WriteLEDs();    pulseLEDStart();   delay(waitingtime);  
  //ledsoff();  WriteLEDs();  TWENTY; VOOR;           WriteLEDs();    pulseLEDStart();   delay(waitingtime);
  //ledsoff();  WriteLEDs();  KWART;VOOR;           WriteLEDs();    pulseLEDStart();   delay(waitingtime);
  //ledsoff();  WriteLEDs();  MTIEN;VOOR;            WriteLEDs();    pulseLEDStart();   delay(waitingtime);
  //ledsoff();  WriteLEDs();  MVIJF;VOOR;           WriteLEDs();    pulseLEDStart();   delay(waitingtime); 

}   



////////////////////////////////////////////////////////////////////////////////////////
void pulseLEDeveryMinute()
{
  for (float pulse = 10; pulse < 250; pulse *= 1.0002)
  {
    analogWrite(EnablePin, pulse);
  }

  for (float pulse = 250; pulse > 200; pulse -= 0.001)
  {
    analogWrite(EnablePin, pulse);
  }

}
////////////////////////////////////////////////////////////////////////////////////////
void pulseLEDButton()
{
for (float pulse = 10; pulse < 250; pulse *= 1.0003)
  {
    analogWrite(EnablePin, pulse);
  }

  for (float pulse = 250; pulse > 200; pulse -= 0.004)
  {
    analogWrite(EnablePin, pulse);
  }
 }
////////////////////////////////////////////////////////////////////////////////////////
void pulseLEDStart()
{

   for (float pulse = 10; pulse < 250; pulse *= 1.0005)
  {
    analogWrite(EnablePin, pulse);
  }

  for (float pulse = 250; pulse > 200; pulse -= 0.005)
  {
    analogWrite(EnablePin, pulse);
  }
  
 }
////////////////////////////////////////////////////////////////////////////////////////
