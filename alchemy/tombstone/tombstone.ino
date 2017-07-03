#define __STDC_LIMIT_MACROS

#include <MFRC522.h>
#include <SPI.h>
#include <EEPROM.h>
#include <Servo.h>
#include <DFPlayer_Mini_Mp3.h>

#define DEBUG 0

#define RST_PIN        8           // Configurable, see typical pin layout above
#define SS_PIN         10          // Configurable, see typical pin layout above

#define SERVO_S1       5            // Сигнал управления сервоприводом №1
#define SERVO_S2       6            // Сигнал управления сервоприводом №2

#define EXT_OUT        4            // Внешний сигнал активации второй платы

#define SD_R           16 //A2      // Красный цвет второго светодиода
#define SD_G           17 //A3      // Зелёный цвет первого светодиода

#define PRG            14 //A0      // Вывод отвечает за вход в режим программирования новой карточки (если на него подан логический 0 - GND)

#define GARGOIL_POSITION 9 // Герконы от гаргулий
#define INPUT_SKULL 3 // Входной сигнал от черепа

#define GARGOIL_EYES 7 // Глаза гаргулий 
#define GARGOIL_LOCK A1 // Отпускаем гаргулий
#define TOMB_LOCK A4 // Отпускаем "камень" от могилы
#define CAPTION A5 // Включаем подпись

#define sd_access_green       ( digitalWrite(SD_G, HIGH), digitalWrite(SD_R, LOW) )
#define sd_access_red         ( digitalWrite(SD_G, LOW), digitalWrite(SD_R, HIGH) )
#define sd_access_yellow      ( digitalWrite(SD_G, HIGH), digitalWrite(SD_R, HIGH) )
#define sd_access_clear       ( digitalWrite(SD_G, LOW), digitalWrite(SD_R, LOW) )

#define ext_out_on            ( digitalWrite(EXT_OUT, LOW) )
#define ext_out_off           ( digitalWrite(EXT_OUT, HIGH) )


MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;

Servo myservo1;  // create servo object to control a servo
// twelve servo objects can be created on most boards
Servo myservo2;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int angle = 0;    // variable to store the servo position

//const unsigned char Saved_UID[10] = {0x34, 0x38, 0x07, 0x81, 0xda, 0xed, 0x56, 0, 0, 0};
unsigned char legal_UID[10];
unsigned char card_i = 0, i = 0;

unsigned long old_time = 0, new_time = 0, timeout = 0;

////////////////////////////////////////////////////////////////////////////////////////

enum EState
{
  ES_WAIT_SKULL,
  ES_WAIT_GARGOIL,
  ES_WAIT_CRYSTALL,
  ES_ETERNITY
};

EState State = ES_WAIT_SKULL;

////////////////////////////////////////////////////////////////////////////////////////

#if (DEBUG)
/**
   Helper routine to dump a byte array as hex values to Serial.
*/
void dump_byte_array(byte *buffer, byte bufferSize)
{
  for (byte i = 0; i < bufferSize; i++)
  {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    // delay(50);
    Serial.print(buffer[i], HEX);
    // delay(50);
  }
}
#endif

void(* doReset) (void) = 0; //declare reset function @ address 0

void RecordNewTagAndReset()
{
  sd_access_yellow;

  timeout = 0;
  old_time = millis();

  while (timeout < 1000) {
    new_time = millis();
    if ((new_time - old_time) >= 0) {
      timeout = new_time - old_time;
    } else {
      timeout = 0xffffffff - old_time + new_time;
    }

    // Look for new cards
    if (!mfrc522.PICC_IsNewCardPresent()) {
      continue;
    }
#if(DEBUG)
    Serial.println("IsNewCard");
    //delay(50);
#endif

    // Select one of the cards
    if ( mfrc522.PICC_ReadCardSerial()) {
#if (DEBUG)
      // Show some details of the PICC (that is: the tag/card)
      Serial.print("Card UID:");
      dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);

      Serial.println();
      Serial.print("PICC type: ");
      byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
      delay(50);
#endif

      unsigned char PROGRAMM_OK = 1;
      for (card_i = 0; card_i < 10; card_i++) {
        if (card_i < mfrc522.uid.size) {
          EEPROM.write(32245 + card_i, mfrc522.uid.uidByte[card_i]);
          legal_UID[card_i] = mfrc522.uid.uidByte[card_i];
          delay(5);
        } else {
          EEPROM.write(32245 + card_i, 0);
          legal_UID[card_i] = 0;
          delay(5);
        }
      }

      for (card_i = 0; card_i < mfrc522.uid.size; card_i++) {
        if (mfrc522.uid.uidByte[card_i] != EEPROM.read(32245 + card_i)) {
          PROGRAMM_OK = 0;
        }
      }

#if (DEBUG)
      if (PROGRAMM_OK) {
        Serial.println("Programm OK");
      } else {
        Serial.println("Programm mistake");
      }
      //delay(50);
#endif

      if (PROGRAMM_OK) {
        sd_access_green;
        delay(2000);
        sd_access_yellow;
        delay(2000);
        sd_access_red;
        delay(6000);
      } else {
        sd_access_red;
        delay(10000);
      }

      // Stop encryption on PCD
      mfrc522.PCD_StopCrypto1();
      sd_access_yellow;

      doReset();
    }
  }
}

void DoWaitSkull()
{
  if (digitalRead(INPUT_SKULL) == LOW) {
    digitalWrite(GARGOIL_LOCK, HIGH);
    digitalWrite(CAPTION, LOW);

    digitalWrite(GARGOIL_EYES, HIGH);
    
    State = ES_WAIT_GARGOIL;

    sd_access_green;
    delay(500);
    sd_access_yellow;
  }
}

void DoWaitGargoil()
{
  if (digitalRead(GARGOIL_POSITION) == LOW) {
  #if (!DEBUG)
    mp3_play(1);
  #endif

    // Blink eyes.

    for (int i = 0; i < 3; ++i) {
      delay(500);
      digitalWrite(GARGOIL_EYES, LOW);
      delay(500);
      digitalWrite(GARGOIL_EYES, HIGH); 
    }

    // Открыть магнит.
    digitalWrite(TOMB_LOCK, HIGH);

    State = ES_WAIT_CRYSTALL;

    sd_access_green;
    delay(500);
    sd_access_yellow;

  /*
  #if (!DEBUG)
    delay(3000);
    mp3_stop();
  #endif  */
    
  }
}

void DoWaitCrystall()
{
  sd_access_yellow;

  timeout = 0;
  old_time = millis();

  while (timeout < 1000) //продолжать опрос в течение 1с
  {
    new_time = millis();
    if ((new_time - old_time) >= 0) timeout = new_time - old_time;
    else timeout = 0xffffffff - old_time + new_time;

    // Look for new cards
    if ( mfrc522.PICC_IsNewCardPresent())
    {
#if (DEBUG)
      Serial.println("IsNewCard");
      //delay(50);
#endif
      // Select one of the cards
      if ( mfrc522.PICC_ReadCardSerial())
      {


#if (DEBUG)
        // Show some details of the PICC (that is: the tag/card)
        Serial.print("Card UID:");
        //delay(50);
        dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);

        Serial.println();
        Serial.print("Legal UID:");
        dump_byte_array(legal_UID, sizeof(legal_UID));

        //delay(50);
        Serial.println();
        //delay(50);
        Serial.print("PICC type: ");
        //delay(50);
        byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
        //Serial.println(mfrc522.PICC_GetTypeName(piccType));
        delay(50);
#endif

        unsigned char SUCCESS = 1;
        for (card_i = 0; card_i < mfrc522.uid.size; card_i++)
        {
          if (mfrc522.uid.uidByte[card_i] != legal_UID[card_i])
          {
            SUCCESS = 0;
            break;
          }
        }

        if (SUCCESS) { //Если карточка принята
#if (DEBUG)
          Serial.println();
          Serial.println("Card success");
#endif
          sd_access_green;
          ext_out_on;                   // подать сигнал активности плате второго кристала
          
          myservo1.attach(SERVO_S1);  // подключить сервопривод
          myservo1.write(91);         // скорость вращения сервоприводов >90 вправо <90 влево
          myservo2.attach(SERVO_S2);  // подключить сервопривод
          myservo2.write(89);         // скорость вращения сервоприводов >90 вправо <90 влево
          delay(30000);
          myservo1.detach();          //отключить сервопривод
          myservo2.detach();
          sd_access_yellow;

          State = ES_ETERNITY;
        } else { //если карточка не верна
#if (DEBUG)
          Serial.println();
          Serial.println("Card denied");
#endif
          sd_access_red;
          delay(2000);
          sd_access_yellow;
        }

        // Stop encryption on PCD
        mfrc522.PCD_StopCrypto1();
        break;
      }
    }
  }
}

/**
   Initialize.
*/

void setup()
{
  // конфигурируем порты
  ext_out_off;
  pinMode(EXT_OUT, OUTPUT);

  pinMode(SD_R, OUTPUT);
  pinMode(SD_G, OUTPUT);
  sd_access_yellow;

  pinMode(PRG, INPUT); // настроить как вход
  digitalWrite(PRG, HIGH); // притянуть к питанию

  pinMode(GARGOIL_EYES, OUTPUT);
  pinMode(GARGOIL_LOCK, OUTPUT);
  pinMode(TOMB_LOCK, OUTPUT);
  pinMode(CAPTION, OUTPUT);

  digitalWrite(GARGOIL_EYES, LOW);
  digitalWrite(GARGOIL_LOCK, LOW);
  digitalWrite(TOMB_LOCK, LOW);
  digitalWrite(CAPTION, HIGH);

  pinMode(GARGOIL_POSITION, INPUT_PULLUP);
  pinMode(INPUT_SKULL, INPUT_PULLUP);

  Serial.begin(9600); // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)

#if (!DEBUG)
  mp3_set_serial (Serial);    //set Serial for DFPlayer-mini mp3 module 
  delay (100);
  mp3_set_volume(30);

  delay(500);
  mp3_play(1);
  delay(2000);
  mp3_stop();
#endif

  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card

  // Prepare the key (used both as key A and as key B)
  // using FFFFFFFFFFFFh which is the default at chip delivery from the factory

  for (byte i = 0; i < 10; i++)
  {
    legal_UID[i] = EEPROM.read(32245 + i);
  }

#if (DEBUG)
  Serial.println("Scan a MIFARE Ultralight Started");
  //delay(50);
  dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);
  //delay(50);
  Serial.println();
  //delay(50);
#endif
}

/**
   Main loop.
*/
void loop() {
  if (digitalRead(PRG) == LOW) {
    RecordNewTagAndReset();
  }

  switch (State) {
    case ES_WAIT_SKULL:
      DoWaitSkull();
      break;

    case ES_WAIT_GARGOIL:
      DoWaitGargoil();
      break;

    case ES_WAIT_CRYSTALL:
      DoWaitCrystall();
      break;

    case ES_ETERNITY:
      while (true) {
        delay(100);
      }
  };
}

