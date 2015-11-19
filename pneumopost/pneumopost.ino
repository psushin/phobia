

#define LED1 8  
//#define LED2 9 
#define LED0 9 
#define Vx_1 A3  // Датчик закрытия двери 1
#define Vx_2 A4  // Подготовка к работе
#define Vx_3 A5  // Датчик установленной бутылки в двери
#define Vx_4 13  // Сигнал с кассового аппарата
#define Vx_7      // Поворот стола


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
  {'1','2','3','A'},  
  {'4','5','6','B'},  
  {'7','8','9','C'},  
  {'*','0','#','D'} 
}; 

byte rowPins[ROWS] = {0,1,2,3}; //connect to the row pinouts of the keypad  
byte colPins[COLS] = {4,5,6,7}; //connect to the column pinouts of the keypad 

//initialize an instance of class NewKeypad  
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
int Kassa=0;
int Kassa_1=0;
int Enter;      // Нажатие кнопки Enter
int Vx_2_st=0;  // Подготовка к работе
int Vx_1_st=0;  // Состояние датчика двери 1 
int Vx_3_st=0;  // Состояние датчика установленной бутылки в двери
int Vx_4_st=0;  // Сигнал с кассового аппарата
int Vx_4_st1=0;
int Vx_7_st=0;

byte led1Status = 0;
byte led2Status = 0;
byte led0Status = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED0,OUTPUT);  
  pinMode(8,OUTPUT);
  pinMode(10,OUTPUT);
  leftDoorServo.attach(12);
  rightDoorServo.attach(11);
  leftDoorServo.write(0);
  rightDoorServo.write(0);

  pinMode(A0,OUTPUT); // выход  электромагнит
  pinMode(A1,OUTPUT); // выход светодиод двери 1
  pinMode(A2,OUTPUT); // выход светодиод двери 2
  pinMode(Vx_1,INPUT);
  pinMode(Vx_2,INPUT);
  pinMode(Vx_3,INPUT);
  pinMode(Vx_4,INPUT);
  digitalWrite(A0,LOW);
  digitalWrite(A1,LOW);
  digitalWrite(A2,LOW);
  digitalWrite(LED0,LOW);
  digitalWrite(10,HIGH);
  myOS.begin();     
  myOS.addTask(Key, 1);
  myOS.addTask(flashLed1, 100);
  //myOS.addTask(flashLed2, 200);
  //myOS.addTask(flashLed0, 150);
}

void loop() {
  // put your main code here, to run repeatedl

 Vx_2_st = digitalRead(Vx_2); 
 //delay(100);
  if (Kassa == 1) 
   {
    delay(1000);
    rightDoorServo.write(140);
    leftDoorServo.write(140);
    digitalWrite(A2,LOW); 
    digitalWrite(A1,LOW); 
    flag1=0;
           Vx_1_st = digitalRead(Vx_1); 
           //delay(100);
           if (Vx_1_st == LOW)
           {
            //Kassa =0; 
            delay (5000);
           digitalWrite(10,LOW);
            delay (2500);
            leftDoorServo.write(0); 
            digitalWrite(A1,HIGH);
            // Kassa_1=1;
            General_restaet_2:
            Vx_4_st = digitalRead(Vx_4);
             if (Vx_4_st == HIGH)
               {
                Kassa_1 = 1; 
                
                 
                // Kassa=0;
              // goto  General_restaet_2:
       General_restaet_1:
       Vx_2_st = digitalRead(Vx_2); 
       //delay(100);
         // if (Vx_2_st == HIGH) 
             {
               if (Enter==1)
                {
                 if (flag1==3 && flag2==1 && flag3==1 && flag4==1&& Kassa_1 == 1)
                    {
                      Vx_3_st = digitalRead(Vx_3);
                          // delay(100);
                        if (Vx_3_st == LOW)
                         {
                            leftDoorServo.write(140); 
                            digitalWrite(A1,LOW); 
                            delay(3000);
                            digitalWrite(A0,HIGH); 
                            delay(2000);
                            rightDoorServo.write(0);
                            digitalWrite(A2,HIGH); 
                            flag1=0;
                            flag2=0;
                            flag3=0; 
                            flag4=0;
                            digitalWrite(LED0,HIGH);
                            delay(3000);
                            digitalWrite(LED0,LOW);
                            //goto General_restart;
                 
                            while (Vx_4_st1<200)
                            {
                              
                            }
                            
                            
                         }
                         else
                         {
                           leftDoorServo.write(140); 
                           digitalWrite(A1,LOW); 
                           delay(5000); 
                           leftDoorServo.write(0);
                           digitalWrite(A1,HIGH);
                           Enter=0;
                           flag1=0;
                           flag2=0;
                           flag3=0; 
                           flag4=0;
                           goto General_restaet_1;
                         }
                  
                         
                    }

                else  
                    {
                      leftDoorServo.write(140); 
                      digitalWrite(A1,LOW); 
                      delay(5000); 
                      leftDoorServo.write(0);
                      digitalWrite(A1,HIGH);
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

/*
delay(500);
leftDoorServo.write(120);
rightDoorServo.write(120);
delay(500);
leftDoorServo.write(0);
rightDoorServo.write(0);
*/       

}
               

void Key ()
{
   Vx_2_st = digitalRead(Vx_2);
  // delay(100);
 char customKey = customKeypad.getKey();
 
if (Vx_2_st == LOW && Kassa_1 == 1)
  {
   if (customKey == 'A') { 
   flag1=flag1+1;
}  else if (customKey == 'B') { 
  
  flag1=flag1+1;
} else if (customKey == 'C') { 
  
  flag1=flag1+1; 
} else if (customKey == 'D') {  
 
      if (flag1==3 && flag2==1 && flag3==1 && flag4==1) 
      {
      Enter=1;  
      }
       else 
       {
       Enter=1; 
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
  }
  else if (customKey == '*') { 
  Kassa=1;
  flag1=flag1+1;
  }
}

void flashLed1()  
{ 
    led1Status=~led1Status; 
    digitalWrite(LED1, led1Status); 
} 

//void flashLed2()  
//{ 
  //  led2Status=~led2Status; 
   // digitalWrite(LED2, led2Status);
//}

//void flashLed0()  
//{ 
    //led0Status=~led0Status; 
   // digitalWrite(LED0, led0Status);
//}



