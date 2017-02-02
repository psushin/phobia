#include <Keypad.h>

const byte ROWS = 4;
const byte COLS = 4;  
char hexaKeys[ROWS][COLS] = { 
  {'1', '2', '3', 'A'},  
  {'4', '5', '6', 'B'},  
  {'7', '8', '9', 'C'},  
  {'*', '0', '#', 'D'} 
}; 

byte rowPins[ROWS] = {8, 9, 10, 11};
byte colPins[COLS] = {4, 5, 6, 7};

Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
 
////////////////////////////////////////////////////////////////////////////////

const int CLOCK_PIN = A5;
const int LATCH_PIN = A4;
const int DATA_PIN = A3;

byte Register = 0;

byte InvertMask(byte mask)
{
  return ~mask;
}

void SetRegisterBit(byte bitNumber)
{
  digitalWrite(LATCH_PIN, LOW);
  Register |= 1 << bitNumber;
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, Register); 
  digitalWrite(LATCH_PIN, HIGH); 
}

void UnsetRegisterBit(byte bitNumber)
{
  digitalWrite(LATCH_PIN, LOW);
  Register &= InvertMask(1 << bitNumber);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, Register); 
  digitalWrite(LATCH_PIN, HIGH); 
}

void SetupRegisterPins()
{
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
}

////////////////////////////////////////////////////////////////////////////////

#include <Servo.h>
Servo LeftDoorServo;  // Привод двери 1
//Servo RightDoorServo;  // Привод двери 2

#define LEFT_DOOR_PIN 3
void SetupServo()
{
  LeftDoorServo.attach(LEFT_DOOR_PIN);
//  RightDoorServo.attach(RIGHT_SERVO);
}

////////////////////////////////////////////////////////////////////////////////

// Basic utilities.

void OpenLeftDoor()
{
  LeftDoorServo.write(130);
}

void CloseLeftDoor()
{
  LeftDoorServo.write(180);
}

void OpenRightDoor()
{
  UnsetRegisterBit(5);
}

void CloseRightDoor()
{
  SetRegisterBit(5);
}

void LockRightMagnet()
{
  SetRegisterBit(6);
}

void ReleaseRightMagnet()
{
  UnsetRegisterBit(6);
}

void LockLeftMagnet()
{
  SetRegisterBit(7);
}

void ReleaseLeftMagnet()
{
  UnsetRegisterBit(7);
}

void EnableLeftDiod()
{
  SetRegisterBit(2);
}

void DisableLeftDiod()
{
  UnsetRegisterBit(2);
}

void EnableRightDiod()
{
  SetRegisterBit(1);
}

void DisableRightDiod()
{
  UnsetRegisterBit(1);
}

void FireFire()
{
  SetRegisterBit(3);
  delay(1000);
  UnsetRegisterBit(3);
}

void BlinkLeftDiod(int count = 1, bool enable = false)
{
  for (int i = 0; i < count; ++i) {
    DisableLeftDiod();
    delay(100);
    EnableLeftDiod();
    delay(100);
    DisableLeftDiod();
  }
  
  if (enable) {
    EnableLeftDiod();
  }
}

void BlinkRightDiod(int count = 1, bool enable = false)
{
  for (int i = 0; i < count; ++i) { 
    DisableRightDiod();
    delay(100);
    EnableRightDiod();
    delay(100);
    DisableRightDiod();
  }
  
  if (enable) {
    EnableRightDiod();
  }
}

void BlinkBothDiods(int count = 1, bool left = false, bool right = false)
{
  for (int i = 0; i < count; ++i) { 
    DisableRightDiod();
    DisableLeftDiod();
    delay(300);
    EnableRightDiod();
    EnableLeftDiod();
    delay(300);
  }
  
  DisableRightDiod();
  DisableLeftDiod();
  delay(300);
  
  if (left) {
    EnableLeftDiod();
  }
  
  if (right) {
    EnableRightDiod();
  }
}

////////////////////////////////////////////////////////////////////////////////

int Mode = 0;

////////////////////////////////////////////////////////////////////////////////

// Zero mode - quest collection.

void SetupZeroMode()
{
  LockLeftMagnet();
  LockRightMagnet();
  DisableLeftDiod();
  DisableRightDiod();
  OpenLeftDoor();
  OpenRightDoor();  
}

void RunZeroMode()
{
  char key = keypad.getKey();
  if (key == '*') {
    CloseLeftDoor();
    CloseRightDoor();
    BlinkLeftDiod();
    Mode = 1;
  } 
}

////////////////////////////////////////////////////////////////////////////////

// Simple inputs.
const int CASH_REGISTER_PIN = A2;
const int TABLE_PIN = A1;
const int DOOR_BUTTON_PIN = 12;
const int TOBACCO_PIN = 13;

void SetupInputPins()
{
  pinMode(CASH_REGISTER_PIN, INPUT_PULLUP);
  pinMode(TABLE_PIN, INPUT_PULLUP);
  pinMode(DOOR_BUTTON_PIN, INPUT_PULLUP);
  pinMode(TOBACCO_PIN, INPUT_PULLUP);
}

////////////////////////////////////////////////////////////////////////////////

// Mode 1 - get ready to send orders.

bool HasTable = false;
long TableInstant = 0;
bool HasCashRegister = false;
bool HasOrder = false;

const long TableTimeout = 10000;

void RunOneMode()
{
  if (!HasTable && digitalRead(TABLE_PIN) == LOW) {
    HasTable = true;
    TableInstant = millis();
    BlinkLeftDiod(1);
  }
  
  if (!HasCashRegister && digitalRead(CASH_REGISTER_PIN) == HIGH) {
    HasCashRegister = true;
    BlinkRightDiod(1);
  }
  
  if (!HasOrder && HasTable && millis() > TableInstant + TableTimeout) {
    HasOrder = true;
    ReleaseLeftMagnet();
    delay(2000);
    OpenLeftDoor();
    BlinkLeftDiod(2);
    EnableLeftDiod();
  }
  
  if (HasOrder && HasCashRegister) {
    Mode = 2;
    BlinkRightDiod();
  }
}

////////////////////////////////////////////////////////////////////////////////

// Cyclic buffer for letters.

#define N 3

class TLetterBuffer
{
public:
  void AddLetter(char letter)
  {
    for (int i = N - 1; i > 0; --i) {
      Letters[i] = Letters[i - 1];
    }
    Letters[0] = letter;
    ++Count;
    if (Count > N) {
      Count = N;
    }
  }
  
  void Reset()
  {
    Count = 0;
  }
  
  bool CheckRoomNumber() const
  {
    Serial.println(Count);
    Serial.println(Letters[0]);
    Serial.println(Letters[1]);
    Serial.println(Letters[2]);
    return Count == 3 && Letters[0] == '2' && Letters[1] == '0' && Letters[2] == '5';
  }

private:
  char Letters[N];
  int Count = 0;
};

////////////////////////////////////////////////////////////////////////////////

// Two mode: sending the order back.

TLetterBuffer LetterBuffer;

void RunTwoMode()
{
  char key = keypad.getKey();
  if ('0' <= key && key <= '9') {
    LetterBuffer.AddLetter(key);
  } else if (key == 'A') {
    LetterBuffer.Reset();
  } else if (key == 'D') {
    
    if (digitalRead(DOOR_BUTTON_PIN) == HIGH) {
      // Door is not tightly closed.
      BlinkBothDiods(3, true, false);
      return;
    }
    
    if (digitalRead(TOBACCO_PIN) == LOW && LetterBuffer.CheckRoomNumber()) {
      // OK, send it!
      
      DisableLeftDiod();
      CloseLeftDoor();
      delay(5000);
      ReleaseRightMagnet();
      delay(1000);
      OpenRightDoor();
      delay(500);
      EnableRightDiod();
      delay(1000);
      FireFire();
      Mode = 3;
    } else {
      // Fake send with return.
      DisableLeftDiod();
      CloseLeftDoor();
      delay(5000);
      OpenLeftDoor();
      BlinkBothDiods(3, true, false);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

void setup() 
{
  SetupRegisterPins();
  SetupInputPins();
  SetupServo(); 
  
  SetupZeroMode();  
  
  Serial.begin(9600);
}
 
void loop() 
{
  switch (Mode) {
    case 0:
      RunZeroMode();
      break;
      
    case 1:
      RunOneMode();
      break;
      
    case 2:
      RunTwoMode();
      break;
  };
  delay(100);
}

