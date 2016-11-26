#include "RunningMedian.h"

////////////////////////////////////////////////////////////////////////////////

class TTimeoutActionBase
{
public:
  TTimeoutActionBase(unsigned long delta)
    : Delta_(delta)
  {
    ResetLastTime(); 
  }

  void Update()
  {
    unsigned long time = millis();
    unsigned long diff = time - LastTime_;
    if (diff < Delta_) {
      return;
    }

    LastTime_ = time;
    Do();
  }

  virtual void Do() = 0;
  
protected:
  void ResetLastTime()
  {
    LastTime_ = millis();
  }
  
private:
  const unsigned long Delta_;
  unsigned long LastTime_;
};

////////////////////////////////////////////////////////////////////////////////

#define WINDOW_SIZE 7

class TWindow
  : public TTimeoutActionBase
{
public:
  TWindow(unsigned long delta, int echoPin, int trigPin)
    : TTimeoutActionBase(delta)
    , EchoPin_(echoPin)
    , TrigPin_(trigPin)
    , Samples_(WINDOW_SIZE)
  {
    pinMode(TrigPin_, OUTPUT);
    pinMode(EchoPin_, INPUT);

    Reset(delta);
  }

  long GetMedianDistance()
  {
    return Samples_.getMedian();
  }

  void Reset(int delayTime = 0)
  {
    Samples_.clear();
    for (int i = 0; i < WINDOW_SIZE; ++i) {
      Samples_.add(GetDistance());
      delay(delayTime);
    }
    ResetLastTime();
  }
  
private:
  const int EchoPin_;
  const int TrigPin_;

  RunningMedian Samples_;

  virtual void Do()
  { 
    Samples_.add(GetDistance());
  }

  long GetDistance()
  {
    digitalWrite(TrigPin_, LOW);
    delayMicroseconds(2);
    digitalWrite(TrigPin_, HIGH);
    delayMicroseconds(10);
    digitalWrite(TrigPin_, LOW);
    
    long duration = pulseIn(EchoPin_, HIGH);
    return duration / 2 / 2.91;
  }
};

////////////////////////////////////////////////////////////////////////////////

/*

class TFlasher
  : public TTimeoutActionBase
{
public:
  TFlasher(unsigned long delta, int pin)
    : TTimeoutActionBase(delta)
    , Pin_(pin)
  {
    pinMode(Pin_, OUTPUT);
    digitalWrite(Pin_, HIGH);
  }
  
  void StartFlashing()
  {
    IsFlashing_ = true;
    Next();
  }

  void StopFlashing()
  {
    IsFlashing_ = false;
    On_ = true;
 
    digitalWrite(Pin_, HIGH);
  }

private:
  const int Pin_;
  bool IsFlashing_;

  int Count_ = 0;
  int Limit_ = 0;
  bool On_ = true;

  virtual void Do()
  {
    if (!IsFlashing_) {
      return;
    }
    
    if (++Count_ == Limit_) {
      digitalWrite(Pin_, On_ ? LOW : HIGH);
      On_ = !On_;
      Next();
    }
  }

  void Next()
  {
    Count_ = 0;
    Limit_ = random(1, 10);
  }
};
*/

////////////////////////////////////////////////////////////////////////////////

const int ShakerPin = 11;
const int LightPin = 10;
const int MagnetPin = 9;
const int FanPin = 8;

const int MaxDiffCount = 3;
const int DistanceDelta = 2;

const int BuggyJumpDelta = 100;

const int MaxDistance = 250;
const int MinDistance = 150;

class TState
  : public TTimeoutActionBase
{
public:
  TState(unsigned long delta, TWindow* window /*, TFlasher* flasher */)
    : TTimeoutActionBase(delta)
    , Window_(window)
  //  , Flasher_(flasher)
  {
    pinMode(ShakerPin, OUTPUT);
    pinMode(MagnetPin, OUTPUT);
    pinMode(FanPin, OUTPUT);
    pinMode(LightPin, OUTPUT);

    digitalWrite(MagnetPin, HIGH);
    digitalWrite(ShakerPin, HIGH);
    digitalWrite(FanPin, HIGH);
    digitalWrite(LightPin, HIGH);
    
    LastDistance_ = Window_->GetMedianDistance();
  }

  virtual void Do()
  {
    long distance = Window_->GetMedianDistance();
    if (distance != LastDistance_) {
      Serial.print("Distance ");
      Serial.println(distance);
    }
    
    if (distance > MaxDistance) {
      FlushMoney();
      return;
    }
    
    int delta = abs(LastDistance_ - distance);
    
    if (delta > BuggyJumpDelta) {
      Serial.println("Buggy jump");
      DiffCount_ = 0;
      return;
    }
 
    bool hasMovement = (delta > DistanceDelta);
    LastDistance_ = distance;
    
    if ((hasMovement && IsMoving_) || (!hasMovement && !IsMoving_)) {
      DiffCount_ = 0;
      return;
    }
  
    if (++DiffCount_ < MaxDiffCount) {
      return;
    }

    DiffCount_ = 0;
    IsMoving_ = !IsMoving_;
    
    if (distance < MinDistance) {
      IsMoving_ = false;
    }

    Serial.print("IsMoving ");
    Serial.print(IsMoving_);
    Serial.print("Distance");
    Serial.println(distance);
    if (IsMoving_) {
      StartShakingAndFlashing();
    } else {
      StopShakingAndFlashing();
    }
  }
  
private:
  TWindow* Window_;
  //TFlasher* Flasher_;
  
  bool IsMoving_ = false;
  int DiffCount_ = 0;
  
  long LastDistance_;

  void FlushMoney()
  {
    Serial.println("Flush money");

    if (!IsMoving_) {
      StartShakingAndFlashing();
    }

    // First start fan.
    digitalWrite(FanPin, LOW);
    delay(2000);
    
    // Open magnet forever, to make sure that door has opened as well.
    digitalWrite(MagnetPin, LOW);
    
    delay(1000);
    
    StopShakingAndFlashing();
        
    // Run fan for 120 sec.
    delay(120000);
    digitalWrite(FanPin, HIGH);
    
    // Sleep almost forever.
    delay(10 * 60 * 60000);
  }

  void StartShakingAndFlashing()
  {
    Serial.println("Start shaking and flashing");
    //Flasher_->StartFlashing();
    digitalWrite(ShakerPin, LOW);
    digitalWrite(LightPin, LOW);
  }

  void StopShakingAndFlashing()
  {
    Serial.println("Stop shaking and flashing");
    //Flasher_->StopFlashing();
    digitalWrite(ShakerPin, HIGH);
    digitalWrite(LightPin, HIGH);
  }

  void Reset()
  {
    //Flasher_->StopFlashing();
    Window_->Reset();
  }
};

////////////////////////////////////////////////////////////////////////////////

const int EchoPin = 5;
const int TrigPin = 4;

const int LedPin = 13;

TWindow* Window;
//TFlasher* Flasher;
TState* State;

void setup() {
  Serial.begin(9600);

  Window = new TWindow(70, EchoPin, TrigPin);
  //Flasher = new TFlasher(1000, LightPin);
  State = new TState(200, Window /*, Flasher */);

  pinMode(LedPin, OUTPUT);
  digitalWrite(LedPin, HIGH);
}

bool Started = false;

void loop() {
  Window->Update();
  if (!Started && Window->GetMedianDistance() > MaxDistance) {
    return;
  }
  
  digitalWrite(LedPin, LOW);

  if (!Started) {
    Serial.println("Start WALL-E!");
    Started = true;
  }
  
  //Flasher->Update();
  State->Update();
}

