int sensor = 13; // входной сигнал от датчика движения
int chain = 3;  //  входной сигнал от задачи с цепями
int step = 8; // шаг
int shield = 4; //сигнал о закрытии
int dir = 9; // направление
int vel = 500; // задержка скорости движения двери
long pos = 0;  //начальная позиция двери
long n = 51660; // максимальная позиция двери
bool was_opened = false;
bool was_closed = false;
bool firstscan = true;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(sensor, INPUT); // указываем, что ожидается сигнал от датчика движения
  pinMode(chain, INPUT); // указываем, что ожидается сигнал от решения задачи с цепями
  pinMode(shield, INPUT);
  pinMode(dir, OUTPUT);
  pinMode(step, OUTPUT);

}


void loop() {
  if (firstscan == true)
  {
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
    } firstscan = false;
  }
  // put your main code here, to run repeatedly:
  if (digitalRead(chain) == true && was_opened==false) {
    delay(1000);
    if (digitalRead(chain) == true && was_opened==false)
      {
        open_door();
        was_opened = true;
      
  }}
  if (digitalRead(shield) == true && was_closed == false) {
    delay(400);
    if (digitalRead(shield) == true && was_closed == false)
      {
        close_door();
        was_closed = true;
      }
  }
}

void open_door() {
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

void close_door() {
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
    } else vel = 700;
  }
}
