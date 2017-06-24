#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>

int sensor = 13; // входной сигнал от датчика движения
int chain = 11; // входной сигнал от задачи с цепями
int step = 8; // шаг
int mind_breaker = 4; //сигнал о закрытии
int dir = 9; // направление
int vel = 500; // задержка скорости движения двери
long pos = 0; //начальная позиция двери
long n = 51660; // максимальная позиция двери
bool was_opened = true;
bool was_closed = false;
bool firstscan = true;
int light = 5; //твердотельное реле для выключения света
int next_door = 12; //на реле двери
int OPEN = 2; //кнопка открытия
int CLOSE = 6; //кнопка закрытия


void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);

  mp3_set_serial(Serial); //set Serial for DFPlayer-mini mp3 module
  mp3_set_volume(30);

  pinMode(OPEN, INPUT_PULLUP);
  pinMode(CLOSE, INPUT_PULLUP);
  pinMode(sensor, INPUT); // указываем, что ожидается сигнал от датчика движения
  pinMode(chain, INPUT); // указываем, что ожидается сигнал от решения задачи с цепями
  pinMode(mind_breaker, INPUT_PULLUP);
  pinMode(dir, OUTPUT);
  pinMode(step, OUTPUT);
  pinMode(light, OUTPUT);
  pinMode(next_door, OUTPUT);
  pinMode(A0, INPUT_PULLUP); //сигнал о решении щита
  pinMode(A3, OUTPUT); //сигнал активаци щита
  digitalWrite(A3, LOW);
  digitalWrite(next_door, HIGH);
}

void loop()
{
  if (firstscan == true) {
    vel = 700;
    pos = 51660;
    digitalWrite(dir, LOW); // CLOSE direction
    while (pos > 0) {
      digitalWrite(step, HIGH);
      delayMicroseconds(vel);
      digitalWrite(step, LOW);
      delayMicroseconds(vel);
      pos--;
      if (vel > 200) vel--;
    }

    vel = 700;
    digitalWrite(dir, HIGH); // OPEN direction
    while (pos < 51660) {
      digitalWrite(step, HIGH);
      delayMicroseconds(vel);
      digitalWrite(step, LOW);
      delayMicroseconds(vel);
      pos++;
      if (vel > 200) vel--;
    }

    digitalWrite(light, HIGH);

    firstscan = false;
  }
  
  if (digitalRead(CLOSE) == false) { 
     delay(10);
     if (digitalRead(CLOSE) == false) {
      vel = 700;
      digitalWrite(dir, LOW); // CLOSE direction
      while (pos > 0) {
        digitalWrite(step, HIGH);
        delayMicroseconds(vel);
        digitalWrite(step, LOW);
        delayMicroseconds(vel);
        pos--;
        if (vel > 200) vel--;
      }

      was_opened = false;
      was_closed = false;
      digitalWrite(light, LOW);
      digitalWrite(A3, LOW);
    }
  }

  if (digitalRead(OPEN) == false) {
    delay(10);
    if (digitalRead(OPEN) == false)
    {
      vel = 700;
      digitalWrite(dir, HIGH); // OPEN direction
      while (pos < 51660) {
        digitalWrite(step, HIGH);
        delayMicroseconds(vel);
        digitalWrite(step, LOW);
        delayMicroseconds(vel);
        pos++;
        if (vel > 200) vel--;
      }
      digitalWrite(A3, LOW);
      digitalWrite(light, HIGH);
    }
  }

  if (digitalRead(chain) == true && was_opened == false) {
    delay(500);
    if (digitalRead(chain) == true && was_opened == false)
    { digitalWrite(light, HIGH);
      open_door();
      was_opened = true;
    }
  }
  if (digitalRead(mind_breaker) == false && !was_closed) {
    delay(200);
    if (digitalRead(mind_breaker) == false && !was_closed)
    {
      digitalWrite(light, LOW);
      digitalWrite(A3, HIGH); //сигнал активации щита
      delay(500);
      digitalWrite(A3, LOW);
      for (;;) {
        if (digitalRead(A0) == LOW) {
          delay(200);
          if (digitalRead(A0) == LOW) {
            break;
          }
        }

        delay(10);
      }
      while (digitalRead(A0) == HIGH)
      { }

      digitalWrite(light, HIGH);
      digitalWrite(next_door, LOW);
      delay(10000);
      mp3_play();
      delay(10000);
      close_door();
      digitalWrite(next_door, HIGH);
      was_closed = true;
    }
  }
}

void open_door()
{
  vel = 700;
  digitalWrite(dir, HIGH);
  while (pos < 51660) {
    digitalWrite(step, HIGH);
    delayMicroseconds(vel);
    digitalWrite(step, LOW);
    delayMicroseconds(vel);
    pos++;
    if (vel > 200) vel--;
  }
}

void close_door()
{
  vel = 700;
  digitalWrite(dir, LOW); // CLOSE direction
  while (pos > 0) {
    if (digitalRead(sensor) != 1) {
      digitalWrite(step, HIGH);
      delayMicroseconds(vel);
      digitalWrite(step, LOW);
      delayMicroseconds(vel);
      pos--;
      if (vel > 200) vel--;
    } else {
      vel = 700;
    }
  }
}
