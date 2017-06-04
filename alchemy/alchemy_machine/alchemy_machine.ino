/*!
 *  Alchemy machine.
 */

const int SmokePowerPin = 7;
const int SmokeRunPin = 6;
const int PumpPin = 5;
const int LockPin = 4;

const int GoodOutPin = A0;
const int BadOutPin = A1;

const int B1Pin = 8;
const int B2Pin = 9;
const int B3Pin = 10;
const int B4Pin = 11;
const int WarmupPin = 12;
const int MagicBallPin = 13;

const int StartPin = 3;

///////////////////////////////////////////////////////////////////////////////

#define N 4

class TIngridientBuffer
{
public:
  void AddValue(int value)
  {
    if (Count > 0 && Ingridients[0] == value) {
      // Double presses do not count
      return;
    }
    
    for (int i = N - 1; i > 0; --i) {
      Ingridients[i] = Ingridients[i - 1];
    }
    Ingridients[0] = value;
    ++Count;
    if (Count > N) {
      Count = N;
    }
    ++TotalCount;
  }
  
  void Reset()
  {
    Count = 0;
    TotalCount = 0;
  }
  
  int GetTotalCount() const 
  {
    return TotalCount;
  }
  
  bool CheckIngridients() const
  {
    Serial.print("Checking ingridients ");
    Serial.println(Count);
    Serial.println(Ingridients[0]);
    Serial.println(Ingridients[1]);
    Serial.println(Ingridients[2]);
    Serial.println(Ingridients[3]);
    return Count == N && Ingridients[0] == 2 && Ingridients[1] == 4 && Ingridients[2] == 3 && Ingridients[3] == 1;
  }

private:
  int Ingridients[N];
  int Count = 0;
  int TotalCount = 0;
};

TIngridientBuffer IngridientBuffer;

///////////////////////////////////////////////////////////////////////////////

enum EState {
  ES_STANDBY,
  ES_WARMUP,
  ES_ACTIVE
};

EState State;

///////////////////////////////////////////////////////////////////////////////

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  pinMode(B1Pin, INPUT_PULLUP);
  pinMode(B2Pin, INPUT_PULLUP);
  pinMode(B3Pin, INPUT_PULLUP);
  pinMode(B4Pin, INPUT_PULLUP);
  
  pinMode(WarmupPin, INPUT_PULLUP);
  pinMode(MagicBallPin, INPUT_PULLUP);
  pinMode(StartPin, INPUT_PULLUP);
  
  pinMode(SmokePowerPin, OUTPUT);
  pinMode(SmokeRunPin, OUTPUT);
  pinMode(PumpPin, OUTPUT);
  pinMode(LockPin, OUTPUT);  
  pinMode(BadOutPin, OUTPUT);
  pinMode(GoodOutPin, OUTPUT);
  
  Reset();
}

void Reset()
{
  Serial.println("Switched to standby mode");
  
  digitalWrite(SmokePowerPin, HIGH);
  digitalWrite(SmokeRunPin, HIGH);
  digitalWrite(PumpPin, HIGH);
  digitalWrite(LockPin, HIGH); 
  digitalWrite(BadOutPin, LOW);
  digitalWrite(GoodOutPin, LOW);  
  
  IngridientBuffer.Reset();
  
  State = ES_STANDBY;
}

///////////////////////////////////////////////////////////////////////////////

void RunStandby()
{
  int value = digitalRead(StartPin); 
  if (value == LOW) {
    Serial.println("Switch to warmup mode");
    State = ES_WARMUP;
    digitalWrite(SmokePowerPin, LOW);
  }
}

void RunWarmup()
{
 if (digitalRead(MagicBallPin) == LOW) {
   Serial.println("Switch to active mode");
   State = ES_ACTIVE;
 } 
}

int GetButton()
{
  if (digitalRead(B1Pin) == HIGH) {
    return 1;
  }
  
  if (digitalRead(B2Pin) == HIGH) {
    return 2;
  }
  
  if (digitalRead(B3Pin) == HIGH) {
    return 3;
  }
  
  if (digitalRead(B4Pin) == HIGH) {
    return 4;
  }
  
  return 0;
}

bool HasWarmedUp()
{
  // Warmup detector works wrong.
  return true;
  
  for (int i = 0; i < 100; ++i) {
    int value = digitalRead(WarmupPin);
    Serial.print("Warmup value ");
    Serial.println(value);
    if (value == LOW) {
      return true;
    }
    delay(1);
  }
  return false;
}

void DoAllMagic()
{
  // Switch pump.
  digitalWrite(PumpPin, LOW);
  delay(7000);
  digitalWrite(PumpPin, HIGH);
  
  if (HasWarmedUp()) {
    Serial.println("Smoke machine warmed up");
    digitalWrite(SmokeRunPin, LOW);
    delay(3000);
    digitalWrite(SmokeRunPin, HIGH);
  } else {
    Serial.println("Smoke machine didn't warmed up, skipping...");
  }
  
  digitalWrite(LockPin, LOW);
}

void RunActive()
{
  int button = GetButton();
  if (button == 0) {
    return;
  }
  
  IngridientBuffer.AddValue(button);
  if (IngridientBuffer.CheckIngridients()) {
    DoAllMagic();
    
    delay(30 * 1000);
    
    digitalWrite(GoodOutPin, HIGH);      
    delay(20000);

    Reset();
  } else if (IngridientBuffer.GetTotalCount() % 12 == 0) {
    digitalWrite(BadOutPin, HIGH);      
    delay(100);
    digitalWrite(BadOutPin, LOW);      
  }
}

///////////////////////////////////////////////////////////////////////////////


void loop() {
 switch (State) {
   case ES_STANDBY:
     RunStandby();
     break;
     
   case ES_WARMUP:
     RunWarmup();
     break;
   
   case ES_ACTIVE:
     RunActive();
     break;
 }; 

 delay(1);
}
