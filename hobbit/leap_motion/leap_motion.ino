const int RelayPin = 13;

void setup()
{
  pinMode(RelayPin, OUTPUT);
  digitalWrite(RelayPin, HIGH);
  
  // start serial port at 9600 bps:
  Serial.begin(9600);
  while (!Serial) {
  }
}
 
void loop()
{
  if (Serial.available() > 0) {
    while (Serial.available() > 0) {
      // Read one byte.
      Serial.read();
    }
    
    // Open relay.
    digitalWrite(RelayPin, LOW);
    delay(2000);
    digitalWrite(RelayPin, HIGH);               
  }
}

