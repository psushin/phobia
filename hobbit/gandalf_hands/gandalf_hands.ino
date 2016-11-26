#include <Servo.h>

Servo Rservo;
Servo Lservo;

int state = 2;
int button = 12;
int count = 3;

const int MaxAngle = 80;
const int JitterAngle = 8;
const int JitterCount = 5;

bool buttonReset = false;

int getLeftAngle(int angle)
{
  return angle;
}

int getRightAngle(int angle)
{
  return 98 - angle;
}

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
    
    for (int angle = 0; angle <= MaxAngle; ++angle) {
      // in steps of 1 degree
      
      if (state == 0) Rservo.write(getRightAngle(angle));
      if (state == 1) Lservo.write(getLeftAngle(angle));
      if (state == 2) {
        Rservo.write(getRightAngle(angle));
        Lservo.write(getLeftAngle(angle));
      }
      
      delay(15);
    }
    
    if (state != 2) {
      delay(1000);
    } else {
      // Move hands up and down a little bit.
      for (int j = 0; j < JitterCount; ++j) {
        for (int i = 0; i < JitterAngle; ++i) {
          Lservo.write(getLeftAngle(MaxAngle - i));
          Rservo.write(getRightAngle(MaxAngle + i));
          delay(50);
        }
        
        for (int i = JitterAngle; i >= 0; --i) {
          Lservo.write(getLeftAngle(MaxAngle - i));
          Rservo.write(getRightAngle(MaxAngle + i));
          delay(50);
        }
        
        for (int i = 0; i < JitterAngle; ++i) {
          Lservo.write(getLeftAngle(MaxAngle + i));
          Rservo.write(getRightAngle(MaxAngle - i));
          delay(50);
        }
        
        for (int i = JitterAngle; i >= 0; --i) {
          Lservo.write(getLeftAngle(MaxAngle + i));
          Rservo.write(getRightAngle(MaxAngle - i));
          delay(50);
        }
        
        delay(500);
      }
    }
    
    for (int angle = MaxAngle; angle >= 0; angle -= 1) {
      if (state == 0) Rservo.write(getRightAngle(angle));
      if (state == 1) Lservo.write(getLeftAngle(angle));
      if (state == 2) {
        Rservo.write(getRightAngle(angle));
        Lservo.write(getLeftAngle(angle));
      }
      delay(15);
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

