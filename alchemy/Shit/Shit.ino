#define KEY_SENCE        20         // чувствительность кнопки "мс"
#define MAX_TIME_DELAY   7000       // начальное время на поиск нужного сегмента "с"
#define MIN_TIME_DELAY   200       // конечное время на поиск нужного сегмента "с"

#define MGN            2            // Сигнал включения магнита
#define EXT            3            // Внешний сигнал перехода в активный режим
#define SD1            4            // Подсветка 1-й кнопки
#define BUTTON1        5            // 1-я кнопка
#define SD2            6            // Подсветка 2-й кнопки
#define BUTTON2        7            // 2-я кнопка
#define BUTTON4        8            // 4-я кнопка
#define SD4            9            // Подсветка 4-й кнопки
#define BUTTON3        10           // 3-я кнопка
#define SD3            11           // Подсветка 3-й кнопки
#define SD5            14 //A0      // Подсветка 5-й кнопки
#define BUTTON5        15 //A1      // 5-я кнопка
#define SD6            16 //A2      // Подсветка 6-й кнопки
#define BUTTON6        17 //A3      // 6-я кнопка
#define SD7            18 //A4      // Подсветка 7-й кнопки
#define BUTTON7        19 //A5      // 7-я кнопка


#define sd1_on                digitalWrite(SD1, HIGH) //выключение подсветки 1-й кнопки
#define sd1_off               digitalWrite(SD1, LOW)  //включение подсветки 1-й кнопки
#define sd2_on                digitalWrite(SD2, HIGH) //выключение подсветки 2-й кнопки
#define sd2_off               digitalWrite(SD2, LOW)  //включение подсветки 2-й кнопки
#define sd3_on                digitalWrite(SD3, HIGH) //выключение подсветки 3-й кнопки
#define sd3_off               digitalWrite(SD3, LOW)  //включение подсветки 3-й кнопки
#define sd4_on                digitalWrite(SD4, HIGH) //выключение подсветки 4-й кнопки
#define sd4_off               digitalWrite(SD4, LOW)  //включение подсветки 4-й кнопки
#define sd5_on                digitalWrite(SD5, HIGH) //выключение подсветки 5-й кнопки
#define sd5_off               digitalWrite(SD5, LOW)  //включение подсветки 5-й кнопки
#define sd6_on                digitalWrite(SD6, HIGH) //выключение подсветки 6-й кнопки
#define sd6_off               digitalWrite(SD6, LOW)  //включение подсветки 6-й кнопки
#define sd7_on                digitalWrite(SD7, HIGH) //выключение подсветки 7-й кнопки
#define sd7_off               digitalWrite(SD7, LOW)  //включение подсветки 7-й кнопки

#define mgn_on                digitalWrite(MGN, HIGH) //включение магнита
#define mgn_off               digitalWrite(MGN, LOW)  //выключение магнита

#define but1_state            digitalRead(BUTTON1)    //прочитать состояние 1-й кнопки
#define but2_state            digitalRead(BUTTON2)    //прочитать состояние 2-й кнопки
#define but3_state            digitalRead(BUTTON3)    //прочитать состояние 3-й кнопки
#define but4_state            digitalRead(BUTTON4)    //прочитать состояние 4-й кнопки
#define but5_state            digitalRead(BUTTON5)    //прочитать состояние 5-й кнопки
#define but6_state            digitalRead(BUTTON6)    //прочитать состояние 6-й кнопки
#define but7_state            digitalRead(BUTTON7)    //прочитать состояние 7-й кнопки

//Флаги
volatile unsigned char MODE = 0; // 0 - до получения сигнала, 1 - играем, 2 - выиграли.
unsigned char START_KEY_TIMEOUT = 0; //начало таймаута нажатия кнопки
unsigned char SEGMENT_PRESSED = 0; //нужный сегмент был нажат
unsigned char GAME_OVER = 0; //игра проиграна

//Переменные
unsigned long old_time = 0, new_time = 0, timeout = 0; // таймаут
unsigned long key_old_time = 0, key_new_time = 0, key_timeout = 0; //таймаут на клавиши
unsigned char key = 0, old_key = 0, new_key = 0, received_key = 0; //состояние кнопок
unsigned char segment = 0; //выпавший сегмент
unsigned char seg_counter = 0; //счётчик сегментов
unsigned long time_delay = MAX_TIME_DELAY;
 
void setup() 
{ 
  //Инициализируем генератор случайных чисел
  randomSeed(analogRead(6));

  // конфигурируем порты    
  
  pinMode(SD1, OUTPUT); // настроить как выход
  sd1_off;            // и подать логический 0
  pinMode(SD2, OUTPUT); // настроить как выход
  sd2_off;            // и подать логический 0
  pinMode(SD3, OUTPUT); // настроить как выход
  sd3_off;            // и подать логический 0
  pinMode(SD4, OUTPUT); // настроить как выход
  sd4_off;            // и подать логический 0  
  pinMode(SD5, OUTPUT); // настроить как выход
  delay(50);
  sd5_off;            // и подать логический 0
  pinMode(SD6, OUTPUT); // настроить как выход
  sd6_off;            // и подать логический 0
  pinMode(SD7, OUTPUT); // настроить как выход
  sd7_off;            // и подать логический 0
  

  pinMode(BUTTON1, INPUT); // настроить как вход
  pinMode(BUTTON2, INPUT); // настроить как вход
  pinMode(BUTTON3, INPUT); // настроить как вход
  pinMode(BUTTON4, INPUT); // настроить как вход
  pinMode(BUTTON5, INPUT); // настроить как вход
  pinMode(BUTTON6, INPUT); // настроить как вход
  pinMode(BUTTON7, INPUT); // настроить как вход
  
  pinMode(EXT, INPUT); // настроить как вход
  
  pinMode(MGN, OUTPUT); // настроить как выход
  mgn_on;             // включить магнит
  
  Serial.begin(9600); // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)

  Serial.println("Shield Proramm Started");
  delay(50);
  
  timeout = 0;
  old_time = millis();
    
  //for(loop_index = 0; loop_index<1000; loop_index++)
  while(timeout<10000) //запустить тестовый режим в течение 10с
  { 
    new_time = millis();
    if((new_time - old_time)>=0) {
      timeout = new_time - old_time;
    } else {
      timeout = 0xffffffff - old_time + new_time;
    }

	key = (but7_state<<6) + (but6_state<<5) + (but5_state<<4) + (but4_state<<3) + (but3_state<<2) + (but2_state<<1) + but1_state; // собираем состояние кнопок
	
	if(old_key != key) // если значение кнопок изменилось, то начать проверку от дребезга
	{
		old_key = key; 
		key_timeout = 0;
		key_old_time =  millis(); 
		START_KEY_TIMEOUT = 1;
	}
	
	if(START_KEY_TIMEOUT)//если таймаут кнопки запущен
	{
		if(key_timeout<KEY_SENCE )
		{
			if((key_new_time - key_old_time)>=0) key_timeout = key_new_time - key_old_time;
			else key_timeout = 0xffffffff - key_old_time + key_new_time;
		}
		else //если таймаут выдержался
		{
			START_KEY_TIMEOUT = 0;
			new_key = old_key; 
		}
	}
	
	if(received_key != new_key) //зажигать нажатую кнопку(тестовый режим)
	{
		received_key = new_key;
		if(received_key & 0b00000001) sd1_on;
		else sd1_off;
		if(received_key & 0b00000010) sd2_on;
		else sd2_off;
		if(received_key & 0b00000100) sd3_on;
		else sd3_off;
		if(received_key & 0b00001000) sd4_on;
		else sd4_off;
		if(received_key & 0b00010000) sd5_on;
		else sd5_off;
		if(received_key & 0b00100000) sd6_on;
		else sd6_off;
		if(received_key & 0b01000000) sd7_on;
		else sd7_off;
	}
	
  }
  
  sd1_off; sd2_off; sd3_off; sd4_off; sd5_off; sd6_off; sd7_off;
  
  Serial.println("Test finished");
  delay(50);
  
  //LED_BLINKING = 1;
  //led_off;            //выключение подсветки
  
}

void select_segment()
{
  unsigned char new_segment = 0;
  do {
    new_segment = (unsigned char) random(1, 7); //выбираем новый сегмент
  } while (segment == new_segment);
  segment = new_segment;
}

void fire_segment()
{
  switch ( segment ) { //зажигаем нужный сегмент 
      case 1:
        sd1_on; sd2_off; sd3_off; sd4_off; sd5_off; sd6_off; sd7_off;
        break;
      case 2:
        sd1_off; sd2_on; sd3_off; sd4_off; sd5_off; sd6_off; sd7_off;
        break;
    case 3:
        sd1_off; sd2_off; sd3_on; sd4_off; sd5_off; sd6_off; sd7_off;
        break;
    case 4:
        sd1_off; sd2_off; sd3_off; sd4_on; sd5_off; sd6_off; sd7_off;
        break;
    case 5:
        sd1_off; sd2_off; sd3_off; sd4_off; sd5_on; sd6_off; sd7_off;
        break;
    case 6:
        sd1_off; sd2_off; sd3_off; sd4_off; sd5_off; sd6_on; sd7_off;
        break;
    case 7:
        sd1_off; sd2_off; sd3_off; sd4_off; sd5_off; sd6_off; sd7_on;
        break;
    default:
        sd1_off; sd2_off; sd3_off; sd4_off; sd5_off; sd6_off; sd7_off;
  }
}

void mode0()
{
  if (digitalRead(EXT) == LOW) {
    MODE = 1;
    Serial.println("Switched to active mode");  
  }
}

void mode1()
{
  select_segment();
  fire_segment();
  
  while (received_key != (1<<(segment-1))) //ждём пока его нажмут
  {
    key = (but7_state<<6) + (but6_state<<5) + (but5_state<<4) + (but4_state<<3) + (but3_state<<2) + (but2_state<<1) + but1_state; // собираем состояние кнопок
  
    if(old_key != key) // если значение кнопок изменилось, то начать проверку от дребезга
    {
      old_key = key; 
      key_timeout = 0;
      key_old_time =  millis(); 
      START_KEY_TIMEOUT = 1;
    }
  
    if(START_KEY_TIMEOUT)//если таймаут кнопки запущен
    {
      if (key_timeout<KEY_SENCE) 
      {
        if((key_new_time - key_old_time)>=0) key_timeout = key_new_time - key_old_time;
        else key_timeout = 0xffffffff - key_old_time + key_new_time;
      }
      else //если таймаут выдержался
      {
        START_KEY_TIMEOUT = 0;
        new_key = old_key; 
      }
    }
  
    received_key = new_key; 
  }
  
  //Начало игры
  seg_counter = 0;
  GAME_OVER = 0;
  Serial.println("Start game");
  delay(50);
  
  while((seg_counter < 10) && !GAME_OVER) //пока не будут нажаты все сегменты или игра не будет проиграна
  {
    select_segment();
    fire_segment();
    
    SEGMENT_PRESSED = 0;
    
    timeout = 0;
    old_time = millis();
    
    while((timeout<time_delay) && !SEGMENT_PRESSED) //пока не вышел таймаут или не нажат сегмент
    {
      
       new_time = millis();
      if((new_time - old_time)>=0) timeout = new_time - old_time;
      else timeout = 0xffffffff - old_time + new_time;
      
      key = (but7_state<<6) + (but6_state<<5) + (but5_state<<4) + (but4_state<<3) + (but3_state<<2) + (but2_state<<1) + but1_state; // собираем состояние кнопок
  
      if(old_key != key) // если значение кнопок изменилось, то начать проверку от дребезга
      {
        old_key = key; 
        key_timeout = 0;
        key_old_time =  millis(); 
        START_KEY_TIMEOUT = 1;
      }
  
      if(START_KEY_TIMEOUT) //если таймаут кнопки запущен
      {
        if(key_timeout<KEY_SENCE )
        {
          if((key_new_time - key_old_time)>=0) key_timeout = key_new_time - key_old_time;
          else key_timeout = 0xffffffff - key_old_time + key_new_time;
        }
        else //если таймаут выдержался
        {
          START_KEY_TIMEOUT = 0;
          new_key = old_key; 
        }
      }
  
      if(received_key != new_key) 
      {
        received_key = new_key; 
        if(received_key == (1<<(segment-1))) 
        {
          SEGMENT_PRESSED = 1; //нажат нужный сегмент
          time_delay = time_delay-(MAX_TIME_DELAY-MIN_TIME_DELAY)/10;
          seg_counter++;
          
          Serial.print("seg_counter ");
              delay(50);
          Serial.println(seg_counter);
              delay(50);
          
          Serial.print("time_delay ");
              delay(50);
          Serial.println(time_delay);
              delay(50);
          
        }
      }
    }

    if(!SEGMENT_PRESSED) // ели игра проиграна
    {
      GAME_OVER = 1;
      time_delay = MAX_TIME_DELAY;
      
      sd1_on; sd2_on; sd3_on; sd4_on; sd5_on; sd6_on; sd7_on; //финальная анимация
      delay(500);
      sd1_off; sd2_off; sd3_off; sd4_off; sd5_off; sd6_off; sd7_off;
      delay(500);
      sd1_on; sd2_on; sd3_on; sd4_on; sd5_on; sd6_on; sd7_on;
      delay(500);
      sd1_off; sd2_off; sd3_off; sd4_off; sd5_off; sd6_off; sd7_off;
      delay(500);
      sd1_on; sd2_on; sd3_on; sd4_on; sd5_on; sd6_on; sd7_on;
      delay(500);
      sd1_off; sd2_off; sd3_off; sd4_off; sd5_off; sd6_off; sd7_off;
      delay(500);
      Serial.println("Game_over");
      delay(50);
    }
  }
  
  if(seg_counter == 10) { //если игра пройдена
  
    time_delay = MAX_TIME_DELAY;
    sd1_on; sd2_on; sd3_on; sd4_on; sd5_on; sd6_on; sd7_on; //финальная анимация
    delay(500);
    sd1_off; sd2_off; sd3_off; sd4_off; sd5_off; sd6_off; sd7_off;
    delay(500);
    sd1_on; sd2_on; sd3_on; sd4_on; sd5_on; sd6_on; sd7_on;
    delay(500);
    sd4_off;
    delay(700);
    sd3_off; sd5_off;
    delay(700);
    sd2_off; sd6_off; 
    delay(700);
    sd1_off; sd7_off;
    mgn_off;                         //отключение магнитного замка
    Serial.println("You win!!!!");
    delay(50);
    MODE = 2;
  }
}

 
void loop() 
{ 
  switch (MODE) {
    case 0:
      // Wait for activation signal.
      mode0();
      break;

    case 1:
      // Play with shield.
      mode1();
      break;

    case 2:
      // We are in a final state, wait for a reset.
      break;
  }

} 
