#include "images.h"
#include "display.h"

void createSmallPhobia(bool image[WIDTH][HEIGHT])
{
  using namespace Font3x7;
  TImage images[] = {Letter_p, Letter_h, Letter_o, Letter_b, Letter_i, Letter_a};
  createImage(image, images, 6, 1);
}

void createLargePhobia(bool image[WIDTH][HEIGHT]) 
{
  using namespace Font4x7;
  TImage images[] = {Letter_P, Letter_H, Letter_O, Letter_B, Letter_I, Letter_A};
  createImage(image, images, 6, 1);
}

void createHotel(bool image[WIDTH][HEIGHT]) 
{
  using namespace Font4x7;
  TImage images[] = {Letter_H, Letter_O, Letter_T, Letter_E, Letter_L};
  createImage(image, images, 5, 1);
}

void createNoir(bool image[WIDTH][HEIGHT]) 
{
  using namespace Font4x7;
  TImage images[] = {Letter_N, Letter_O, Letter_I, Letter_R};
  createImage(image, images, 4, 1);
}

void createSpecial(bool image[WIDTH][HEIGHT])
{
  TImage images[] = {Letter_K, PlusSign, Letter_PRus, EqualSign, Heart};
  createImage(image, images, 5, 1);
}

TFlipDotDisplay Display(8);

void setup() 
{ 
  Display.CleanImage();
  delay(5000); 
}

void loop() 
{
  bool image[WIDTH][HEIGHT];

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
  
  /*
  createSpecial(image);
  Display.ChaoticMorph(image);

  delay(5000);
  */
}
