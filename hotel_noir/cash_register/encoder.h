#pragma once

/////////////////////////////////////////////////////////////

class TEncoder
{
public:
  TEncoder(int pinA, int pinB)
    : PinA_(pinA)
    , PinB_(pinB)
  {
    pinMode(PinA_, INPUT);
    pinMode(PinB_, INPUT);
    LastState_ = digitalRead(PinA_);
    LastMoveTime_ = millis();
  }

  unsigned long GetUpdateTime()
  {
    return LastMoveTime_;
  }

  void Update()
  {
    unsigned long t = millis();
    if (t - LastMoveTime_  < 1) {
      return;
    }

    if (t - LastMoveTime_  > 5000) {
      // If no move for a long time - reset counter.
      Count_ = 0;
    }
    
    int a = digitalRead(PinA_);
    int b = digitalRead(PinB_);

    bool state = (a == HIGH);

    if (state == LastState_) {
      return;
    }

    LastState_ = state;
    LastMoveTime_ = t;
    
    if (!LastState_) {
      return;
    } 

    bool direction = (b == HIGH);
    if (Direction_ == direction) {
      ++Count_;
    } else {
      Direction_ = direction;
      Count_ = 0;
    }
  }

  bool GetDirection() const
  {
    return Direction_;
  }

  int GetCounter() const
  {
    return Count_;
  }
  
private:
  const int PinA_;
  const int PinB_;

  bool LastState_ = false;
  unsigned long LastMoveTime_ = 0;

  bool Direction_ = false;
  int Count_ = 0;
  
};

/////////////////////////////////////////////////////////////

