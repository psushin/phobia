#pragma once

#include "image_parts.h"
#include "display.h"

/////////////////////////////////////////////////////////////

void createSmallPhobia(bool image[WIDTH][HEIGHT])
{
  using namespace Font3x7;
  TImage images[] = {Letter_p, Letter_h, Letter_o, Letter_b, Letter_i, Letter_a};
  createImageMiddle(image, images, 6, 1);
}

/*
void createLargePhobia(bool image[WIDTH][HEIGHT]) 
{
  using namespace Font4x7;
  TImage images[] = {Letter_P, Letter_H, Letter_O, Letter_B, Letter_I, Letter_A};
  createImageMiddle(image, images, 6, 1);
}
*/

void createHotel(bool image[WIDTH][HEIGHT]) 
{
  using namespace Font4x7;
  TImage images[] = {Letter_H, Letter_O, Letter_T, Letter_E, Letter_L};
  createImageMiddle(image, images, 5, 1);
}

void createNoir(bool image[WIDTH][HEIGHT]) 
{
  using namespace Font4x7;
  TImage images[] = {Letter_N, Letter_O, Letter_I, Letter_R};
  createImageMiddle(image, images, 4, 1);
}

void createError(bool image[WIDTH][HEIGHT])
{
  using namespace Font4x7;
  TImage images[] = {Letter_E, Letter_R, Letter_R, Letter_O, Letter_R};
  createImageMiddle(image, images, 5, 1);
}

void createSpecial(bool image[WIDTH][HEIGHT])
{
  TImage images[] = {Letter_K, PlusSign, Letter_PRus, EqualSign, Heart};
  createImageMiddle(image, images, 5, 1);
}

void createDigits(bool image[WIDTH][HEIGHT], int* digits, int size)
{
  using namespace Font4x7;
  TImage images[10];

  int delta = 0;
  int imagesSize = size + 1;
  if (size < 3) {
    imagesSize = 4;
    delta = imagesSize - size - 1;
    images[0] = GetImage('0');
  }

  if (size < 2) {
    images[2] = GetImage('0');
  }

  if (size < 1) {
    images[3] = GetImage('0');
  }
  
  images[imagesSize - 3] = Point;
  
  for (int i = 0; i < size; ++i) {    
    int imageIndex = delta + i;
    if (imageIndex >= imagesSize - 3) {
      ++imageIndex;
    }
    images[imageIndex] = GetImage('0' + digits[i]);
  }

  createImageRight(image, images, imagesSize);
}

/////////////////////////////////////////////////////////////

