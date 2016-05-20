#include "receipt_printer.h"
#include "button.h"
#include "encoder.h"

#include "display.h"
#include "images.h"

#undef USE_ENCODER

//////////////////////////////////////////////////////////////////

const int RelayValue = 610;
const int RelayPin = A0;

//////////////////////////////////////////////////////////////////

const int PressCount = 5;
int PressedDigits[PressCount];
int UnpressedIndex = 0;

//////////////////////////////////////////////////////////////////

#define  DIGIT_COUNT 10    
// Pins 3 to 13 are for buttons 0 to 9.
const int BaseButtonPin = 4;
TButton Buttons[DIGIT_COUNT];

//////////////////////////////////////////////////////////////////

#define ENCODER_THRESHOLD 20
TEncoder Encoder(A3, A2);

//////////////////////////////////////////////////////////////////

volatile bool Interrupt = false;

void InterruptBackground()
{
  Interrupt = true;
}

//////////////////////////////////////////////////////////////////

const unsigned long IdlePeriod = 30000;
unsigned long LastIdleEnd;
int DisplayPin = 3;
bool image[WIDTH][HEIGHT];
TFlipDotDisplay Display(DisplayPin, &Interrupt);

//////////////////////////////////////////////////////////////////

void SetNextPressed(int digit)
{
  if (UnpressedIndex == 0 && digit == 0) {
    return; 
  }
  
  PressedDigits[UnpressedIndex] = digit;
  ++UnpressedIndex;
}

int32_t GetValue()
{
  int32_t  value = 0;
  int32_t multiplier = 1;
  for (int i = UnpressedIndex - 1; i >= 0; --i) {
      value += PressedDigits[i] * multiplier;
      multiplier *= 10;
  } 
  return value;
}

//////////////////////////////////////////////////////////////////

void PrintReceipt()
{
  int32_t value = GetValue();
  PrintReceipt(value);

  // Открываем замок.
  if (value == RelayValue) {
    digitalWrite(RelayPin, HIGH);
    delay(500);
    digitalWrite(RelayPin, LOW); 
  }
}

//////////////////////////////////////////////////////////////////

void UpdateDisplay()
{
  if (UnpressedIndex > 4) {
    int32_t value = GetValue();
    if (value == 70214) {
      createSpecial(image);
      Display.Shuffle(2000);  
      Display.ChaoticMorph(image);  
    } else {
      createError(image);
      Display.TopShutter(image);
    }
    delay(5000);
    UnpressedIndex = 0;
    
    UpdateDisplay();
  } else {
    createDigits(image, PressedDigits, UnpressedIndex);
    Display.RightMorph(image);
  }  
}

// Easy version without encoder.
void UpdateDisplayLight()
{
  createDigits(image, PressedDigits, UnpressedIndex);
  Display.RightMorph(image);
    
  if (UnpressedIndex > 2) {
    int32_t value = GetValue();
    if (value == RelayValue) {
      PrintReceipt();
    } else {
      createError(image);
      Display.TopShutter(image);
    }
    delay(5000);
    UnpressedIndex = 0;
    
    UpdateDisplayLight();
  }  
}

void PlayBackground()
{
  #define INTERRUPTABLE(x) x; \
    if (Interrupt) return
    
   // Interruptable background display show.
  Interrupt = false;

  for (int i = 0; i < 5; ++i) {
    INTERRUPTABLE(Display.Shuffle(2000));
  
    createHotel(image);
    INTERRUPTABLE(Display.ChaoticMorph(image));
    INTERRUPTABLE(DelayImpl(8000, &Interrupt));
    INTERRUPTABLE(Display.Shuffle(2000));
    
    createNoir(image);
    INTERRUPTABLE(Display.ChaoticMorph(image));
    INTERRUPTABLE(DelayImpl(8000, &Interrupt));
    
    INTERRUPTABLE(Display.Shuffle(2000));
    
    createSmallPhobia(image);
    INTERRUPTABLE(Display.ChaoticMorph(image));
    
    INTERRUPTABLE(DelayImpl(8000, &Interrupt));
  }

  #undef INTERRUPTABLE
}

//////////////////////////////////////////////////////////////////

void setup() {
  //start serial connection
  Serial.begin(9600);
  PrinterSetup();

  pinMode(RelayPin, OUTPUT); 
  digitalWrite(RelayPin, LOW); 

  for (int i = 0; i < DIGIT_COUNT; ++i) {
    Buttons[i] = TButton(BaseButtonPin + i, i, SetNextPressed);
  }

  createSpecial(image);
  Display.SetImage(image);
  delay(5000); 

  UpdateDisplay();
  Serial.println(F("Start the fun!"));

  attachInterrupt(digitalPinToInterrupt(2), InterruptBackground, RISING);
  LastIdleEnd = millis();
}

void loop() {
  int prevUnpressedIndex = UnpressedIndex;

  /*
  int prevDir = Encoder.GetDirection();
  int prevCount = Encoder.GetCounter();
  */

for (int i = 0; i < DIGIT_COUNT; ++i) {
    Buttons[i].Update();
    if (Buttons[i].UpdateTime > LastIdleEnd) {
      LastIdleEnd = Buttons[i].UpdateTime;
    }
  }


#ifdef USE_ENCODER
  Encoder.Update();
  if (Encoder.GetUpdateTime() > LastIdleEnd) {
    LastIdleEnd = Encoder.GetUpdateTime();
  }

  if (millis() - LastIdleEnd > IdlePeriod) {
    Serial.print("LastIdle ");
    Serial.print(LastIdleEnd);
    Serial.print(" millis ");
    Serial.print(millis());
    
    
    PlayBackground();
    LastIdleEnd = millis();

    UnpressedIndex = 0;
    UpdateDisplay();
  }

  /*
  if (Encoder.GetDirection() != prevDir || Encoder.GetCounter() != prevCount) {
    Serial.print("Direction ");
    Serial.print(Encoder.GetDirection());
    Serial.print(" counter ");
    Serial.println(Encoder.GetCounter());
  }*/
  
  if (UnpressedIndex != prevUnpressedIndex) {
    Serial.println(F("Update display."));
    UpdateDisplay();
    Display.PrintToSerial();
  }

  if (Encoder.GetCounter() > ENCODER_THRESHOLD) {
    if (Encoder.GetDirection()) {
      Serial.println("Print receipt");
      PrintReceipt();
    }
    // If opposite direction - reset the value.

    UnpressedIndex = 0;
    UpdateDisplay();
  }
#else
  if (UnpressedIndex != prevUnpressedIndex) {
    Serial.println(F("Update display."));
    UpdateDisplayLight();
    Display.PrintToSerial();
  }
#endif

}
