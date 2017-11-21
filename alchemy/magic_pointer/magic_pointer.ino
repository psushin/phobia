int step_delay = 2500;

const int Limits = 1510;

class Pointer
{
public:
  Pointer(int dirPin, int stepPin)
    : DirPin(dirPin)
    , StepPin(stepPin)
  {
      pinMode(DirPin, OUTPUT);
      pinMode(StepPin, OUTPUT);
  }
  
  void Prepare()
  {
    for (int i = 0; i < Limits; ++i) {
      Step(1);
    }
    
    Serial.println("Gone left");
  }
  
  // Possible values for each axis are {-1, 0, 1}
  void Step(int value) 
  { 
    if (value == 0) {
      delayMicroseconds(2 * step_delay);
      return;
    }
    
    if (value > 0) {
      digitalWrite(DirPin, LOW);
    } else if (value < 0) {
      digitalWrite(DirPin, HIGH);
    }
    
    digitalWrite(StepPin, HIGH);
    delayMicroseconds(step_delay);
    digitalWrite(StepPin, LOW);
    delayMicroseconds(step_delay);
  }
  
private:  
  int DirPin;
  int StepPin;
};

// DirectionX, StepX, DirectionY, StepY.
Pointer pointer(5, 4);

int currentStep = Limits;
void MakeStep(bool forward)
{
  int c = forward ? -1 : 1;
  currentStep += c;
  
  if (currentStep < 0) {
    currentStep = 0;
    delay(10000);
  } else if (currentStep < Limits) {
    pointer.Step(1 * c);
  } else {
    currentStep = Limits;
  }
}

const int HallPin = A0;
const int HallDiff = 2;
const long MillisDiff = 1000;

// Value when nothing is present.
const int MaxHallValue = 517;

int lastHallValue;
long lastMillis;

void setup() 
{
  Serial.begin(9600);
  
  pointer.Prepare();
  
  lastHallValue = analogRead(HallPin);
  Serial.print("Hall value ");
  Serial.println(lastHallValue);
}

void loop() {
  int hallValue = analogRead(HallPin);
  long now = millis();
  
  /*
  Serial.print("Value ");
  Serial.print(hallValue);
  Serial.print(" lastValue ");
  Serial.print(lastHallValue);
  Serial.print(" millis ");
  Serial.print(now);
  Serial.print(" lastMillis ");
  Serial.println(lastMillis);
  */
  
  if (abs(hallValue - lastHallValue) > HallDiff && hallValue < MaxHallValue) {
    MakeStep(true);
    lastHallValue = hallValue;
    lastMillis = now;
  } else if (now - lastMillis < MillisDiff) {
    MakeStep(true);
  } else {
    MakeStep(false);
  }
  
  //Serial.println(currentStep);
  delay(2);
}
