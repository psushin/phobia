// Windows-only!

#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>
#include <Servo.h> 
#include <IRremote.h>

#undef DEBUG

#define RST_PIN        8           // Configurable, see typical pin layout above
#define SS_PIN         10          // Configurable, see typical pin layout above

#define EXT_IN1        A1           // Внешний сигнал активации 1 (от первой платы кристалла)
#define EXT_IN2        5            // Внешний сигнал активации 2
#define EXT_IN3        6            // Внешний сигнал активации 3

#define FINAL_RELAY    9
  
#define DOOR           7            // Открылась дверь в четвертую комнату, включаем все.
#define FAILURE        4            // Окончание квеста по кнопке, невыход.

#define SD_R           16 //A2      // Красный цвет второго светодиода
#define SD_G           17 //A3      // Зелёный цвет первого светодиода
  
#define PRG            A0           // Вывод отвечает за вход в режим программирования новой карточки (если на него подан логический 0)

#define TRIG           A4
#define ECHO           A5

#define sd_access_green       ( digitalWrite(SD_G, HIGH), digitalWrite(SD_R, LOW) )
#define sd_access_red         ( digitalWrite(SD_G, LOW), digitalWrite(SD_R, HIGH) )
#define sd_access_yellow      ( digitalWrite(SD_G, HIGH), digitalWrite(SD_R, HIGH) )
#define sd_access_clear       ( digitalWrite(SD_G, LOW), digitalWrite(SD_R, LOW) )


MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;

unsigned char legal_UID[10];
unsigned char card_i = 0;

enum EState {
  ES_INIT,
  ES_PREPARED,
  ES_OPENED,
  ES_ACTIVE,
  ES_FINISHED
};

EState State;

//////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() 
{
  // конфигурируем порты    
  pinMode(EXT_IN1, INPUT); // настроить как вход  
  pinMode(EXT_IN2, INPUT); // настроить как вход
  pinMode(EXT_IN3, INPUT); // настроить как вход
  pinMode(FINAL_RELAY, OUTPUT); // настроить как вход

  digitalWrite(FINAL_RELAY, HIGH);
  
  pinMode(SD_R, OUTPUT);
  pinMode(SD_G, OUTPUT);

  pinMode(TRIG, OUTPUT); // Trig pin for distance meter
  pinMode(ECHO, INPUT);  // Echo pin for distance meter

  pinMode(DOOR, INPUT_PULLUP);
  pinMode(FAILURE, INPUT_PULLUP);
  
  sd_access_clear;
  
  pinMode(PRG, INPUT); // настроить как вход
  digitalWrite(PRG, HIGH); // притянуть к питанию

  Serial.begin(9600); // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
	  
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card

  // Prepare the key (used both as key A and as key B)
  // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
   
  for (byte i = 0; i < 10; i++) {
     legal_UID[i] = EEPROM.read(32245+i);
  }
  
#ifdef DEBUG
  Serial.println("Scan a MIFARE Ultralight Started");
  dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE); 
  Serial.println();   
#endif

  Serial.println("RESET");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(DEBUG)
/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) 
{
  for (byte i = 0; i < bufferSize; i++) 
  {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////

void save_new_tag()
{
  sd_access_yellow;
    
  unsigned long timeout = 0;
  unsigned long old_time = millis();

  bool PROGRAMM_OK = true;
    
  while (timeout < 1000) { //продолжать опрос в течение 1с
    unsigned long new_time = millis();
    if ((new_time - old_time)>=0) 
      timeout = new_time - old_time;
    else 
      timeout = 0xffffffff - old_time + new_time;
            
    // Look for new cards
    if ( mfrc522.PICC_IsNewCardPresent()) {
#if defined(DEBUG)
      Serial.println("IsNewCard");
#endif
      // Select one of the cards
      if ( mfrc522.PICC_ReadCardSerial()) {
#if defined(DEBUG)
        // Show some details of the PICC (that is: the tag/card)
        Serial.print("Card UID:");
        dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
         
        Serial.println();
        Serial.print("PICC type: ");
      
        byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
        Serial.println(mfrc522.PICC_GetTypeName(piccType));
        delay(50);
#endif   
        for(card_i = 0; card_i < 10; card_i++) {
          if(card_i < mfrc522.uid.size) {
            EEPROM.write(32245 + card_i, mfrc522.uid.uidByte[card_i]);  
            legal_UID[card_i] = mfrc522.uid.uidByte[card_i];
            delay(5);
          } else {
            EEPROM.write(32245 + card_i, 0);
            legal_UID[card_i] = 0;
            delay(5);
          }
        }
    
        for(card_i = 0; card_i < mfrc522.uid.size; card_i++) {
          if(mfrc522.uid.uidByte[card_i] != EEPROM.read(32245+card_i)) 
            PROGRAMM_OK = false;
        }
      
#if defined(DEBUG)
        if(PROGRAMM_OK) 
          Serial.println("Programm OK");
        else 
          Serial.println("Programm mistake");
#endif
        
        if(PROGRAMM_OK) {
          sd_access_green;
          delay(2000);
          sd_access_yellow;
          sd_access_red;
          delay(6000);      
        } else {
          sd_access_red;
          delay(10000);
        }
    
        // Stop encryption on PCD
        mfrc522.PCD_StopCrypto1();
      }
    }
  }  
}

void handle_init()
{
  if (digitalRead(EXT_IN2) == LOW) {
    // Сигнал с алхимической машины.
    
    sd_access_green;
    delay(1000);
    sd_access_clear;

    {
      IRsend irsend;
      irsend.sendNEC(0x8C73817E, 32); 
    }

    delay(5000);
    
    Serial.println("PREPARE");
    State = ES_PREPARED;
    
  }
}

unsigned long was_opened_timestamp = 0;

// Waiting for a door to open.
void handle_prepared()
{
  if (digitalRead(DOOR) == HIGH) {

    sd_access_green;
    delay(500);
    sd_access_clear;

    // Wait 10 sec before calling closer.
    delay(10000);
    
    Serial.println("CALL_CLOSER");
    State = ES_OPENED;
    was_opened_timestamp = millis();
  }
}

void handle_opened()
{
  if (millis() - was_opened_timestamp < 10 * 1000) {
    // Даже если датчик глючит, нужно время чтобы войти и осмотреться.
    return;
  }

  if (millis() - was_opened_timestamp > 60LL * 1000) {
    // Если уже очень долго, не ждем, пока они подойдут.
    Serial.println("GREETING");
    State = ES_ACTIVE;
    return;
  }
  
  const int PRESENCE_DISTANCE = 80;
  
  //for (int i = 0; i < 5; ++i) {  
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
  
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);
  
    long duration = pulseIn(ECHO, HIGH);
    // Calculating the distance
    int distance = duration * 0.034 / 2;
      
    #ifdef DEBUG
        Serial.print("Distance: ");
        Serial.println(distance);
    #endif

    // May be this will reduce noise.
    delay(500);

    if (distance >= PRESENCE_DISTANCE) {
      // if distance is longer - fail
      return;
    }
  //}

  sd_access_green;
  delay(200);
  sd_access_yellow;
  delay(200);
  sd_access_clear;


  // We measured 5 times in a row less than distance.
  Serial.println("GREETING");
  State = ES_ACTIVE;
}

unsigned long was_stupid_timestamp = 0;
const unsigned long STUPID_TIMEOUT = 5UL * 60 *  1000;

void handle_active()
{
  if (digitalRead(FAILURE) == LOW) {
    sd_access_red;
    delay(300);
    sd_access_clear;
    delay(100);
    sd_access_red;
    delay(300);
    sd_access_clear;
    
    Serial.println("FAILURE");
    digitalWrite(FINAL_RELAY, LOW);
    State = ES_FINISHED;
    return;
  }
  
  unsigned long timeout = 0;
  unsigned long old_time = millis();
  
  while (timeout < 1000) { //продолжать опрос в течение 1с
    unsigned long new_time = millis();
    if (new_time - old_time >= 0) 
      timeout = new_time - old_time;
    else 
      timeout = 0xffffffff - old_time + new_time;
          
   // Look for new cards
   if (mfrc522.PICC_IsNewCardPresent()) {
   
#if defined(DEBUG)
     Serial.println("IsNewCard");
#endif
     // Select one of the cards
     if ( mfrc522.PICC_ReadCardSerial()) {

#if defined(DEBUG)
       // Show some details of the PICC (that is: the tag/card)
       Serial.print("Card UID:");
       dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
  
       Serial.println();
       Serial.print("Legal UID:");     
       dump_byte_array(legal_UID, sizeof(legal_UID));
       
       Serial.println();
       Serial.print("PICC type: ");
       byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
       Serial.println(mfrc522.PICC_GetTypeName(piccType));
       delay(50);
#endif
 
       for (card_i = 0; card_i <mfrc522.uid.size; card_i++) {
         if (mfrc522.uid.uidByte[card_i] != legal_UID[card_i]) {
           // Неправильная карточка - не реагируем никак!
#ifdef DEBUG 
           Serial.println();
           Serial.println("Card denied");
#endif
           sd_access_red;
           delay(2000);
           sd_access_clear;
           return;
         }
       }
      
     //Если карточка принята
#if defined(DEBUG)
       Serial.println();
       Serial.println("Card success");
#endif
       if (digitalRead(EXT_IN1) == LOW) { 
         // Амулет закален.
         sd_access_green;      
         Serial.println("SUCCESS");  
         State = ES_FINISHED;
         digitalWrite(FINAL_RELAY, LOW);
       } else if (was_stupid_timestamp == 0 || new_time - was_stupid_timestamp > STUPID_TIMEOUT) {
         // Не посылать STUPID чаще чем раз в 5 минут.
         Serial.println("STUPID");
         was_stupid_timestamp = new_time;
       }
        
       return;
      }
    }
  }
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Main loop.
 */
void loop() {

  if (digitalRead(PRG) == LOW) {
    save_new_tag();
    return;
  }

  switch (State) {
    case ES_INIT:
      handle_init();
      break;

    case ES_PREPARED:
      handle_prepared();
      break;

    case ES_OPENED:
      handle_opened();
      break;

    case ES_ACTIVE:
      handle_active();
      break;

    case ES_FINISHED:
      break;
  };
}


