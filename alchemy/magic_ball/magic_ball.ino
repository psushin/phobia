#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>

#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 2

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)h 
Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, PIN, NEO_GRB + NEO_KHZ800);

SoftwareSerial MP3Serial(3, 4);

//////////////////////////////////////////////////////////////////////////////////

// Mode 1 - fade in into blue-white.

void FadeIn()
{
  // Into blue.
  const int R = 14;
  const int G = 119;
  const int B = 174;
  
  const int TotalTime = 10000;
  const int DeltaTime = 50;
  const int N = TotalTime / DeltaTime;
  
  for (int i = 0; i < N; ++i) {
    ColorFill(strip.Color(
      R * (i + 1) / N, 
      G * (i + 1) / N, 
      B * (i + 1) / N));
    delay(DeltaTime);
  }
}

void Heartbeat()
{
  const int HighDelay = 150;
  const int LowDelay = 1100;
  
  ColorFill(strip.Color(255, 0, 0));
  delay(HighDelay);
  ColorFill(strip.Color(20, 0, 0));
  delay(HighDelay);
  ColorFill(strip.Color(255, 0, 0));
  delay(HighDelay);
  ColorFill(strip.Color(20, 0, 0));
  delay(LowDelay);
}

void ColorFill(uint32_t color) {
  for(int i = 0; i < strip.numPixels(); ++i) {
    strip.setPixelColor(i, color);
  }
  strip.show();
}

void Rainbow() 
{ 
  for(int j = 0; j < 25; ++j) {
    int color = random(26);
    for(int i = 0; i < strip.numPixels(); ++i) {
      strip.setPixelColor(i, Wheel(color * 10));
    }
    strip.show();
    delay(200);
  }
}

//////////////////////////////////////////////////////////////////////////////////

int Mode = 1;

long TimestampMode2;
const long GloomDuration = 5 * 60 * 1000;
bool WasGloom = false;

//////////////////////////////////////////////////////////////////////////////////

#define PIN_RING1 8
#define PIN_RING2 9
#define PIN_RING3 10

#define PIN_FROM_ALCHEMY_OK 5
#define PIN_FROM_ALCHEMY_FAIL 6

#define PIN_ACTIVATE_ALCHEMY A0

// Mode 1: wait for gerkons.
void RunMode1()
{
  int value = int(digitalRead(PIN_RING1)) + int(digitalRead(PIN_RING2)) + int(digitalRead(PIN_RING3));  
  Serial.println(value);
 
  if (value == 0) {
    FadeIn();
    mp3_play(1); // Проигрываем "mp3/0001.mp3"
    delay(10000);
    mp3_stop();
  
    TimestampMode2 = millis();  
    Mode = 2;
  
    Serial.println("Sending signal to alchemy machine");
    digitalWrite(PIN_ACTIVATE_ALCHEMY, HIGH);
    delay(1500);
    digitalWrite(PIN_ACTIVATE_ALCHEMY, LOW);
  }
}

// Mode 2: wait for signal from alchemy machine.
void RunMode2()
{
  if (digitalRead(PIN_FROM_ALCHEMY_OK) == LOW) {
    Serial.println("Alchemy machine produced crystall");
    mp3_play(2);
    delay(10000);
    mp3_stop();  
    
    Mode = 3;
  }  
  
  if (digitalRead(PIN_FROM_ALCHEMY_FAIL) == LOW) {
    Serial.println("Alchemy machine was pissed off");
    mp3_play(3);
    delay(10000);
    mp3_stop();  
  }
  
  //if ((millis() - TimestampMode2 > GloomDuration) && !WasGloom) {
  //  WasGloom = true;
  //  mp3_play(3);
  //  Rainbow();
  //  mp3_stop();  
  //}
}

// Mode 3: wait for signal from crypt.
void RunMode3()
{
  Heartbeat();
}


//////////////////////////////////////////////////////////////////////////////////

void setup() {
  strip.begin();
  strip.show();
    
  MP3Serial.begin(9600);
  mp3_set_serial(MP3Serial); 
  delay(100);
  mp3_set_volume(20);
  
  Serial.begin(9600);
  
  pinMode(PIN_RING1, INPUT_PULLUP);
  pinMode(PIN_RING2, INPUT_PULLUP);
  pinMode(PIN_RING3, INPUT_PULLUP);
  
  pinMode(PIN_FROM_ALCHEMY_OK, INPUT_PULLUP);  
  pinMode(PIN_FROM_ALCHEMY_FAIL, INPUT_PULLUP);  
  
  pinMode(PIN_ACTIVATE_ALCHEMY, OUTPUT);
  digitalWrite(PIN_ACTIVATE_ALCHEMY, LOW);
}

void loop() {
  
  switch (Mode) {
     case 1: 
       RunMode1();
       break;
       
     case 2:
       RunMode2();
       break;
       
     case 3:
       RunMode3();
       break;
       
     default:
       break;
  };
  
  delay(10);
}

//////////////////////////////////////////////////////////////////////////////////

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
