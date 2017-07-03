#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>

#ifdef __AVR__
  #include <avr/power.h>
#endif

#define LED_STRIP_PIN 2

// Pattern types supported:
enum  pattern { NONE, RAINBOW_CYCLE, THEATER_CHASE, COLOR_WIPE, SCANNER, FADE, HEARTBEAT };

// Patern directions supported:
enum  direction { FORWARD, REVERSE };
 
// NeoPattern Class - derived from the Adafruit_NeoPixel class
class NeoPatterns : public Adafruit_NeoPixel
{
    public:
 
    // Member Variables:  
    pattern  ActivePattern;  // which pattern is running
    direction Direction;     // direction to run the pattern
    
    unsigned long Interval;   // milliseconds between updates
    unsigned long lastUpdate; // last update of position
    
    uint32_t Color1, Color2;  // What colors are in use
    uint16_t TotalSteps;  // total number of steps in the pattern
    uint16_t Index;  // current step within the pattern
    
    void (*OnComplete)();  // Callback on completion of pattern
    
    // Constructor - calls base-class constructor to initialize strip
    NeoPatterns(uint16_t pixels, uint8_t pin, uint8_t type, void (*callback)())
    :Adafruit_NeoPixel(pixels, pin, type)
    {
        OnComplete = callback;
    }

    void Stop()
    {
      ActivePattern = NONE;
    }
    
    // Update the pattern
    void Update()
    {
        if((millis() - lastUpdate) > Interval) // time to update
        {
            lastUpdate = millis();
            switch(ActivePattern)
            {
                case RAINBOW_CYCLE:
                    RainbowCycleUpdate();
                    break;
                case THEATER_CHASE:
                    TheaterChaseUpdate();
                    break;
                case COLOR_WIPE:
                    ColorWipeUpdate();
                    break;
                case SCANNER:
                    ScannerUpdate();
                    break;
                case FADE:
                    FadeUpdate();
                    break;

                case HEARTBEAT:
                    HeartbeatUpdate();
                    break;
                    
                default:
                    break;
            }
        }
    }
  
    // Increment the Index and reset at the end
    void Increment()
    {
        if (Direction == FORWARD)
        {
           Index++;
           if (Index >= TotalSteps)
            {
                Index = 0;
                if (OnComplete != NULL)
                {
                    OnComplete(); // call the comlpetion callback
                }
            }
        }
        else // Direction == REVERSE
        {
            --Index;
            if (Index <= 0)
            {
                Index = TotalSteps-1;
                if (OnComplete != NULL)
                {
                    OnComplete(); // call the comlpetion callback
                }
            }
        }
    }
    
    // Reverse pattern direction
    void Reverse()
    {
        if (Direction == FORWARD)
        {
            Direction = REVERSE;
            Index = TotalSteps-1;
        }
        else
        {
            Direction = FORWARD;
            Index = 0;
        }
    }
    
    // Initialize for a RainbowCycle
    void RainbowCycle(uint8_t interval, int steps,  direction dir = FORWARD)
    {
        ActivePattern = RAINBOW_CYCLE;
        Interval = interval;
        TotalSteps = steps;
        Index = 0;
        Direction = dir;
    }
    
    // Update the Rainbow Cycle Pattern
    void RainbowCycleUpdate()
    {
      const int ColorCount = 16;
      int color = Wheel(((Index * 256 / ColorCount)) & 255);
      ColorSet(color);
      show();
      Increment();

      if (Index == 0) {
        ActivePattern = NONE;
        ColorSet(0);
        show();
      }
    }
 
    // Initialize for a Theater Chase
    void TheaterChase(uint32_t color1, uint32_t color2, uint8_t interval, direction dir = FORWARD)
    {
        ActivePattern = THEATER_CHASE;
        Interval = interval;
        TotalSteps = numPixels();
        Color1 = color1;
        Color2 = color2;
        Index = 0;
        Direction = dir;
   }

   void Heartbeat()
   {
    ActivePattern = HEARTBEAT;
    Interval = 200;
    Color1 = Color(255, 0, 0);
    Color2 = Color(20, 0, 0);
    Index = 0;
    TotalSteps = 8;
    Direction = FORWARD;
   }

   void HeartbeatUpdate()
   {
     switch (Index) {
      case 0:
      case 2:
        ColorSet(Color1);
        break;
        
      case 1:
      case 3:
        ColorSet(Color2);
        break;
     }

     show();
     Increment();
   }
    
    // Update the Theater Chase Pattern
    void TheaterChaseUpdate()
    {
        for(int i=0; i< numPixels(); i++)
        {
            if ((i + Index) % 3 == 0)
            {
                setPixelColor(i, Color1);
            }
            else
            {
                setPixelColor(i, Color2);
            }
        }
        show();
        Increment();
    }
 
    // Initialize for a ColorWipe
    void ColorWipe(uint32_t color, uint8_t interval, direction dir = FORWARD)
    {
        ActivePattern = COLOR_WIPE;
        Interval = interval;
        TotalSteps = numPixels();
        Color1 = color;
        Index = 0;
        Direction = dir;
    }
    
    // Update the Color Wipe Pattern
    void ColorWipeUpdate()
    {
        setPixelColor(Index, Color1);
        show();
        Increment();
    }
    
    // Initialize for a SCANNNER
    void Scanner(uint32_t color1, uint8_t interval)
    {
        ActivePattern = SCANNER;
        Interval = interval;
        TotalSteps = (numPixels() - 1) * 2;
        Color1 = color1;
        Index = 0;
    }
 
    // Update the Scanner Pattern
    void ScannerUpdate()
    { 
        for (int i = 0; i < numPixels(); i++)
        {
            if (i == Index)  // Scan Pixel to the right
            {
                 setPixelColor(i, Color1);
            }
            else if (i == TotalSteps - Index) // Scan Pixel to the left
            {
                 setPixelColor(i, Color1);
            }
            else // Fading tail
            {
                 setPixelColor(i, DimColor(getPixelColor(i)));
            }
        }
        show();
        Increment();
    }
    
    // Initialize for a Fade
    void Fade(uint32_t color1, uint32_t color2, uint16_t steps, uint8_t interval, direction dir = FORWARD)
    {
        ActivePattern = FADE;
        Interval = interval;
        TotalSteps = steps;
        Color1 = color1;
        Color2 = color2;
        Index = 0;
        Direction = dir;
    }
    
    // Update the Fade Pattern
    void FadeUpdate()
    {      
      // Calculate linear interpolation between Color1 and Color2
      // Optimise order of operations to minimize truncation error
      uint8_t red = ((Red(Color1) * (TotalSteps - Index)) + (Red(Color2) * Index)) / TotalSteps;
      uint8_t green = ((Green(Color1) * (TotalSteps - Index)) + (Green(Color2) * Index)) / TotalSteps;
      uint8_t blue = ((Blue(Color1) * (TotalSteps - Index)) + (Blue(Color2) * Index)) / TotalSteps;
      
      ColorSet(Color(red, green, blue));
      show();
      Increment();
      
      if (Index == 0) {
        ActivePattern = NONE;
      }
    }
   
    // Calculate 50% dimmed version of a color (used by ScannerUpdate)
    uint32_t DimColor(uint32_t color)
    {
        // Shift R, G and B components one bit to the right
        uint32_t dimColor = Color(Red(color) >> 1, Green(color) >> 1, Blue(color) >> 1);
        return dimColor;
    }
 
    // Set all pixels to a color (synchronously)
    void ColorSet(uint32_t color)
    {
        for (int i = 0; i < numPixels(); i++)
        {
            setPixelColor(i, color);
        }
        show();
    }
 
    // Returns the Red component of a 32-bit color
    uint8_t Red(uint32_t color)
    {
        return (color >> 16) & 0xFF;
    }
 
    // Returns the Green component of a 32-bit color
    uint8_t Green(uint32_t color)
    {
        return (color >> 8) & 0xFF;
    }
 
    // Returns the Blue component of a 32-bit color
    uint8_t Blue(uint32_t color)
    {
        return color & 0xFF;
    }
    
    // Input a value 0 to 255 to get a color value.
    // The colours are a transition r - g - b - back to r.
    uint32_t Wheel(byte WheelPos)
    {
        WheelPos = 255 - WheelPos;
        if(WheelPos < 85)
        {
            return Color(255 - WheelPos * 3, 0, WheelPos * 3);
        }
        else if(WheelPos < 170)
        {
            WheelPos -= 85;
            return Color(0, WheelPos * 3, 255 - WheelPos * 3);
        }
        else
        {
            WheelPos -= 170;
            return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
        }
    }
};

NeoPatterns LedStrip(8, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800, NULL);

//////////////////////////////////////////////////////////////////////////////////

SoftwareSerial MP3Serial(3, 4);

class TMusic
{
public:
  void SetTrack(int trackNumber)
  { 
    TrackNumber = trackNumber;
    NewTrack = true;
  }
  
  void Update()
  {
    if (IsPlaying) {
      if (millis() >= PlayDeadline) {
        mp3_stop();
        IsPlaying = false;

        if (NewTrack && TrackNumber == 3) {
          // We don't play track number 3 immediately after any other one.
          NewTrack = false;
        }
      }
    } else if (NewTrack) {
      NewTrack = false;
      IsPlaying = true;
      switch (TrackNumber) {
        case 1:
          mp3_play(1); // Проигрываем "mp3/0001.mp3"
          PlayDeadline = millis() + 60000;
          break;
          
        case 2:
          mp3_play(2);
          PlayDeadline = millis() + 25000;
          break;
          
        case 3:
          mp3_play(3);
          PlayDeadline = millis() + 30000;
          break;
        
        case 4:
          mp3_play(4);
          PlayDeadline = millis() + 35000;
          break;
  
         default:
          break;
      }
    }
  }
  
private:  
  int TrackNumber = 1;
  bool NewTrack = false;

  long PlayDeadline;
  bool IsPlaying;
} Music;

//////////////////////////////////////////////////////////////////////////////////

class TSignaler
{
public:
  void SendSignal(int pin)
  {
    digitalWrite(pin, HIGH);
    SignalDeadline = millis() + 1000;
    Pin = pin;
  }

  void Update()
  {
    if (Pin >= 0 && millis() > SignalDeadline) {
      digitalWrite(Pin, LOW);
      Pin = -1;
    }
  }

private:
  int Pin = -1;
  long SignalDeadline;
} Signaler;

//////////////////////////////////////////////////////////////////////////////////

int Mode = 1;

// We do not use timestamp-based gloom now.
long TimestampMode2;
const long GloomDuration = 5L * 60 * 1000;
bool WasGloom = false;

//////////////////////////////////////////////////////////////////////////////////

#define PIN_RINGS 10

#define PIN_FROM_ALCHEMY_OK 5
#define PIN_FROM_ALCHEMY_FAIL 6
#define PIN_FROM_CRYPT 7

#define PIN_ACTIVATE_ALCHEMY A0
#define PIN_ACTIVATE_CRYPT 12

//////////////////////////////////////////////////////////////////////////////////

// Mode 1: wait for rings.
void RunMode1()
{ 
  if (digitalRead(PIN_RINGS) == LOW) {
    StartFadeIn();

    // This fires track number 1.
    Music.SetTrack(1);
  
    TimestampMode2 = millis();  
    Mode = 2;
  
    Serial.println("Sending signal to alchemy machine");
    Signaler.SendSignal(PIN_ACTIVATE_ALCHEMY);
  }
}

// Mode 2: wait for signal from alchemy machine.
void RunMode2()
{
  if (digitalRead(PIN_FROM_ALCHEMY_OK) == LOW) {
    Serial.println("Alchemy machine produced crystall");
    Music.SetTrack(2);
    LedStrip.Heartbeat();

    Serial.println("Sending signal to crypt");
    Signaler.SendSignal(PIN_ACTIVATE_CRYPT);
        
    Mode = 3;
  } else if (digitalRead(PIN_FROM_ALCHEMY_FAIL) == LOW) {
    Serial.println("Alchemy machine was pissed off");
    Music.SetTrack(3);
  }
}

// Mode 3: wait for signal from crypt.
void RunMode3()
{
  if (digitalRead(PIN_FROM_CRYPT) == LOW) {
    Music.SetTrack(4);

    LedStrip.Stop();
    LedStrip.RainbowCycle(100, 300);
    Mode = 4;
  }
}

//////////////////////////////////////////////////////////////////////////////////

void StartFadeIn()
{
  const long R = 14;
  const long G = 119;
  const long B = 174;
    
  const int TotalTime = 10000;
  const int DeltaTime = 50;
  const int N = TotalTime / DeltaTime;
    
  LedStrip.Fade(LedStrip.Color(0, 0, 0), LedStrip.Color(R, G, B), N, DeltaTime);
}

//////////////////////////////////////////////////////////////////////////////////

void setup() {    
  MP3Serial.begin(9600);
  mp3_set_serial(MP3Serial); 
  delay(100);
  mp3_set_volume(25);
  
  Serial.begin(9600);
  
  pinMode(PIN_RINGS, INPUT_PULLUP);
  
  pinMode(PIN_FROM_ALCHEMY_OK, INPUT_PULLUP);  
  pinMode(PIN_FROM_ALCHEMY_FAIL, INPUT_PULLUP);  
  pinMode(PIN_FROM_CRYPT, INPUT_PULLUP);  
  
  pinMode(PIN_ACTIVATE_ALCHEMY, OUTPUT);
  digitalWrite(PIN_ACTIVATE_ALCHEMY, LOW);

  pinMode(PIN_ACTIVATE_CRYPT, OUTPUT);
  digitalWrite(PIN_ACTIVATE_CRYPT, LOW);

  LedStrip.begin();
}

void loop() {
  Music.Update();
  Signaler.Update();
  LedStrip.Update();

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

     case 4:
       break;
       
     default:
       break;
  };
}

//////////////////////////////////////////////////////////////////////////////////
