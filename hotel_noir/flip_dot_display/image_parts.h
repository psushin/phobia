#pragma once

#define HEIGHT 7

/////////////////////////////////////////////////////////////

struct TImage
{
  int Width = 0;
  const byte (*Data)[HEIGHT] = NULL;

  TImage()
  { } 

  TImage(int width, const byte data[][HEIGHT])
    : Width(width)
    , Data(data)
  { }
};

/////////////////////////////////////////////////////////////

const byte Letter_KData[][HEIGHT] PROGMEM = {
  {0, 1, 1, 1, 1, 1, 0},
  {0, 0, 0, 1, 0, 0, 0},
  {0, 0, 1, 0, 1, 0, 0},
  {0, 1, 0, 0, 0, 1, 0},
}; 
TImage Letter_K(4, Letter_KData);

const byte Letter_KData1[][HEIGHT] = {
  {0, 1, 1, 1, 1, 1, 0},
  {0, 0, 0, 1, 0, 0, 0},
  {0, 0, 1, 0, 1, 0, 0},
  {0, 1, 0, 0, 0, 1, 0},
}; 

const byte Letter_PRusData[][HEIGHT] PROGMEM = {
  {false, true, true, true, true, true, false},
  {false, true, false, false, false, false, false},
  {false, true, false, false, false, false, false},
  {false, true, true, true, true, true, false},
};
TImage Letter_PRus(4, Letter_PRusData);

const byte PlusSignData[][HEIGHT] PROGMEM = {
  {false, false, false, true, false, false, false},
  {false, false, true, true, true, false, false},
  {false, false, false, true, false, false, false}
};
TImage PlusSign(3, PlusSignData);

const byte EqualSignData[][HEIGHT] PROGMEM = {
  {false, false, true, false, true, false, false},
  {false, false, true, false, true, false, false},
  {false, false, true, false, true, false, false},
};
TImage EqualSign(3, EqualSignData);

const byte __Heart[][HEIGHT] PROGMEM = {
  {false, false, true, true, false, false, false},
  {false, true, false, false, true, false, false},
  {false, false, true, false, false, true, false},
  {false, true, false, false, true, false, false},
  {false, false, true, true, false, false, false},
};
TImage Heart(5, __Heart);

/////////////////////////////////////////////////////////////

namespace Font3x7 {

// Small english letters 3 x 7

const byte __Letter_a[][HEIGHT] PROGMEM = {
  {false, false, false, true, true, false, false},
  {false, false, true, false, true, false, false},
  {false, false, true, true, true, false, false},
};
TImage Letter_a(3, __Letter_a);

const byte __Letter_b[][HEIGHT] PROGMEM = {
  {1, 1, 1, 1, 1, 0, 0},
  {0, 0, 1, 0, 1, 0, 0},
  {0, 0, 0, 1, 0, 0, 0},
};
TImage Letter_b(3, __Letter_b);

const byte __Letter_h[][HEIGHT] PROGMEM = {
  {1, 1, 1, 1, 1, 0, 0},
  {0, 0, 1, 0, 0, 0, 0},
  {0, 0, 0, 1, 1, 0, 0},
};
TImage Letter_h(3, __Letter_h);

const byte __Letter_i[][HEIGHT] PROGMEM = {
  {1, 0, 1, 1, 1, 0, 0},
};
TImage Letter_i(1, __Letter_i);

const byte __Letter_o[][HEIGHT] PROGMEM = {
  {0, 0, 0, 1, 0, 0, 0},
  {0, 0, 1, 0, 1, 0, 0},
  {0, 0, 0, 1, 0, 0, 0},
};
TImage Letter_o(3, __Letter_o);

const byte __Letter_p[][HEIGHT] PROGMEM = {
  {0, 0, 0, 1, 1, 1, 1},
  {0, 0, 1, 0, 1, 0, 0},
  {0, 0, 0, 1, 0, 0, 0},
};
TImage Letter_p(3, __Letter_p);

} // namespace Font3x7

/////////////////////////////////////////////////////////////

namespace Font4x7 {

const byte __Letter_A[][HEIGHT] PROGMEM = {
  {0, 0, 1, 1, 1, 1, 0},
  {0, 1, 0, 0, 1, 0, 0},
  {0, 1, 0, 0, 1, 0, 0},
  {0, 0, 1, 1, 1, 1, 0},
};
TImage Letter_A(4, __Letter_A);

const byte __Letter_B[][HEIGHT] PROGMEM = {
  {0, 1, 1, 1, 1, 1, 0},
  {0, 1, 0, 1, 0, 1, 0},
  {0, 1, 0, 1, 0, 1, 0},
  {0, 0, 1, 0, 1, 0, 0},
};
TImage Letter_B(4, __Letter_B);

const byte __Letter_E[][HEIGHT] PROGMEM = {
  {0, 1, 1, 1, 1, 1, 0},
  {0, 1, 0, 1, 0, 1, 0},
  {0, 1, 0, 0, 0, 1, 0},
};
TImage Letter_E(3, __Letter_E);

const byte __Letter_H[][HEIGHT] PROGMEM = {
  {0, 1, 1, 1, 1, 1, 0},
  {0, 0, 0, 1, 0, 0, 0},
  {0, 0, 0, 1, 0, 0, 0},
  {0, 1, 1, 1, 1, 1, 0},
};
TImage Letter_H(4, __Letter_H);

const byte __Letter_L[][HEIGHT] PROGMEM = {
  {0, 1, 1, 1, 1, 1, 0},
  {0, 0, 0, 0, 0, 1, 0},
  {0, 0, 0, 0, 0, 1, 0},
};
TImage Letter_L(3, __Letter_L);

const byte __Letter_I[][HEIGHT] PROGMEM = {
  {0, 1, 0, 0, 0, 1, 0},
  {0, 1, 1, 1, 1, 1, 0},
  {0, 1, 0, 0, 0, 1, 0},
};
TImage Letter_I(3, __Letter_I);

const byte __Letter_N[][HEIGHT] PROGMEM = {
  {0, 1, 1, 1, 1, 1, 0},
  {0, 0, 1, 0, 0, 0, 0},
  {0, 0, 0, 1, 0, 0, 0},
  {0, 1, 1, 1, 1, 1, 0},
};
TImage Letter_N(4, __Letter_N);

const byte __Letter_O[][HEIGHT] PROGMEM = {
  {0, 0, 1, 1, 1, 0, 0},
  {0, 1, 0, 0, 0, 1, 0},
  {0, 1, 0, 0, 0, 1, 0},
  {0, 0, 1, 1, 1, 0, 0},
};
TImage Letter_O(4, __Letter_O);

const byte __Letter_P[][HEIGHT] PROGMEM = {
  {0, 1, 1, 1, 1, 1, 0},
  {0, 1, 0, 0, 1, 0, 0},
  {0, 1, 0, 0, 1, 0, 0},
  {0, 0, 1, 1, 0, 0, 0},
};
TImage Letter_P(4, __Letter_P);

const byte __Letter_R[][HEIGHT] PROGMEM = {
  {0, 1, 1, 1, 1, 1, 0},
  {0, 1, 0, 1, 0, 0, 0},
  {0, 1, 0, 1, 1, 0, 0},
  {0, 0, 1, 0, 0, 1, 0},
};
TImage Letter_R(4, __Letter_R);

const byte __Letter_T[][HEIGHT] PROGMEM = {
  {0, 1, 0, 0, 0, 0, 0},
  {0, 1, 0, 0, 0, 0, 0},
  {0, 1, 1, 1, 1, 1, 0},
  {0, 1, 0, 0, 0, 0, 0},
};
TImage Letter_T(4, __Letter_T);

const byte __Digit_0[][HEIGHT] PROGMEM = {
  {0, 0, 1, 1, 1, 0, 0},
  {0, 1, 0, 0, 0, 1, 0},
  {0, 1, 0, 0, 0, 1, 0},
  {0, 0, 1, 1, 1, 0, 0},
};
TImage Digit_0(4, __Digit_0);

const byte __Digit_1[][HEIGHT] PROGMEM = {
  {0, 0, 1, 0, 0, 1, 0},
  {0, 1, 1, 1, 1, 1, 0},
  {0, 0, 0, 0, 0, 1, 0},
};
TImage Digit_1(3, __Digit_1);

const byte __Digit_2[][HEIGHT] PROGMEM = {
  {0, 0, 1, 0, 0, 1, 0},
  {0, 1, 0, 0, 1, 1, 0},
  {0, 1, 0, 1, 0, 1, 0},
  {0, 0, 1, 0, 0, 1, 0},
};
TImage Digit_2(4, __Digit_2);

const byte __Digit_3[][HEIGHT] PROGMEM = {
  {0, 1, 0, 0, 0, 1, 0},
  {0, 1, 0, 1, 0, 1, 0},
  {0, 1, 0, 1, 0, 1, 0},
  {0, 0, 1, 0, 1, 0, 0},
};
TImage Digit_3(4, __Digit_3);

const byte __Digit_4[][HEIGHT] PROGMEM = {
  {0, 0, 0, 1, 1, 0, 0},
  {0, 0, 1, 0, 1, 0, 0},
  {0, 1, 0, 0, 1, 0, 0},
  {0, 1, 1, 1, 1, 1, 0},
};
TImage Digit_4(4, __Digit_4);

const byte __Digit_5[][HEIGHT] PROGMEM = {
  {0, 1, 1, 1, 0, 1, 0},
  {0, 1, 0, 1, 0, 1, 0},
  {0, 1, 0, 1, 0, 1, 0},
  {0, 1, 0, 1, 1, 0, 0},
};
TImage Digit_5(4, __Digit_5);

const byte __Digit_6[][HEIGHT] PROGMEM = {
  {0, 0, 1, 1, 1, 0, 0},
  {0, 1, 0, 1, 0, 1, 0},
  {0, 1, 0, 1, 0, 1, 0},
  {0, 0, 0, 0, 1, 0, 0},
};
TImage Digit_6(4, __Digit_6);

const byte __Digit_7[][HEIGHT] PROGMEM = {
  {0, 1, 0, 0, 0, 0, 0},
  {0, 1, 0, 0, 0, 0, 0},
  {0, 1, 0, 1, 1, 1, 0},
  {0, 1, 1, 0, 0, 0, 0},
};
TImage Digit_7(4, __Digit_7);

const byte __Digit_8[][HEIGHT] PROGMEM = {
  {0, 0, 1, 0, 1, 0, 0},
  {0, 1, 0, 1, 0, 1, 0},
  {0, 1, 0, 1, 0, 1, 0},
  {0, 0, 1, 0, 1, 0, 0},
};
TImage Digit_8(4, __Digit_8);

const byte __Digit_9[][HEIGHT] PROGMEM = {
  {0, 0, 1, 0, 0, 0, 0},
  {0, 1, 0, 1, 0, 1, 0},
  {0, 1, 0, 1, 0, 1, 0},
  {0, 0, 1, 1, 1, 0, 0},
};
TImage Digit_9(4, __Digit_9);

const byte __Point[][HEIGHT] PROGMEM = {
  {0, 0, 0, 0, 0, 1, 0},
};
TImage Point(1, __Point);

TImage GetImage(char c)
{
  #define CASE_DIGIT(x) case #x [0] : return Digit_ ## x
  
  switch (c) {
    CASE_DIGIT(0);
    CASE_DIGIT(1);
    CASE_DIGIT(2);
    CASE_DIGIT(3);
    CASE_DIGIT(4);
    CASE_DIGIT(5);
    CASE_DIGIT(6);
    CASE_DIGIT(7);
    CASE_DIGIT(8);
    CASE_DIGIT(9);

    case '.':
      return Point;

    default:
      return Heart;
  }

  #undef CASE_DIGIT
}

} // namespace Font4x7

