#include <Servo.h>
#include <SoftwareSerial.h>

SoftwareSerial Serial7Segment(0, 13); //RX pin, TX pin
Servo myservo;
int cycles = 0;
int AmLed = 7;
int PmLed = 8;
int HourBut = A3;
int MinBut = A2;
int SetAlarmTime = A4;
int alarmOnLed = A0;
int snooze = 10;
int alarmOnBut = A5;
int alarmReset = A1;
int snoozeCount;
int pos = 0; 
//From https://github.com/sparkfun/Serial7SegmentDisplay/wiki/Special-Commands#wiki-decimal
#define APOSTROPHE  5
#define COLON       4
#define DECIMAL4    3
#define DECIMAL3    2
#define DECIMAL2    1
#define DECIMAL1    0

long millisTimer;
byte seconds = 0;
byte minutes = 0;
byte hours = 0;
byte AlarmHrs = 0;
byte AlarmMin = 0;
byte motorCount = 0;
boolean amTime = true;
boolean AlarmamTime = true;
boolean colonOn = false;
boolean motorOn = false;
boolean alarmOn = false;
byte leds = 0;
char time[100]; //Used for sprintf
char alarmTime[100];
char snoozeCountDown[100];
char motorCountDown[100];
void LedControl(){     //this function controls the leds
  if(hours == 12)
        {
          if(amTime == true) 
            {
              amTime = false; //Flip AM to PM
              digitalWrite(AmLed, LOW); //turns off am led first
              digitalWrite(PmLed, HIGH);// then turns on pm led
            }
            else
            {
              amTime = true;
              digitalWrite(PmLed, LOW); //turns off pm led first
              digitalWrite(AmLed, HIGH);// then turns on am led
            }
        }
}
void ALedControl(){
    if(AlarmHrs == 12)
        {
          if(AlarmamTime == true) 
            {
              AlarmamTime = false; //Flip AM to PM
              digitalWrite(AmLed, LOW); //turns off am led first
              digitalWrite(PmLed, HIGH);// then turns on pm led
            }
            else
            {
              AlarmamTime = true;
              digitalWrite(PmLed, LOW); //turns off pm led first
              digitalWrite(AmLed, HIGH);// then turns on am led
            }
        }
}
void alarmStart(){
  if(hours == AlarmHrs && minutes == AlarmMin && amTime == AlarmamTime){
    motorOn;
  }
}

void setup() {

  Serial.begin(9600);
  Serial.println("Alarm Clock by Sunny Chao");

  Serial7Segment.begin(9600); //Talk to the Serial7Segment at 9600 bps
  Serial7Segment.write('v'); //Reset the display - this forces the cursor to return to the beginning of the display

//  Serial7Segment.print("0822"); //Send the hour and minutes to the display
  pinMode(AmLed, OUTPUT);
  pinMode(PmLed, OUTPUT);
  pinMode(HourBut, INPUT_PULLUP);  
  pinMode(MinBut, INPUT_PULLUP);
  pinMode(SetAlarmTime, INPUT_PULLUP);
  pinMode(alarmOnLed, OUTPUT);
  pinMode(snooze, INPUT_PULLUP);
  pinMode(alarmOnBut, INPUT_PULLUP);
  pinMode(alarmReset, INPUT_PULLUP);
  millisTimer = millis();
  digitalWrite(AmLed, HIGH);
//  myservo.attach(9);
  Serial7Segment.write(0x77);  // Decimal, colon, apostrophe control command
  Serial7Segment.write(1<<COLON); //keeps colon turned on
  //For testing, we initialize the variables to the current time
  seconds = 55;
  minutes = 59;
  hours = 11;
  AlarmHrs = 10;
  AlarmMin = 10;
}

void loop() 
{
  //Every second update the various variables and blink colon/apos/decimal
  if( (millis() - millisTimer) > 1000)
  {
    millisTimer += 1000; //Adjust the timer forward 1 second
    
    seconds++;
    if(seconds > 59)
    {
      seconds -= 60; //Reset seconds and increment minutes
      minutes++;
      
      if(minutes > 59)
      {
        minutes -= 60; //Reset minutes and increment hours
        hours++;
        if(hours > 12)
        {
          hours -= 12; //Reset hours and flip AM/PM
        }
        LedControl();//AM-PM leds
      }
    }
  } //if (millis.. function ends here
  
  
  /////////////////////// MOTOR CONTROL   //////////////////////
    if(motorOn == true){
      Serial.print("motor on");
      for(pos = 0; pos < 180; pos++ )    // goes from 0 degrees to 180 degrees 
        {                                  // in steps of 1 degree 
          myservo.write(pos);              // tell servo to go to position in variable 'pos' 
          delay(100);                       // waits 15ms for the servo to reach the position 
        } 
      for(pos = 180; pos>=1; pos--)     // goes from 180 degrees to 0 degrees 
        {                                
          myservo.write(pos);              // tell servo to go to position in variable 'pos' 
          delay(100);                       // waits 15ms for the servo to reach the position 
        } 
      motorOn = false;
    }
   /////////////////     snooze button     //////////////////////
    if(digitalRead(snooze) == LOW){
      Serial.print("snooze button");
      if(alarmOn == true){
        motorOn = false;
        snoozeCount = 300;
        snoozeCount--;
        if(snoozeCount == 0){
          motorOn = true;
        }
      }
    }
  
///////////////////  Hours and MInutes buttons /////////////////////////
  if (digitalRead(MinBut) == LOW && digitalRead(SetAlarmTime) == HIGH)
  {
    minutes++;
    if(minutes > 59)
      {
        minutes -= 60; //Reset minutes and increment hours 
        hours++;
        if(hours > 12)
        {
          hours -= 12; //Reset hours and flip AM/PM
        }
        LedControl();//AM-PM leds
      }
  }
  if (digitalRead(HourBut) == LOW && digitalRead(SetAlarmTime) == HIGH)
  {
    hours++;
    if(hours > 12)
    {
      hours -= 12; //Reset hours and flip AM/PM
    }
    LedControl();//controlling AM-PM leds
  }
/////////////////  Set Alarm Button   ///////////////////  
  if (digitalRead(SetAlarmTime) == LOW)
    {
      Serial7Segment.print(alarmTime);
      digitalWrite(AmLed, AlarmamTime);
      digitalWrite(PmLed, !AlarmamTime);
      if(digitalRead(MinBut) == LOW)
      {
        AlarmMin++;
        if(AlarmMin > 59)
        {
          AlarmMin -= 60;
        }
      }
      if(digitalRead(HourBut) == LOW)
      {
        AlarmHrs++;
        if(AlarmHrs > 12)
        {
          AlarmHrs -= 12;
        }
          ALedControl();
      }
    }
   else
        {
          sprintf(time, "%2d%02d", hours, minutes);
          Serial7Segment.print(time);
          digitalWrite(AmLed, amTime);
          digitalWrite(PmLed, !amTime);
        } 
////////////////////////   alarm ON button     //////////////////////
   if(digitalRead(alarmOnBut) == LOW){
     if(alarmOn == true){
       Serial.print("alarm OFF");
       alarmOn = false;
       digitalWrite(alarmOnLed, LOW);
     }
     else{
       Serial.print("alarm ON");
       alarmOn = true;
       digitalWrite(alarmOnLed, HIGH);
       alarmStart();
     }
   }
/////////////////////////////////////////////////////////////
//////////////////     alarm reset button   ////////////////
   if(digitalRead(alarmReset) == LOW){
     Serial.print("alarm reset");
     if(alarmOn == true){
       motorOn = false;
       alarmStart();
     }
   }
  //Debug print the time
//  sprintf(motorCountDown, "%02d", motorCount);
//  sprintf(snoozeCountDown, "%02d", snoozeCount);
  sprintf(alarmTime, "%2d%02d", AlarmHrs, AlarmMin);
  sprintf(time, "HH:MM:SS %02d:%02d:%02d", hours, minutes, seconds);
  Serial.println(time);
//  Serial.println(motorCountDown);
//  Serial.println(snoozeCountDown);
//  sprintf(time, "%2d%02d", hours, minutes); //change this to display different time format .
  //%2d removes zero in front on single digit numbers
//  Serial7Segment.print(time); //Send serial string out the soft serial port to the S7S

  delay(200);
}

