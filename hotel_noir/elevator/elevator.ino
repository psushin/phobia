#include <Servo.h>

#define SD             2            // Подсветка
#define EXT            3            // Внешний сигнал перехода в режим с отваливающейся стрелкой
#define BUTTON         4            // Кнопка вызова
#define SERVO_S        6            // Сигнал управления сервоприводом
#define SP             11           // Сигнал управления моторчиком/динамиком
#define MGN            12           // Сигнал включения магнита

#define led_on                (digitalWrite(SD, LOW), LED_ENABLED = 1)  //выключение подсветки
#define led_off               (digitalWrite(SD, HIGH), LED_ENABLED = 0) //включение подсветки

#define mgn_off               digitalWrite(MGN, LOW)  //выключение магнита
#define mgn_on                digitalWrite(MGN, HIGH) //включение магнита


//myservo.write(pos);

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int angle = 0;    // variable to store the servo position

volatile unsigned char floor1_mode_on = 0; //флаг включения режима доступа на первый этаж
volatile unsigned char LED_BLINKING = 0, LED_ENABLED = 0; //флаг включения режима мигания подсветки, и флаг сигнализирующий и состоянии подсветки
unsigned char msg1_pinted = 0, button_pushed = 0, active_mode = 1;
unsigned long old_time = 0, new_time = 0, timeout = 0;
unsigned long blink_old_time = 0, blink_new_time = 0, blink_timeout = 0;
unsigned long random_delay = 0;

void floor_enable()
{
  floor1_mode_on = 1;
  LED_BLINKING = 1;
  led_off;            //выключение подсветки
}

void setup()
{
  //Инициализируем генератор случайных чисел
  randomSeed(analogRead(0));
  random_delay = (unsigned long) random(2000, 5000);
  // конфигурируем порты

  pinMode(SD, OUTPUT); // настроить как выход
  led_off;            // и подать логический 0

  pinMode(EXT, INPUT); // настроить как вход

  //pinMode(BUTTON, INPUT_PULLUP); // настроить как вход
  pinMode(BUTTON, INPUT); // настроить как вход
  //digitalWrite(BUTTON, LOW); // притянуть к земле

  pinMode(MGN, OUTPUT); // настроить как выход
  mgn_off;             // выключить магнит

  Serial.begin(9600); // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)

  myservo.attach(SERVO_S);  // attaches the servo on pin 9 to the servo object

  Serial.println("Lift Proramm Started");
  delay(50);

  for (angle = 0; angle <= 180; angle += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    myservo.write(angle);              // tell servo to go to position in variable 'pos'
    delay(66);                       // waits 15ms for the servo to reach the position
  }

  //установка стрелки
  analogWrite(SP, 255); //включаем моторчик
  delay(100);
  analogWrite(SP, 80); //включаем моторчик
  delay(1000);  //дать сигнал, что стартовали
  analogWrite(SP, 0); //вылючаем моторчик
  delay(5000); //дать время оператору установить стрелку
  mgn_on;                                    //включить магнит
  led_on;                                    //включить подсветку
  analogWrite(SP, 255); //включаем моторчик
  delay(100);
  analogWrite(SP, 80); //включаем моторчик
  delay(1000);  //дать сигнал, что что магнит зафиксирован
  analogWrite(SP, 0); //вылючаем моторчик
  delay(5000); //дать время оператору убрать руки


  //от 8 до 1 и обратно
  for (angle = 180; angle >= 12; angle -= 1) // goes from 180 degrees to 0 degrees
  {
    myservo.write(angle);              // tell servo to go to position in variable 'pos'
    delay(66);                       // waits 15ms for the servo to reach the position
  }

  for (angle = 12; angle <= 180; angle += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    myservo.write(angle);              // tell servo to go to position in variable 'pos'
    delay(66);                       // waits 15ms for the servo to reach the position
  }

  /*
    //от 8 до 2 и обратно
    for(angle = 180; angle>=30; angle-=1)     // goes from 180 degrees to 0 degrees
    {
    myservo.write(angle);              // tell servo to go to position in variable 'pos'
    delay(66);                       // waits 15ms for the servo to reach the position
    }

    for(angle = 30; angle <= 180; angle += 1) // goes from 0 degrees to 180 degrees
    {                                  // in steps of 1 degree
    myservo.write(angle);              // tell servo to go to position in variable 'pos'
    delay(66);                       // waits 15ms for the servo to reach the position
    }
  */
  attachInterrupt(1, floor_enable, FALLING); //включаем прерывание по спаду на Int1, обработчик "floor_enable"
  Serial.println("Arrow prepared");
  delay(50);

  //LED_BLINKING = 1;
  //led_off;            //выключение подсветки

}

void loop()
{
  if (active_mode)
  {
    if (floor1_mode_on && !msg1_pinted)
    {
      Serial.println("External signal received");
      delay(50);
      msg1_pinted = 1;
    }

    //Serial.println(digitalRead(BUTTON));
    //delay(50);

    if (LED_BLINKING)
    {

      blink_new_time = millis();                                           //задержка на мигание
      if ((blink_new_time - blink_old_time) >= 0) blink_timeout = blink_new_time - blink_old_time;
      else blink_timeout = 0xffffffff - blink_old_time + blink_new_time;

      if (blink_timeout > 800)
        led_off; //если подсветка горит больше 0,8с - погасить

      if (blink_timeout > random_delay)
      {
        //if(LED_ENABLED) led_off;                        //выключить подсветку
        //else led_on;                                    //включить подсветку

        //Serial.println(blink_timeout);
        //Serial.println(random_delay);
        //delay(50);

        led_on;

        blink_old_time = millis();
        random_delay = (unsigned long) random(2000, 5000); //новое значение задержки перед очередным миганием
      }

    }

    if (digitalRead(BUTTON) && !button_pushed) //если нажата кнопка
    {
      new_time = millis();                                           //защта от дребезга
      if ((new_time - old_time) >= 0) timeout = new_time - old_time;
      else timeout = 0xffffffff - old_time + new_time;

      if (timeout > 50)
      {
        button_pushed = 1;
        Serial.println("Button pushed");
        delay(50);
      }
    }
    else
    {
      old_time = millis();
    }

    if (button_pushed)
    {
      if (floor1_mode_on)
      {

        LED_BLINKING = 0;       //выключение режима мигания
        led_off;            //выключение подсветки

        analogWrite(SP, 255); //включаем моторчик
        delay(100);
        analogWrite(SP, 80); //включаем моторчик
        for (angle = 180; angle >= 12; angle -= 1) // goes from 0 degrees to 180 degrees
        { // in steps of 1 degree
          myservo.write(angle);              // tell servo to go to position in variable 'pos'
          delay(66);                       // waits 15ms for the servo to reach the position
        }
        analogWrite(SP, 0); //вылючаем моторчик
        delay(2000);
        mgn_off;             //выключение магнита
        active_mode = 0;
      }
      else
      {
        analogWrite(SP, 255); //включаем моторчик
        delay(100);
        analogWrite(SP, 80); //включаем моторчик
        for (angle = 180; angle >= 30; angle -= 1) // goes from 0 degrees to 180 degrees
        { // in steps of 1 degree
          myservo.write(angle);              // tell servo to go to position in variable 'pos'
          delay(66);                       // waits 15ms for the servo to reach the position
        }
        analogWrite(SP, 0); //вылючаем моторчик

        delay(5000);

        analogWrite(SP, 255); //включаем моторчик
        delay(100);
        analogWrite(SP, 80); //включаем моторчик
        for (angle = 30; angle <= 180; angle += 1) // goes from 180 degrees to 0 degrees
        {
          myservo.write(angle);              // tell servo to go to position in variable 'pos'
          delay(66);                       // waits 15ms for the servo to reach the position
        }
        analogWrite(SP, 0); //вылючаем моторчик
      }
    }
    button_pushed = 0;
  }

}
