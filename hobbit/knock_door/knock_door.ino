#include <SoftwareSerial.h>
//#include <DFPlayer_Mini_Mp3.h>


const int PiezoPin = A2;
const int LedPin = 13;
const int RelayPin = 12;

const long MinTimeBetweenKnocks = 100;
const long MaxTimeBetweenKnocks = 1500;

const int ThresholdValue = 3;
const int MaxKnocks = 5;

// Timestamps of the last 5 knocks.
long knockTimes[MaxKnocks];
int knockCount = 0;

void addKnock()
{
  if (knockCount >= MaxKnocks) {
    for (int i = 0; i < MaxKnocks - 1; ++i) {
      knockTimes[i] = knockTimes[i + 1];
    }
    --knockCount;
  }
  
  knockTimes[knockCount] = millis();
  ++knockCount;
}

long getLastKnockTime()
{
  if (knockCount == 0) {
    return 0;
  } else {
    return knockTimes[knockCount - 1];
  }
}

bool validateRythm()
{
  if (knockCount < MaxKnocks) {
    return false;
  }
  
  long d1 = knockTimes[1] - knockTimes[0];
  long d2 = knockTimes[2] - knockTimes[1];
  long d3 = knockTimes[3] - knockTimes[2];
  long d4 = knockTimes[4] - knockTimes[3];
  
  /*
  Durations between knocks must be approximately the following:
    d2 == d1 (mistake <= 1/5 d1)
    d3 == 4 * d1 / 9 (mistake <= 1/9 d1)
    d4 == 4 * d1 / 9 (mistake <= 1/9 d1)
  */

  if (d2 > (6 * d1 / 5) || d2 < (4 * d1 / 5)) {
    return false;
  }
  
  if (d3 > (5 * d1 / 9) || d3 < (3 * d1 / 9)) {
    return false;
  }
  
  if (d4 > (5 * d1 / 9) || d4 < (3 * d1 / 9)) {
    return false;
  }
  
  return true;
}

void setup () 
{
  Serial.begin(9600);
  //mp3_set_serial(Serial);	//set softwareSerial for DFPlayer-mini mp3 module 
  //mp3_set_volume(20);
  pinMode(LedPin, OUTPUT); //просто так - лампочка
  pinMode(RelayPin, OUTPUT); //реле
  digitalWrite(RelayPin, HIGH);
}

void loop () 
{
  int value = analogRead(A2);
  if (value >= 1) {  
    Serial.print("New piezo value ");
    Serial.print(value);
    Serial.print(" millis ");
    Serial.println(millis());
  }
  
  if (value > ThresholdValue && millis() - getLastKnockTime() > MinTimeBetweenKnocks) { 
    Serial.println("Add knock");
    addKnock();
    if (validateRythm()) {
      Serial.println("Open door");
      knockCount = 0;
      digitalWrite(RelayPin, LOW); 
      delay(5000);  
      digitalWrite(RelayPin, HIGH);
    }
    
    digitalWrite(LedPin, HIGH);
    delay(100);
    digitalWrite(LedPin, LOW);
  } 
  
  // We consider only series of 2+ knocks to filter out noise.
  if (knockCount > 1 && millis() - getLastKnockTime() > MaxTimeBetweenKnocks) {
    Serial.println("Start gandalf, reset knocks");
    knockCount=0;       
  
    /* 
    mp3_play(1); 
    delay(5000); // гендальф не пускает
    */
  }
}


