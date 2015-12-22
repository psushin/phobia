#include "Adafruit_Thermal.h"
#include "SoftwareSerial.h"

const int RelayValue = 610;
const int RelayPin = A0;

#define PRINTER_TX_PIN A4 // YELLOW WIRE  labeled RX on printer
#define PRINTER_RX_PIN A5 // GREEN WIRE   labeled TX on printer

SoftwareSerial printerSerial(PRINTER_RX_PIN, PRINTER_TX_PIN); // Declare SoftwareSerial obj first
Adafruit_Thermal printer(&printerSerial);     // Pass addr to printer constructor

void PrinterSetup()
{
  printerSerial.begin(9600);
  printer.begin();
  printer.sleep();

  pinMode(RelayPin, OUTPUT); 
  digitalWrite(RelayPin, LOW); 
}

void PrintFooterString(int value)
{
  switch(value) {
    case 540:
      // Полная свобода.
      printer.println(F("\x8f\xae\xab\xad\xa0\xef\x20\xe1\xa2\xae\xa1\xae\xa4\xa0"));
      return;

    case 375:
      // Удивительно мягкий вкус
      printer.println(F("\x93\xa4\xa8\xa2\xa8\xe2\xa5\xab\xec\xad\xae\x20\xac\xef\xa3\xaa\xa8\xa9\x20\xa2\xaa\xe3\xe1"));
      return;

    case 385:
      // Как никогда, как всегда
      printer.println(F("\x8a\xa0\xaa\x20\xad\xa8\xaa\xae\xa3\xa4\xa0\x2c\x20\xaa\xa0\xaa\x20\xa2\xe1\xa5\xa3\xa4\xa0"));
      return;

    case 610:
      // Проведи ночь с огоньком
      printer.println(F("\x8f\xe0\xae\xa2\xa5\xa4\xa8\x20\xad\xae\xe7\xec\x20\xe1\x20\xae\xa3\xae\xad\xec\xaa\xae\xac"));
      return;
  }
  
  switch (unsigned(random()) % 3) {
    case 0:
      // Да пребудет с вами сила.
      printer.println(F("\x84\xa0\x20\xaf\xe0\xa5\xa1\xe3\xa4\xa5\xe2\x20\xe1\x20\xa2\xa0\xac\xa8\x20\x91\xa8\xab\xa0\x21")); 
      break;

    case 1:
      // Сосредоточься!
      printer.println(F("\x91\xae\xe1\xe0\xa5\xa4\xae\xe2\xae\xe7\xec\xe1\xef\x2e\x0a\x8f\xae\xe7\xe3\xa2\xe1\xe2\xa2\xe3\xa9\x20\x91\xa8\xab\xeb\x20\xe2\xa5\xe7\xa5\xad\xa8\xa5\x21")); 
      break;

    case 2:
      // Знание - сила!
      printer.println(F("\x87\xad\xa0\xad\xa8\xa5\x20\x2d\x20\xe1\xa2\xa5\xe2\x21\x20\x8f\xe3\xe2\xec\x20\xe3\xaa\xa0\xa6\xa5\xe2\x20\x82\xa0\xac\x21")); 
      break;
  };
}

void PrintGoods(int value)
{
  switch (value) {
    case 565:
      printer.println(F("MURATTI")); 
      break;

    case 580:
      printer.println(F("ROYALE")); 
      break;

    case 370:
      printer.println(F("Chesterfield")); 
      break;

    case 510:
      printer.println(F("MERIT")); 
      break;

    case 540:
      printer.println(F("Winston")); 
      break;
      
    case 375:
      printer.println(F("Camel")); 
      break;


    case 470:
      printer.println(F("MS")); 
      break;

    case 490:
      printer.println(F("MS Rosse")); 
      break;

    case 385:
      printer.println(F("Marlboro")); 
      break;

    case 610:
      printer.println(F("Rothmans")); 
      break;

    default:
      // Товар отсутствует.
      printer.println(F("\x92\xae\xa2\xa0\xe0\x20\xae\xe2\xe1\xe3\xe2\xe1\xe2\xa2\xe3\xa5\xe2"));
      break;
  }
}

void PrintValue(int value)
{
  char data[] = "        ";
  int len = strlen(data);
  int i = 0;
  while (value > 0) {
    int r = value % 10;
    value = value / 10;
    
    char c = 0x30 + r;
    data[len - i - 1] = c;
    ++i;
    if (i == 2) {
      data[len - i - 1] = '.';
      ++i;
    }
  }

  printer.println(data);
}

void PrintReceipt(int value)
{
  printer.wake();

  // Заголовок.
  printer.justify('C');
  printer.setSize('L');
  printer.underlineOn();
  printer.println(F("Hotel Noir"));
  printer.feed(2);

  printer.justify('L');
  printer.setSize('M');
  printer.underlineOff();
  PrintGoods(value);
  
  // Цена.
  printer.justify('R');
  PrintValue(value);
  printer.feed(1);

  // Футер
  printer.justify('C');
  printer.println(F("-----------------------------"));

  printer.setSize('S');
  PrintFooterString(value);
  printer.feed(3);

  // Открываем замок.
  if (value == RelayValue) {
    digitalWrite(RelayPin, HIGH);
    delay(500);
    digitalWrite(RelayPin, LOW); 
  }

  printer.sleep();
}

//////////////////////////////////////////////////////////////////

const int PressCount = 3;
int PressedDigits[PressCount];
int UnpressedIndex = 0;

void SetNextPressed(int digit)
{
  PressedDigits[UnpressedIndex] = digit;
  ++UnpressedIndex;
}

void TryPrintReceipt()
{
  if (UnpressedIndex < PressCount) {
    return;
  }

  int value = 0;
  int multiplier = 1;
  for (int i = PressCount - 1; i >= 0; --i) {
      value += PressedDigits[i] * multiplier;
      multiplier *= 10;
  }

  PrintReceipt(value);
  UnpressedIndex = 0;
}

//////////////////////////////////////////////////////////////////

struct TButton
{
  int Pin;
  int Digit;
  bool Flag = false;

  TButton()
  { }

  TButton(int pin, int digit)
    : Pin(pin)
    , Digit(digit)
  {
    pinMode(Pin, INPUT_PULLUP); 
  }

  void Update()
  {
    int value = digitalRead(Pin);
    if (value == LOW && !Flag) {
      Flag = true;
      // startPlayback();
      SetNextPressed(Digit);
    } else if (value == HIGH && Flag) {
      Flag = false;
    }
  }
};

const int DigitCount = 10;
// Pins 2 to 12 are for buttons 0 to 9.
const int BaseButtonPin = 2;
TButton Buttons[DigitCount];

//////////////////////////////////////////////////////////////////

void setup() {
  //start serial connection
  Serial.begin(9600);
  PrinterSetup();

  for (int i = 0; i < DigitCount; ++i) {
    Buttons[i] = TButton(BaseButtonPin + i, i);
  }
}

void loop() {
  for (int i = 0; i < DigitCount; ++i) {
    Buttons[i].Update();
  }

  TryPrintReceipt();
}
