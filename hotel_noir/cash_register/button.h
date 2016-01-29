#pragma once

//////////////////////////////////////////////////////////////////

struct TButton
{
  int Pin;
  int Digit;
  bool Flag = false;
  void (*UpdateCallback)(int);
  unsigned long UpdateTime;

  TButton()
  { }

  TButton(int pin, int digit, void (*updateCallback)(int))
    : Pin(pin)
    , Digit(digit)
    , UpdateCallback(updateCallback)
  {
    pinMode(Pin, INPUT_PULLUP); 
  }

  void Update()
  {
    int value = digitalRead(Pin);
    if (value == LOW && !Flag) {
      Flag = true;
      // startPlayback();
      UpdateTime = millis();
      UpdateCallback(Digit);
      Serial.print("Digit ");
      Serial.print(Digit);
      Serial.print("  pin ");
      Serial.println(Pin);
    } else if (value == HIGH && Flag) {
      Flag = false;
    }
  }
};

//////////////////////////////////////////////////////////////////



