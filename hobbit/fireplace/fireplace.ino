const int RelayPin = 10;
const int InputPin = 13;

long Timestamp = 0;
bool Open = false;

long Duration = long(1000) * 60;

void setup() {                
  pinMode(RelayPin, OUTPUT);     
  pinMode(InputPin, INPUT);   
  digitalWrite(RelayPin, HIGH);  
  Serial.begin(9600);
}

void loop() {
  delay(100);
  int a = digitalRead(InputPin);
  
  if (a == LOW) {
    Open = true;
    Timestamp = millis();
    Serial.println("Open relay");
    digitalWrite(RelayPin, LOW);
  } else if (Open && millis() - Timestamp > Duration) {
    Serial.println("Close relay");
    digitalWrite(RelayPin, HIGH);
    Open = false;
  } else {
    Serial.println("Do nothing");
  }
}
