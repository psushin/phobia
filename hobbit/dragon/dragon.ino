// Input shift register box
#define LATCH A5
#define CLOCK A4
#define DATA A3

#define ARKENSTONE A2

// Emergency button
#define PANIC A1

// Output pins activating 9 dragon segment.
#define SEGMENT_BEGIN 0
#define SEGMENT_END 9

// Output pin disabling magnetic lock.
#define LOCK 9

bool PanicFlag = false;
bool ArkenstoneFlag = false;
bool SuccessFlag = false;

// Time of last dragon activation
long LastFlash = 0;

/////////////////////////////////////////////////////////////////////////////

void flash_segments()
{
  long now = millis();
  if (now - LastFlash < 20 * 1000) {
    return;
  }
  LastFlash = now;
  
  Serial.println("Flash segments");
  for (int i = SEGMENT_BEGIN; i < SEGMENT_END; ++i) {
    digitalWrite(i, LOW);
    delay(500);
    digitalWrite(i, HIGH);
  }
}

void close_everything()
{
  Serial.println("Close everything");
  for (int i = SEGMENT_BEGIN; i < SEGMENT_END; ++i) {
    digitalWrite(i, HIGH);
  }
  digitalWrite(LOCK, HIGH);
  PanicFlag = false;
  SuccessFlag = false;
}

void open_everything()
{
  Serial.println("Open everything");
  for (int i = SEGMENT_BEGIN; i < SEGMENT_END; ++i) {
    digitalWrite(i, LOW);
  }
  digitalWrite(LOCK, LOW);
  SuccessFlag = true;
}

// Fills array of open and closed boxes states.
void get_boxes(bool result[8])
{
  static byte last_state = 0;
 
  digitalWrite(CLOCK, HIGH);
  
  digitalWrite(LATCH, LOW);
  delayMicroseconds(1);
  
  digitalWrite(LATCH, HIGH);
  delayMicroseconds(1);

  byte state = shiftIn(DATA, CLOCK, MSBFIRST);
  bool print = state != last_state; 
  for (int i = 0; i < 8; ++i) {
    result[i] = (bitRead(state, i) == 1);
  }

  if (print) {
    for (int i = 0; i < 8; ++i) {
      Serial.print(result[i] ? "1" : "0");
    }
    Serial.println("");
  }
  last_state = state;
}

// Translates array of box states into array of counters for dragon segments.
void get_counters(const bool boxes[8], int counters[9])
{
  for (int i = 0; i < 9; ++i) {
    counters[i] = 0;
  }

  // 0
  if (boxes[0]) {
    counters[0] ++;
    counters[2] ++;
  }

  // 1
  if (boxes[1]) {
    counters[0] ++;
    counters[8] ++;
  }

  // 2
  if (boxes[2]) {
    counters[1] ++;
    counters[3] ++;
  }

  // 3
  if (boxes[3]) {
    counters[3] ++;
    counters[4] ++;
  }

  // 4
  if (boxes[4]) {
    counters[1] ++;
    counters[5] ++;
  }

  // 5
  if (boxes[5]) {
    counters[6] ++;
    counters[8] ++;
  }

  // 6
  if (boxes[6]) {
    counters[6] ++;
    counters[7] ++;
  }

  // 7
  if (boxes[7]) {
    counters[4] ++;
    counters[5] ++;
    counters[7] ++;
  }
}

/////////////////////////////////////////////////////////////////////////////

void setup()
{
  pinMode(LATCH, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA, INPUT);

  pinMode(PANIC, INPUT);
  pinMode(ARKENSTONE, INPUT);

  for (int i = SEGMENT_BEGIN; i < SEGMENT_END; ++i) {
    pinMode(i, OUTPUT);
  }
 
  pinMode(LOCK, OUTPUT);

  close_everything();

  digitalWrite(LATCH, HIGH); 
  Serial.begin(9600);
}

void loop()
{ 
  if (digitalRead(ARKENSTONE) == HIGH) {
    // No arkenstone.
    if (ArkenstoneFlag) {
      close_everything();  
    }
    ArkenstoneFlag = false;
    return;
  }

  // Double check to eliminate bouncing.
  delay(20);
  if (digitalRead(ARKENSTONE) == HIGH) {
    return;
  }
  
  if (!ArkenstoneFlag) {
    ArkenstoneFlag = true;
    flash_segments();
  }

  if (digitalRead(PANIC) == LOW) {
    if (!PanicFlag) {
      open_everything();
    }
    PanicFlag = true;
    return;
  } 

  if (!SuccessFlag) {
    bool boxes[8];
    get_boxes(boxes);
    
    int counters[9];
    get_counters(boxes, counters);

    bool allSegments = true;
    for (int i = 0; i < 9; ++i) {
      //Serial.print(counters[i]);
      //Serial.print(" ");
      if (counters[i] % 2 == 1) {
        digitalWrite(SEGMENT_BEGIN + i, LOW);
      } else {
        digitalWrite(SEGMENT_BEGIN + i, HIGH);
        allSegments = false;
      }
    }
    //Serial.println();

    if (allSegments) {
      Serial.println("All segments");
      open_everything();
    }
  }
}
