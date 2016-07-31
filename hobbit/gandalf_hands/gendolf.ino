#include <Servo.h>

Servo Rservo;  // create servo object to control a servo
Servo Lservo;
// twelve servo objects can be created on most boards
int state = 2;
int button = 12;
int posR = 0;    // variable to store the servo position
int posL = 0;
int count = 3;

bool buttonReset = false;

void setup() {
  Serial.begin(9600);
  pinMode(button, INPUT_PULLUP);
  
  Rservo.attach(6);
  Lservo.attach(5);// attaches the servo on pin 9 to the servo object
  
  Serial.println("Let the fun begin");
}

void loop() {
  int value = digitalRead(button);
  Serial.print("Value ");
  Serial.println(value);
  if (value == HIGH) {
    buttonReset = true;
  }
  
  if (state <= 2) {
    if (state == 0) {
      Serial.println("Right hand");
    } else if (state == 1) {
      Serial.println("Left hand");
    } else if (state == 2) {
      Serial.println("Both hands");
    }
    
    for (posR = 0; posR <= 98; posR += 1) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      
      if (state == 0) Rservo.write(98 - posR);
      if (state == 1) Lservo.write(posR);
      if (state == 2) {
        Lservo.write(posR);
        Rservo.write(98 - posR);
      }
      
      // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
    
    delay(1000);
    
    for (posR = 98; posR >= 0; posR -= 1) { // goes from 180 degrees to 0 degrees
    
      if (state == 0) Rservo.write(98 - posR);
      if (state == 1) Lservo.write(posR);
      if (state == 2) {
        Lservo.write(posR);
        Rservo.write(98 - posR);
      }
      delay(15);                       // waits 15ms for the servo to reach the position
    } 
    delay(1000);
    
    state++;
    Serial.print("Moving done, state ");
    Serial.println(state);
    
  } else {  
    if (count <= 2) {
      state = 0;
      count++;
    }
    
    if (count > 2 && value == LOW && buttonReset) {
      Serial.println("Button pressed");
      buttonReset = false;
      count = 0;
      state = 0;
    }

  }
  Serial.print("Count ");
  Serial.println(count);
}

