#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#define SAMPLE_RATE 8000
#include "sounddata.h"
volatile uint16_t sample;
byte lastSample;
String  sen;

const int speakerPin = 3;
const int relPinL =  4; // Управление реле 220
const int relPinT =  5; // Сигнал на телефон
const int ledPin = 13;
const int sensorMon = A3;
const int sensorProb = A2;

int sensorMonV = 0;
int sensorProbV = 0;

int statM = 0;

void stopPlayback() {
  TIMSK1 &= ~_BV(OCIE1A);
}

ISR(TIMER1_COMPA_vect) {
  if (sample >= sounddata_length) {
    if (sample == sounddata_length + lastSample) {
      stopPlayback();
    }
    else {
      OCR2B = sounddata_length + lastSample - sample ;
    }
  }
  else {
    OCR2B = pgm_read_byte(&sounddata_data[sample]);
  }

  ++sample;
}

void startPlayback() {
  ASSR &= ~(_BV(EXCLK) | _BV(AS2));
  TCCR2A |= _BV(WGM21) | _BV(WGM20);
  TCCR2B &= ~_BV(WGM22);
  TCCR2A = (TCCR2A | _BV(COM2B1)) & ~_BV(COM2B0);
  TCCR2A &= ~(_BV(COM2A1) | _BV(COM2A0));
  TCCR2B = (TCCR2B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);
  OCR2B = pgm_read_byte(&sounddata_data[0]);
  cli();
  TCCR1B = (TCCR1B & ~_BV(WGM13)) | _BV(WGM12);
  TCCR1A = TCCR1A & ~(_BV(WGM11) | _BV(WGM10));
  TCCR1B = (TCCR1B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);
  OCR1A = F_CPU / SAMPLE_RATE;
  TIMSK1 |= _BV(OCIE1A);
  lastSample = pgm_read_byte(&sounddata_data[sounddata_length - 1]);
  sample = 0;
  sei();
}



void setup() {
  pinMode(speakerPin, OUTPUT);
  pinMode(relPinL, OUTPUT);
  pinMode(relPinT, OUTPUT);
  digitalWrite(relPinL, HIGH);
  digitalWrite(relPinT, HIGH);
  Serial.begin(9600);
  delay(700);
  Serial.println("Start!");
}

void loop() {
labelend:
  delay(10);
  Serial.println("Loop>");

  if (statM == 2) {
    sensorProbV = analogRead(sensorProb);

    Serial.print("sensorProb-END = ");
    Serial.println(sensorProbV);
    if ( sensorProbV > 150 ) {
      digitalWrite(relPinL, LOW);
    } else {
      digitalWrite(relPinL, HIGH);
    }
  }

  if (statM == 0) {
    sensorMonV = analogRead(sensorMon);
    sensorProbV = analogRead(sensorProb);
    Serial.print("sensorMon = ");
    Serial.println(sensorMonV);
    Serial.print("sensorProb = ");
    Serial.println(sensorProbV);


    if ( sensorProbV > 150 ) {
      digitalWrite(relPinL, LOW);
    } else {
      digitalWrite(relPinL, HIGH);
    }
    if (sensorMonV > 150 ) {
      statM = 1;
    }
  }

  if (statM == 1) {
    sensorProbV = analogRead(sensorProb);
    Serial.println ("Wait...ON 1");
    if ( sensorProbV > 150 ) {
      digitalWrite(relPinL, LOW);
      for (int i1 = 0; i1 <= 900; i1++) {
        Serial.println ("Wait... OFF 1");
        sensorProbV = analogRead(sensorProb);
        delay(1);
        if (sensorProbV < 500) {
          i1 = 900;
          digitalWrite(relPinL, HIGH);
          for (int i2 = 0; i2 <= 900; i2++) {
            sensorProbV = analogRead(sensorProb);
            Serial.println ("Wait...ON 2");
            delay(1);
            if ( sensorProbV > 150 ) {
              i2 = 900;
              digitalWrite(relPinL, LOW);
              for (int i3 = 0; i3 <= 900; i3++) {
                Serial.println ("Wait... OFF 2");
                sensorProbV = analogRead(sensorProb);
                delay(1);
                if (sensorProbV < 500) {
                  i3 = 900;
                  digitalWrite(relPinL, HIGH);
                  for (int i4 = 0; i4 <= 900; i4++) {
                    sensorProbV = analogRead(sensorProb);
                    Serial.println ("Wait...ON 3");
                    delay(1);
                    if ( sensorProbV > 150 ) {
                      i4 = 900;
                      digitalWrite(relPinL, LOW);
                      for (int i5 = 0; i5 <= 900; i5++) {
                        Serial.println ("Wait... OFF 3");
                        sensorProbV = analogRead(sensorProb);
                        delay(1);
                        if (sensorProbV < 500) {
                          i5 = 900;
                          digitalWrite(relPinL, HIGH);
                          for (int i6 = 0; i6 <= 900; i6++) {
                            sensorProbV = analogRead(sensorProb);
                            Serial.println ("Wait...ON 4");
                            delay(1);
                            if ( sensorProbV > 150 ) {
                              i6 = 900;
                              digitalWrite(relPinL, LOW);
                              for (int i7 = 0; i7 <= 900; i7++) {
                                Serial.println ("Wait... OFF 4");
                                sensorProbV = analogRead(sensorProb);
                                delay(1);
                                if (sensorProbV < 500) {
                                  i7 = 900;
                                  digitalWrite(relPinL, HIGH);
                                  for (int i8 = 0; i8 <= 900; i8++) {
                                    sensorProbV = analogRead(sensorProb);
                                    Serial.println ("Wait...ON 5");
                                    delay(1);
                                    if ( sensorProbV > 150 ) {
                                      i8 = 900;
                                      digitalWrite(relPinL, LOW);
                                      Serial.println ("BipBip");
                                      startPlayback();
                                      delay(700);
                                      startPlayback();
                                      delay(1000);
                                      Serial.println ("START PHONE");
                                      digitalWrite(relPinT, LOW);
                                      delay(1000);
                                      digitalWrite(relPinT, HIGH);
                                      Serial.println ("END");
                                      statM = 2;
                                      goto labelend;
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}













