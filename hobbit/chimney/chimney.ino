const int RelayPin = 10;
const int InputPin = 13;

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
    Serial.println("Open relay");
    digitalWrite(RelayPin, LOW);
    delay(1000 * 60 * 5);
    digitalWrite(RelayPin, HIGH);
  } else {
    Serial.println("Do nothing");
  }
}
