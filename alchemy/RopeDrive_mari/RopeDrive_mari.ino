int step = 7;
int dir = 8;
int pos = 0;
int n = 3370; //
int dest; // текущее положение прибора в шагах
int maxdest = 10; // самое левое допустимое положение прибора в шагах
int mindest = 0; // самое правое допустимое положение прибора в шагах
int vel = 900;
int rope1 = 12;
int rope2 = 11;
int rope3 = 10;
int todoor =3;
bool firstscan = 1;

void one_step(bool step_direction) { //step_direction - направление движения: 1 - to left, 0 - right
  vel = 900;
  // подача крутящего момента на двигатель влево (HIGH) или вправо (LOW) и задание одного шага
  if (step_direction) {
    digitalWrite(dir, HIGH);
    n = 3370;
  } else {
    digitalWrite(dir, LOW);
    n = -3370;
  }
  pos = 0; //делает движение относительным
  while (pos != n) {
    digitalWrite(step, HIGH);
    delayMicroseconds(vel);
    digitalWrite(step, LOW);
    delayMicroseconds(vel);
    if (pos > n) pos--; else pos++;
    if (vel > 300) vel--;
  }
  delay(500);
}


void setup() {
  Serial.begin(9600);
  pinMode(step, OUTPUT);
  pinMode(todoor,OUTPUT);
  pinMode(dir, OUTPUT);
  pinMode(rope1, INPUT);
  pinMode(rope2, INPUT);
  pinMode(rope3, INPUT);
  dest = 0;
  vel = 900;
  digitalWrite(dir, LOW);
  digitalWrite(todoor,LOW);
  for (int i=10; i > 0; i--){
    one_step(0);
  }
  
}


// один шаг - 3370 (n), всего участков на рейке 33700
// всего 10 шагов
// начальная отметка 0 (правый край), максимальная = 33700( левый край)

void loop() {
  if (digitalRead(rope1) == 1) {
    Serial.println("+2 steps (to right)"); 
    Serial.println(dest);
    if (dest<9) { 
      one_step(1); 
      one_step(1);
      dest = dest + 2;
    }
  };
  if (digitalRead(rope2) == 1) {
    Serial.println("+4 steps (to rigth)");
    Serial.println(dest);
    if (dest<7) {
      one_step(1);
      one_step(1);
      one_step(1);
      one_step(1);
      dest = dest + 4;
    }
  };
  if (digitalRead(rope3) == 1) {
    Serial.println("-3 steps (to left)");
    Serial.println(dest);
    if (dest>2) {
      one_step(0);
      one_step(0);
      one_step(0);
      dest = dest - 3;
    }
  };
  if (dest == 5){
    digitalWrite(todoor, HIGH);
    Serial.println("Goooooooooooooood");
  } else {
    digitalWrite(todoor, LOW);
    Serial.println("Baaaaaaaaaaaaaaaad");
  };
}  
