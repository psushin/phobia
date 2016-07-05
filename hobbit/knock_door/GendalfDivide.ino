/*******************************************************************************
 * DFPlayer_Mini_Mp3, This library provides a quite complete function for      * 
 * DFPlayer mini mp3 module.                                                   *
 * www.github.com/dfrobot/DFPlayer_Mini_Mp3 (github as default source provider)*
 *  DFRobot-A great source for opensource hardware and robot.                  *
 *                                                                             *
 * This file is part of the DFplayer_Mini_Mp3 library.                         *
 *                                                                             *
 * DFPlayer_Mini_Mp3 is free software: you can redistribute it and/or          *
 * modify it under the terms of the GNU Lesser General Public License as       *
 * published by the Free Software Foundation, either version 3 of              *
 * the License, or any later version.                                          *
 *                                                                             *
 * DFPlayer_Mini_Mp3 is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 * GNU Lesser General Public License for more details.                         *
 *                                                                             *
 * DFPlayer_Mini_Mp3 is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 * GNU Lesser General Public License for more details.                         *
 *                                                                             *
 * You should have received a copy of the GNU Lesser General Public            *
 * License along with DFPlayer_Mini_Mp3. If not, see                           *
 * <http://www.gnu.org/licenses/>.                                             *
 *									       *
 ******************************************************************************/

/*
 *	Copyright:	DFRobot
 *	name:		DFPlayer_Mini_Mp3 sample code
 *	Author:		lisper <lisper.li@dfrobot.com>
 *	Date:		2014-05-30
 *	Description:	connect DFPlayer Mini by SoftwareSerial, this code is test on Uno
 *			Note: the mp3 files must put into mp3 folder in your tf card 
 */
#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>


unsigned int knockCount=0;
long now=0,knock=0;
long now1=0;
int Summ;
int a;
bool already=1;
long now2=-150000;
float mistake=0.6;
//
void setup () {
	Serial.begin (9600);
	mp3_set_serial (Serial);	//set softwareSerial for DFPlayer-mini mp3 module 
	mp3_set_volume (20);
pinMode(13,OUTPUT); //просто так - лампочка
pinMode(12,OUTPUT); //реле
digitalWrite(12,HIGH);
}


//
void loop () {        
	
   a=analogRead(A2);
    if (a>5) {now1=millis();
    already=0;
   
      if ((millis()-now)>1500) { knockCount=0;   Summ = 0; }
      
      else {
        
        
        
        if (Summ==3)              if ((knock+knock*mistake)/3>millis()-now && (knock-knock*mistake)/3<millis()-now)    {Summ++; }
        if (Summ==2)              if ((knock+knock*mistake)/3>millis()-now && (knock-knock*mistake)/3<millis()-now)    {Summ++; }
        if (Summ==1)              if     (knock+knock*mistake>millis()-now && knock-knock*mistake<millis()-now)        {Summ++; }
        if (Summ==0)              if     (knock+knock*mistake>millis()-now && knock-knock*mistake<millis()-now)        {Summ++; }
        if (knockCount==1)                                      knock=millis()-now;
}  
Serial.print(knock+knock*mistake); 
Serial.print(">");
Serial.print(millis()-now);
Serial.print(">");
Serial.print(knock-knock*mistake);
Serial.print("->");
Serial.println(Summ);
      knockCount++;
     
      digitalWrite(13,HIGH);
      delay(200);
      digitalWrite(13,LOW);
      if (Summ==4)  {digitalWrite(13,HIGH); digitalWrite(12,LOW); 
                     delay(1500);  
                     now2=millis(); //запомнили когда открылась дверь для того чтобы заткнуть гендальфа
                     digitalWrite(13,LOW); digitalWrite(12,HIGH);
                      }  
       now=millis();
    }
   if ((millis()-now1)>1500) if (Summ!=4) if (!already) if ((millis()-now2)>150000)
	{mp3_play(1); delay(5000); // гендальф не пускает
         already=1; }
 if  (knockCount>4) if (!already) if ((millis()-now2)>150000) { knockCount=0;   Summ = 0; 
 mp3_play(1); delay(5000); // гендальф не пускает
         already=1;}
         if  (knockCount>4) { knockCount=0;   Summ = 0; delay(5000);}
         
}

