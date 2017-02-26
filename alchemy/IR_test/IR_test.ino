#define DEBUG
#include <IRremote.h>
//IR PIN mega -9
//IR PIN 328 -3


#define VERSION  1  


IRsend irsend;


#define SERIAL_BAUD  57600

#ifdef DEBUG  
int serial_last_count;
  #define sp(st) Serial.println(st); delay(10); 
#else
  #define sp(st) 
#endif

void setup()
{

#ifdef DEBUG
  Serial.begin(SERIAL_BAUD);
  delay(100);
//temp_time = millis();
#endif


irsend.sendNEC(0x8C73817E, 32); 
  digitalWrite(13,HIGH);
 delay(200); 
  digitalWrite(13,LOW);

}
void loop()
{

 

}


