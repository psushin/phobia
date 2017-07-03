#include <CapacitiveSensor.h>

const int HeartLight1Pin = 12;
const int HeartLight2Pin = 11;
const int HeartLight3Pin = 10;
const int BeepPin = 9;
const int DoorRelayPin = 13;

const int SphereOutputPin = A0;
const int AutoRotateButtonPin = A1;
const int ManualRotateButtonPin = A2;
const int DoorPin = A3;
const int PresencePin = A4;
const int PlatformRotatedPin = A5;

CapacitiveSensor HeartSensor = CapacitiveSensor(6,7);

const int EnginePin = 5;
const int SphereInputPin = 4;
const int PumpPin = 3;

///////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Mode1:
 *  - Жду сигнал от шара. Реагирую на кнопку 1, кнопку 2 которая крутит без остановки, 
 *  
 * Mode2:  
 *  - Жду емкостный датчик.Реагирую на кнопку 1 и 2. 
 *  
 * Mode3: 
 *  - Качаем, ждем время.
 *  
 * Mode4:
 *  - Жду пока посадят и прокрутят человека.
 *  
 * Mode5:
 *  - Прокрутили, реагирую только на кнопки.
 */

enum EState {
  ES_MODE_1,
  ES_MODE_2,
  ES_MODE_3,
  ES_MODE_4,
  ES_MODE_5,
};

EState State = ES_MODE_1;

///////////////////////////////////////////////////////////////////////////////////////////////////

void beep(unsigned long delayms)
{
  analogWrite(BeepPin, 200);     // Almost any value can be used except 0 and 255
                                 // experiment to get the best tone
  delay(delayms);                // wait for a delayms ms
  analogWrite(BeepPin, 0);       // 0 turns it off
}  

void auto_rotate()
{
   digitalWrite(EnginePin, HIGH);
      
   while (digitalRead(PlatformRotatedPin) == HIGH) {
     delay(1); 
   }
   digitalWrite(EnginePin, LOW); 
}

bool check_buttons()
{
  if (digitalRead(ManualRotateButtonPin) == LOW) {
    delay(300);
    Serial.println("Start manual rotation by button");
    while (digitalRead(ManualRotateButtonPin) == LOW) {
      digitalWrite(EnginePin, HIGH);
    }

    Serial.println("Finish manual rotation by button");
    digitalWrite(EnginePin, LOW);
    return true;
  } else if (digitalRead(AutoRotateButtonPin) == LOW) {
    // Double check to eliminate noise.
    delay(100);
    if (digitalRead(AutoRotateButtonPin) != LOW) {
      return false;
    }

    Serial.println("Start auto rotation by button");
    auto_rotate();
    Serial.println("Finish auto rotation by button. Resetting");
    reset();
    return true;
  }

  return false;
}

void reset_heart()
{
  analogWrite(PumpPin, LOW);
  digitalWrite(HeartLight3Pin, HIGH);
  digitalWrite(HeartLight2Pin, HIGH);
  digitalWrite(HeartLight1Pin, HIGH);
}

void reset()
{
  State = ES_MODE_1;
  reset_heart();
  
  digitalWrite(DoorRelayPin, LOW);
  analogWrite(BeepPin, LOW);
  digitalWrite(SphereOutputPin, LOW);
  digitalWrite(EnginePin, LOW);

  beep(500);

  Serial.println("Done resetting, working in mode 1");
}

///////////////////////////////////////////////////////////////////////////////////////////////////

long StartPumpingTime = 0;
long LastSeenCapacity = 0;
const long CapacityTimeout = 800;

void run_mode_1()
{
  if (check_buttons()) {
    return;
  }

  if (digitalRead(SphereInputPin) == LOW) {
    Serial.println("Switched to mode 2");
    State = ES_MODE_2;
    // Recalibrate.
    HeartSensor.reset_CS_AutoCal();
  }
}

const long StartHeartThreshold = 2000;
const long ContinueHeartThreshold = 2000;
const long LevelTimeout = 4000;
void run_mode_2()
{
  if (check_buttons()) {
    return;
  }

  long value = HeartSensor.capacitiveSensor(30);
  if (value > StartHeartThreshold) {
     Serial.print("Start pumping, switched to mode 3. Value ");
     Serial.println(value);
     LastSeenCapacity = StartPumpingTime = millis();
     analogWrite(PumpPin, 150);
     State = ES_MODE_3;
  }
}

void run_mode_3()
{
  if (check_buttons()) {
    return;
  }

  long now = millis();
  long value = HeartSensor.capacitiveSensor(30);
  //if (value > ContinueHeartThreshold) {
  //  LastSeenCapacity = now;
  //}

  // After first touch capacity sensor becomes mad.
  if (now - LastSeenCapacity < CapacityTimeout || true) { 
    Serial.print("Value ");
    Serial.println(value);
    if (now - StartPumpingTime > 4 * LevelTimeout) {
      Serial.println("Finished pumping, open door, send signal to sphere, switch to mode 4");
      digitalWrite(DoorRelayPin, HIGH);
      analogWrite(PumpPin, LOW);

      // Send signal to sphere.
      digitalWrite(SphereOutputPin, HIGH);
      delay(500);
      digitalWrite(SphereOutputPin, LOW);
      
      State = ES_MODE_4;
    } else if (now - StartPumpingTime > 3 * LevelTimeout) {
      Serial.println("Switched level 3");
      digitalWrite(HeartLight3Pin, LOW);
    } else if (now - StartPumpingTime > 2 * LevelTimeout) {
      Serial.println("Switched level 2");
      digitalWrite(HeartLight2Pin, LOW);
    } else if (now - StartPumpingTime > LevelTimeout) {
      Serial.println("Switched level 1");
      digitalWrite(HeartLight1Pin, LOW);
    }
  } else {
    Serial.print("Failed to pump, switch back to mode 2, value ");
    Serial.println(value);
    
    reset_heart();
    State = ES_MODE_2;
  }
}

void run_mode_4()
{
  if (check_buttons()) {
    return;
  }

  if (digitalRead(DoorPin) == LOW && digitalRead(PresencePin) == HIGH) {
     Serial.println("Closing door"); 
     digitalWrite(DoorRelayPin, LOW);
     delay(3000);

     if (digitalRead(DoorPin) != LOW || digitalRead(PresencePin) != HIGH)  {
       Serial.println("Presence not confirmed");    
       digitalWrite(DoorRelayPin, HIGH);
       return;
     }

     beep(500);
     delay(1000);
     beep(500);
     delay(1000);
     beep(1500);
     delay(1000);

     if (digitalRead(DoorPin) != LOW || digitalRead(PresencePin) != HIGH)  {
       Serial.println("Presence not confirmed");    
       digitalWrite(DoorRelayPin, HIGH);
       return;
     }
     
     Serial.println("Finish him! Rotate.");
     auto_rotate(); 
     Serial.println("Finish rotation, switch to mode 5");
     State = ES_MODE_5;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void setup() 
{
  Serial.begin(9600);
  
  pinMode(HeartLight3Pin, OUTPUT);
  pinMode(HeartLight2Pin, OUTPUT);
  pinMode(HeartLight1Pin, OUTPUT);
  pinMode(BeepPin, OUTPUT);
  pinMode(DoorRelayPin, OUTPUT);

  pinMode(SphereOutputPin, OUTPUT);
  pinMode(AutoRotateButtonPin, INPUT_PULLUP);
  pinMode(ManualRotateButtonPin, INPUT_PULLUP);
  pinMode(DoorPin, INPUT_PULLUP);
  pinMode(PresencePin, INPUT);
  pinMode(PlatformRotatedPin, INPUT_PULLUP);

  pinMode(EnginePin, OUTPUT);
  pinMode(SphereInputPin, INPUT_PULLUP);
  pinMode(PumpPin, OUTPUT);

  reset();

  beep(500);
  delay(500);
  beep(500);
}

void loop() 
{
  switch (State) {
    case ES_MODE_1:
      run_mode_1();
      break;
      
    case ES_MODE_2:
      run_mode_2();
      break;

    case ES_MODE_3:
      run_mode_3();
      break;

    case ES_MODE_4:
      run_mode_4();
      break;

    case ES_MODE_5:
      check_buttons();
      break;
    
  };
  delay(1);

  /*
  if (digitalRead(RotateButtonPin) == LOW) {
    delay(300);
    while (digitalRead(RotateButtonPin) == LOW) {
      digitalWrite(EnginePin, HIGH);
    }

    Serial.println("Finish rotation by pin");
    digitalWrite(EnginePin, LOW);
    return;
  }


  if (digitalRead(DoorClosedPin) == LOW) {
    delay(300);

    if ((digitalRead(PresencePin) == HIGH) && (digitalRead(DoorClosedPin) == LOW) && (millis() - StartRotationTime > MovementTimeout)) {  
      //Serial.print("Door ");
      //Serial.println(doorValue);
      //Serial.print("Presence ");
      //Serial.println(presenceValue);
      Serial.println("Start rotation auto");
      StartRotationTime = millis();
      digitalWrite(EnginePin, HIGH);
      
      while (digitalRead(PlatformRotatedPin) == HIGH) 
      {
        delay(1); 
      }
  
      Serial.println("Finish rotation auto");
      digitalWrite(EnginePin, LOW);
    }  
  }
  
  delay(10);

*/

/*
  Serial.print("DoorClosedPin ");
  Serial.println(digitalRead(DoorClosedPin));

  Serial.print("PresencePin ");
  Serial.println(digitalRead(PresencePin));

  Serial.print("ButtonPin ");
  Serial.println(digitalRead(RotateButtonPin));

  Serial.print("PlatformRotatedPin ");
  Serial.println(digitalRead(PlatformRotatedPin));

  Serial.println();
  delay(1000);
*/

}
