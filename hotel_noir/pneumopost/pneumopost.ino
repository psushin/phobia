#define RIGHT_MAGNET A0

#define LEFT_DOOR_LED A1
#define RIGHT_DOOR_LED A2

#define TABLE_ACTIVATED_INPUT A3  // Датчик закрытия двери 1
#define LEFT_DOOR_CLOSED_INPUT A4             // Подготовка к работе
#define CIGARETTE_INPUT A5         // Датчик установленной бутылки в двери

// Pins 0 - 7 reserved for keypad
// 8 - empty  
#define ALARM_TRIGGER_OUTPUT 9
#define LEFT_MAGNET 10
#define RIGHT_SERVO 11
#define LEFT_SERVO 12
#define CASH_REGISTER_INPUT 13  // Сигнал с кассового аппарата

#include "leOS.h" 
leOS myOS;

#include <Servo.h>
Servo leftDoorServo;  // Привод двери 1
Servo rightDoorServo;  // Привод двери 2

#include <Keypad.h> 

int flag1 = 0;
int flag2 = 0;
int flag3 = 0; 
int flag4 = 0;

const byte ROWS = 4; //four rows 
const byte COLS = 4; //four columns  
//define the cymbols on the buttons of the keypads 
char hexaKeys[ROWS][COLS] = { 
  {'1', '2', '3', 'A'},  
  {'4', '5', '6', 'B'},  
  {'7', '8', '9', 'C'},  
  {'*', '0', '#', 'D'} 
}; 

byte rowPins[ROWS] = {0, 1, 2, 3}; //connect to the row pinouts of the keypad  
byte colPins[COLS] = {4, 5, 6, 7}; //connect to the column pinouts of the keypad 

//initialize an instance of class NewKeypad  
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

int Kassa = 0;
int Kassa_1 = 0;
int Enter;      // Нажатие кнопки Enter

int LeftDoorClosed = 0;  // Состояние датчика двери 1 
int Vx_3_st = 0;  // Состояние датчика установленной бутылки в двери
int Vx_4_st = 0;  // Сигнал с кассового аппарата
int Vx_7_st = 0;

byte led1Status = 0;
byte led2Status = 0;
byte led0Status = 0;

void setup() {
  leftDoorServo.attach(LEFT_SERVO);
  rightDoorServo.attach(RIGHT_SERVO);

  leftDoorServo.write(0);
  rightDoorServo.write(0);

  pinMode(ALARM_TRIGGER_OUTPUT, OUTPUT);  
  pinMode(RIGHT_MAGNET, OUTPUT); // выход  электромагнит
  pinMode(LEFT_MAGNET, OUTPUT);
  pinMode(LEFT_DOOR_LED, OUTPUT); // выход светодиод двери 1
  pinMode(RIGHT_DOOR_LED, OUTPUT); // выход светодиод двери 2

  pinMode(TABLE_ACTIVATED_INPUT, INPUT);
  pinMode(LEFT_DOOR_CLOSED_INPUT, INPUT);
  pinMode(CIGARETTE_INPUT, INPUT);
  pinMode(CASH_REGISTER_INPUT, INPUT);

  digitalWrite(RIGHT_MAGNET, LOW);
  digitalWrite(LEFT_MAGNET, LOW);

  digitalWrite(LEFT_DOOR_LED, LOW);
  digitalWrite(RIGHT_DOOR_LED, LOW);
  digitalWrite(ALARM_TRIGGER_OUTPUT, LOW);

  myOS.begin();     
  myOS.addTask(Key, 1);
  //myOS.addTask(flashLed1, 100);
  //myOS.addTask(flashLed2, 200);
  //myOS.addTask(flashLed0, 150);
}

void loop() 
{
  if (Kassa == 1) {
    // Close both doors.
    delay(1000);
    rightDoorServo.write(140);
    leftDoorServo.write(140);
    digitalWrite(RIGHT_DOOR_LED, LOW); 
    digitalWrite(LEFT_DOOR_LED, LOW); 
    flag1 = 0;

    // Table sensor.
    LeftDoorClosed = digitalRead(TABLE_ACTIVATED_INPUT); 
    
    if (LeftDoorClosed == LOW) {
      // Here
      delay(5000);
      digitalWrite(LEFT_MAGNET, LOW);
      delay(2500);
      leftDoorServo.write(0); 
      digitalWrite(LEFT_DOOR_LED, HIGH);

      General_restaet_2:
      Vx_4_st = digitalRead(CASH_REGISTER_INPUT);
      if (Vx_4_st == HIGH) {
        Kassa_1 = 1; 
          
           
          // Kassa=0;
        // goto  General_restaet_2:
        General_restaet_1:
       //delay(100);
         // if (Vx_2_st == HIGH) 
             {
               if (Enter==1) {
                 if (flag1 == 3 && flag2 == 1 && flag3 == 1 && flag4 == 1 && Kassa_1 == 1)
                    {
                      Vx_3_st = digitalRead(CIGARETTE_INPUT);
                          // delay(100);
                        if (Vx_3_st == LOW)
                         {
                            // Sending the package.
                            leftDoorServo.write(140); 
                            digitalWrite(LEFT_DOOR_LED, LOW);
                            // Here goes the sound.
                            delay(3000);
                            digitalWrite(RIGHT_MAGNET, HIGH); 
                            delay(2000);
                            rightDoorServo.write(0);
                            digitalWrite(RIGHT_DOOR_LED, HIGH); 
                            flag1 = 0;
                            flag2 = 0;
                            flag3 = 0; 
                            flag4 = 0;
                            digitalWrite(ALARM_TRIGGER_OUTPUT, HIGH);
                            delay(3000);
                            digitalWrite(ALARM_TRIGGER_OUTPUT, LOW);
                            //goto General_restart;
                         }
                         else
                         {
                           leftDoorServo.write(140); 
                           digitalWrite(LEFT_DOOR_LED, LOW); 
                           delay(5000); 
                           leftDoorServo.write(0);
                           digitalWrite(LEFT_DOOR_LED, HIGH);
                           Enter = 0;
                           flag1 = 0;
                           flag2 = 0;
                           flag3 = 0; 
                           flag4 = 0;
                           goto General_restaet_1;
                         }
                    }

                else  
                    {
                      leftDoorServo.write(140); 
                      digitalWrite(LEFT_DOOR_LED,LOW); 
                      delay(5000); 
                      leftDoorServo.write(0);
                      digitalWrite(LEFT_DOOR_LED,HIGH);
                      Enter=0;
                      flag1=0;
                      flag2=0;
                      flag3=0; 
                      flag4=0;
                      goto General_restaet_1;
                     }
                }               
             //}
             else
             {
              goto General_restaet_1;
                         
             }
            
           }
        }
        else
        {
         goto General_restaet_2; 
        }
  } 
 }
}
               

void Key ()
{
  int leftDoorClosed = digitalRead(LEFT_DOOR_CLOSED_INPUT);
  char customKey = customKeypad.getKey();

  if (leftDoorClosed == LOW && Kassa_1 == 1) {
    if (customKey == 'A') { 
      flag1 = flag1 + 1;
    }  else if (customKey == 'B') { 
      flag1 = flag1 + 1;
    } else if (customKey == 'C') { 
      flag1 = flag1 + 1; 
    } else if (customKey == 'D') {  
      if (flag1 == 3 && flag2 == 1 && flag3 == 1 && flag4 == 1) {
        Enter = 1;  
      } else {
       Enter = 1; 
      } 
     
    } else if (customKey == '1') {  
      flag1=flag1+1;
    } else if (customKey == '2') { 

      flag1=flag1+1;
      if (flag3==1){
       flag4=flag4+1;}

      } else if (customKey == '3') {  

        flag1=flag1+1;
      } else if (customKey == '4') { 

        flag1=flag1+1;
      } else if (customKey == '5') { 

        flag1=flag1+1;
        flag2=flag2+1;
      } else if (customKey == '6') {  

        flag1=flag1+1;
      } else if (customKey == '7') {  

        flag1=flag1+1; 
      } else if (customKey == '8') {  

        flag1=flag1+1;
      } else if (customKey == '9') { 

        flag1=flag1+1;
      } else if (customKey == '0') {  

        flag1=flag1+1;
        if (flag2==1)
        {
          flag3=flag3+1;
        }
      } else if (customKey == '*') { 
        Kassa=1;
        flag1=flag1+1;
      } else if (customKey == '#') {  

        flag1=flag1+1;
      }  
  } else if (customKey == '*') { 
    Kassa=1;
    flag1=flag1+1;
  }
}

//void flashLed2()  
//{ 
  //  led2Status=~led2Status; 
   // digitalWrite(LED2, led2Status);
//}

//void flashLed0()  
//{ 
    //led0Status=~led0Status; 
   // digitalWrite(ALARM_TRIGGER_OUTPUT, led0Status);
//}



