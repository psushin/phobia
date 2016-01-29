#include "images.h"
#include "display.h"

/////////////////////////////////////////////////////////////

bool image[WIDTH][HEIGHT];

TFlipDotDisplay Display(3);

/////////////////////////////////////////////////////////////

volatile bool Interrupt = false;

void InterruptBackground()
{
  Interrupt = true;
  Display.FireInterrupt();
}

void BackgroundShow()
{ 
  // Interruptable background display show.
   
  Interrupt = false;
  Display.ResetInterrupt();

  int i = 0;
  while (true) {
    Display.Shuffle(2000);
    if (Interrupt) {
      return;
    }
    createHotel(image);
    Display.ChaoticMorph(image);

    if (!DelayImpl(8000, &Interrupt)) {
      return;
    }

    Display.Shuffle(2000);
    if (Interrupt) {
      return;
    }
    createNoir(image);
    Display.ChaoticMorph(image);
    if (!DelayImpl(8000, &Interrupt)) {
      return;
    }
    
    Display.Shuffle(2000);
    if (Interrupt) {
      return;
    }
    createSmallPhobia(image);
    Display.ChaoticMorph(image);
    
    if (!DelayImpl(8000, &Interrupt)) {
      return;
    }
  }
}

/////////////////////////////////////////////////////////////

void setup() 
{ 
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(2), InterruptBackground, RISING);
  
  Display.CleanImage();
  delay(5000); 

  createSpecial(image);

  for (int i = 0; i < HEIGHT; ++i) {
    for (int j = 0; j < Letter_K.Width; ++j) {
      Serial.print(pgm_read_byte(&(Letter_KData[j][i])));
    }
    Serial.println();
  }

  for (int i = 0; i < HEIGHT; ++i) {
    for (int j = 0; j < Letter_K.Width; ++j) {
      Serial.print(Letter_KData1[j][i]);
    }
    Serial.println();
  }
  
  
  Display.TopShutter(image);



}

void loop() 
{

/*
  createSpecial(image);
  Display.ChaoticMorph(image);

  delay(5000);
 

  BackgroundShow();


  createDigits(image, "0.42", 4);
  Display.RightMorph(image);
  delay(3000);

  createDigits(image, "4.26", 4);
  Display.RightMorph(image);
  delay(3000);

  createDigits(image, "42.60", 5);
  Display.RightMorph(image);
  delay(3000);

  createError(image);
  Display.TopShutter(image);
  delay(5000);
  createDigits(image, "0.00", 4);
  Display.TopShutter(image);
  delay(3000);
*/

/*
  createSmallPhobia(image);
  Display.Shuffle(2000);
  Display.ChaoticMorph(image);
  
  delay(5000);

  //cleanImage(image);
  //Display.TopShutter(image, 200);
  createHotel(image);
  Display.OuterArrows(image, 200);

  delay(5000);

  //cleanImage(image);
  //Display.MovingArrows(image);
  createNoir(image);
  Display.InnerArrows(image, 200);

  delay(5000);

  cleanImage(image);
  Display.RightShutter(image);
  createLargePhobia(image);
  Display.LeftShutter(image);

  delay(10000);
  
  createSpecial(image);
  Display.ChaoticMorph(image);

  delay(5000);
  */
}
