
const int DoorClosedPin = 13;
const int PlatformRotatedPin = 8;
const int PresencePin = 10;
const int RotateButtonPin = 7;
const int EnginePin = 3;

const long MovementTimeout = 20000;
long StartRotationTime = -MovementTimeout;

void setup() 
{
  // put your setup code here, to run once:
  pinMode(DoorClosedPin, INPUT_PULLUP);
  pinMode(PlatformRotatedPin, INPUT_PULLUP);
  pinMode(PresencePin, INPUT);
  pinMode(RotateButtonPin, INPUT_PULLUP);

  pinMode(EnginePin, OUTPUT);
  digitalWrite(EnginePin, LOW);

  Serial.begin(9600);
}

void loop() 
{
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
