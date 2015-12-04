/* Пины, к которым подключен энкодер */
//enum { ENC_PIN1 = A2, ENC_PIN2 = A3 };


const int ENC_PIN1 =  A2;
const int ENC_PIN2 =  A3;

const int LED = 13;

const int Rel_C1 = 3;
const int Rel_C2 = 4;
const int Rel_C3 = 5;
const int Rel_Fire = 6;
const int Rel_Lock = 7;

int retete = 100;

int rate_up = 0 ;
int rate_down = 0;

int states = 0;


void setup()
{
  pinMode(ENC_PIN1, INPUT);
  pinMode(ENC_PIN2, INPUT);

  digitalWrite(Rel_C1, LOW);
  digitalWrite(Rel_C2, LOW);
  digitalWrite(Rel_C3, LOW);
  digitalWrite(Rel_Fire, LOW);
  digitalWrite(Rel_Lock, LOW);

  pinMode(Rel_C1, OUTPUT);
  pinMode(Rel_C2, OUTPUT);
  pinMode(Rel_C3, OUTPUT);
  pinMode(Rel_Fire, OUTPUT);
  pinMode(Rel_Lock, OUTPUT);

  Serial.begin(9600);
  Serial.println("Start...");
  delay(1000);

  //

  digitalWrite(Rel_C1, LOW);
  digitalWrite(Rel_C2, LOW);
  digitalWrite(Rel_C3, LOW);

  delay(500);

  digitalWrite(Rel_Fire, HIGH);
  delay(900);
  digitalWrite(Rel_Fire, LOW);
  delay(500);

}

/* Функция декодирования кода Грея, взятая с Википедии.
* Принимает число в коде Грея, возвращает обычное его представление.
*/
unsigned graydecode(unsigned gray)
{
  unsigned bin;

  for (bin = 0; gray; gray >>= 1)
    bin ^= gray;

  return bin;
}

void loop()
{
  static uint8_t previous_code = 0; // предыдущий считанный код
  uint8_t gray_code = digitalRead(ENC_PIN1) | (digitalRead(ENC_PIN2) << 1),
          code = graydecode(gray_code);

  /* Начало */
  if (code == 0 && states == 0)
  {
    /* Если переход к нулю был из состояния 3 - ручка вращалась
     * по часовой стрелке, если из 1 - против.
     */
    if (previous_code == 3) {
      Serial.println("Yes ->");
      states = 1;
    }
    else if (previous_code == 1) {
      Serial.println("<-");
    }
  }

  /* Поворот в плюс! */

  if (code == 0 && states == 1)
  {
    /* Если переход к нулю был из состояния 3 - ручка вращалась
     * по часовой стрелке, если из 1 - против.
     */
    if (previous_code == 3) {
      Serial.println("->");
      rate_up = rate_up + 1;

    }
    else if (previous_code == 1) {
      Serial.println("<-");
      rate_down = rate_down + 1;
    }
    Serial.println("Rotari N 1");
    Serial.println(rate_up);
    Serial.println(rate_down);

    if ( rate_up == 6 )
    {
      rate_up = 0;
      rate_down = 0;
      states = 2;
      Serial.println("Open 1");
      delay(100);
      digitalWrite(Rel_C1, HIGH);
      digitalWrite(Rel_C2, LOW);
      digitalWrite(Rel_C3, LOW);
      delay(100);
      digitalWrite(Rel_Fire, HIGH);
      delay(900);
      digitalWrite(Rel_Fire, LOW);
      delay(500);
    }
    if ( rate_down == 2 )
    {
      Serial.println("Error");
      rate_up = 0;
      rate_down = 0;
      states = 0;

      digitalWrite(Rel_C1, LOW);
      digitalWrite(Rel_C2, LOW);
      digitalWrite(Rel_C3, LOW);
      delay(100);
      digitalWrite(Rel_Fire, HIGH);
      delay(900);
      digitalWrite(Rel_Fire, LOW);
      delay(500);
    }
  }

  /* Поворот в минус! */

  if (code == 0 && states == 2)
  {
    /* Если переход к нулю был из состояния 3 - ручка вращалась
     * по часовой стрелке, если из 1 - против.
     */
    if (previous_code == 3) {
      Serial.println("->");
      rate_up = rate_up + 1;

    }
    else if (previous_code == 1) {
      Serial.println("<-");
      rate_down = rate_down + 1;
    }
    Serial.println("Rotari N 2");
    Serial.println(rate_up);
    Serial.println(rate_down);

    if ( rate_down == 12 )
    {
      rate_up = 0;
      rate_down = 0;
      states = 3;
      Serial.println("Open 2");
      delay(100);
      digitalWrite(Rel_C1, HIGH);
      digitalWrite(Rel_C2, HIGH);
      digitalWrite(Rel_C3, LOW);
      delay(100);
      digitalWrite(Rel_Fire, HIGH);
      delay(900);
      digitalWrite(Rel_Fire, LOW);
      delay(500);
    }
    if ( rate_up == 3 )
    {
      Serial.println("Error");
      rate_up = 0;
      rate_down = 0;
      states = 0;

      digitalWrite(Rel_C1, LOW);
      digitalWrite(Rel_C2, LOW);
      digitalWrite(Rel_C3, LOW);
      delay(100);
      digitalWrite(Rel_Fire, HIGH);
      delay(900);
      digitalWrite(Rel_Fire, LOW);
      delay(500);
    }
  }

  /* Поворот в плюс 2! */

  if (code == 0 && states == 3)
  {
    /* Если переход к нулю был из состояния 3 - ручка вращалась
     * по часовой стрелке, если из 1 - против.
     */
    if (previous_code == 3) {
      Serial.println("->");
      rate_up = rate_up + 1;

    }
    else if (previous_code == 1) {
      Serial.println("<-");
      rate_down = rate_down + 1;
    }
    Serial.println("Rotari N 3");
    Serial.println(rate_up);
    Serial.println(rate_down);

    if ( rate_up == 8 )
    {
      rate_up = 0;
      rate_down = 0;
      states = 4;
      Serial.println("Open 3");
      delay(100);
      digitalWrite(Rel_C1, HIGH);
      digitalWrite(Rel_C2, HIGH);
      digitalWrite(Rel_C3, HIGH);
      delay(100);
      digitalWrite(Rel_Fire, HIGH);
      delay(900);
      digitalWrite(Rel_Fire, LOW);
      delay(500);
      digitalWrite(Rel_Lock, HIGH);
      delay(500);
      Serial.println("Stop");
    }
    if ( rate_down == 3 )
    {
      Serial.println("Error");
      rate_up = 0;
      rate_down = 0;
      states = 0;

      digitalWrite(Rel_C1, LOW);
      digitalWrite(Rel_C2, LOW);
      digitalWrite(Rel_C3, LOW);
      delay(100);
      digitalWrite(Rel_Fire, HIGH);
      delay(900);
      digitalWrite(Rel_Fire, LOW);
      delay(500);
    }
  }
  previous_code = code;
  delay(1);
}
