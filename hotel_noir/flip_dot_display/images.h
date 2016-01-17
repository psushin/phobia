#define HEIGHT 7

/////////////////////////////////////////////////////////////

struct TImage
{
	int Width;
	byte (*Data)[HEIGHT];

	TImage(int width, byte data[][HEIGHT])
		: Width(width)
		, Data(data)
	{ }
};

/////////////////////////////////////////////////////////////

byte Letter_KData[][HEIGHT] = {
	{false, true, true, true, true, true, false},
	{false, false, false, true, false, false, false},
	{false, false, true, false, true, false, false},
	{false, true, false, false, false, true, false},
}; 
TImage Letter_K(4, Letter_KData);

byte Letter_PRusData[][HEIGHT] = {
	{false, true, true, true, true, true, false},
	{false, true, false, false, false, false, false},
	{false, true, false, false, false, false, false},
	{false, true, true, true, true, true, false},
};
TImage Letter_PRus(4, Letter_PRusData);

byte PlusSignData[][HEIGHT] = {
	{false, false, false, true, false, false, false},
	{false, false, true, true, true, false, false},
	{false, false, false, true, false, false, false}
};
TImage PlusSign(3, PlusSignData);

byte EqualSignData[][HEIGHT] = {
	{false, false, true, false, true, false, false},
	{false, false, true, false, true, false, false},
	{false, false, true, false, true, false, false},
};
TImage EqualSign(3, EqualSignData);

byte __Heart[][HEIGHT] = {
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

byte __Letter_a[][HEIGHT] = {
	{false, false, false, true, true, false, false},
	{false, false, true, false, true, false, false},
	{false, false, true, true, true, false, false},
};
TImage Letter_a(3, __Letter_a);

byte __Letter_b[][HEIGHT] = {
	{1, 1, 1, 1, 1, 0, 0},
	{0, 0, 1, 0, 1, 0, 0},
	{0, 0, 0, 1, 0, 0, 0},
};
TImage Letter_b(3, __Letter_b);

byte __Letter_h[][HEIGHT] = {
	{1, 1, 1, 1, 1, 0, 0},
	{0, 0, 1, 0, 0, 0, 0},
	{0, 0, 0, 1, 1, 0, 0},
};
TImage Letter_h(3, __Letter_h);

byte __Letter_i[][HEIGHT] = {
	{1, 0, 1, 1, 1, 0, 0},
};
TImage Letter_i(1, __Letter_i);

byte __Letter_o[][HEIGHT] = {
	{0, 0, 0, 1, 0, 0, 0},
	{0, 0, 1, 0, 1, 0, 0},
	{0, 0, 0, 1, 0, 0, 0},
};
TImage Letter_o(3, __Letter_o);

byte __Letter_p[][HEIGHT] = {
	{0, 0, 0, 1, 1, 1, 1},
	{0, 0, 1, 0, 1, 0, 0},
	{0, 0, 0, 1, 0, 0, 0},
};
TImage Letter_p(3, __Letter_p);

} // namespace Font3x7

/////////////////////////////////////////////////////////////

namespace Font4x7 {

byte __Letter_A[][HEIGHT] = {
	{0, 0, 1, 1, 1, 1, 0},
	{0, 1, 0, 0, 1, 0, 0},
	{0, 1, 0, 0, 1, 0, 0},
	{0, 0, 1, 1, 1, 1, 0},
};
TImage Letter_A(4, __Letter_A);

byte __Letter_B[][HEIGHT] = {
	{0, 1, 1, 1, 1, 1, 0},
	{0, 1, 0, 1, 0, 1, 0},
	{0, 1, 0, 1, 0, 1, 0},
	{0, 0, 1, 0, 1, 0, 0},
};
TImage Letter_B(4, __Letter_B);

byte __Letter_E[][HEIGHT] = {
  {0, 1, 1, 1, 1, 1, 0},
  {0, 1, 0, 1, 0, 1, 0},
  {0, 1, 0, 0, 0, 1, 0},
};
TImage Letter_E(3, __Letter_E);

byte __Letter_H[][HEIGHT] = {
  {0, 1, 1, 1, 1, 1, 0},
  {0, 0, 0, 1, 0, 0, 0},
  {0, 0, 0, 1, 0, 0, 0},
  {0, 1, 1, 1, 1, 1, 0},
};
TImage Letter_H(4, __Letter_H);

byte __Letter_L[][HEIGHT] = {
	{0, 1, 1, 1, 1, 1, 0},
	{0, 0, 0, 0, 0, 1, 0},
	{0, 0, 0, 0, 0, 1, 0},
};
TImage Letter_L(3, __Letter_L);

byte __Letter_I[][HEIGHT] = {
	{0, 1, 0, 0, 0, 1, 0},
	{0, 1, 1, 1, 1, 1, 0},
	{0, 1, 0, 0, 0, 1, 0},
};
TImage Letter_I(3, __Letter_I);

byte __Letter_N[][HEIGHT] = {
  {0, 1, 1, 1, 1, 1, 0},
  {0, 0, 1, 0, 0, 0, 0},
  {0, 0, 0, 1, 0, 0, 0},
  {0, 1, 1, 1, 1, 1, 0},
};
TImage Letter_N(4, __Letter_N);

byte __Letter_O[][HEIGHT] = {
	{0, 0, 1, 1, 1, 0, 0},
	{0, 1, 0, 0, 0, 1, 0},
	{0, 1, 0, 0, 0, 1, 0},
	{0, 0, 1, 1, 1, 0, 0},
};
TImage Letter_O(4, __Letter_O);

byte __Letter_P[][HEIGHT] = {
	{0, 1, 1, 1, 1, 1, 0},
	{0, 1, 0, 0, 1, 0, 0},
	{0, 1, 0, 0, 1, 0, 0},
	{0, 0, 1, 1, 0, 0, 0},
};
TImage Letter_P(4, __Letter_P);

byte __Letter_R[][HEIGHT] = {
  {0, 1, 1, 1, 1, 1, 0},
  {0, 1, 0, 1, 0, 0, 0},
  {0, 1, 0, 1, 1, 0, 0},
  {0, 0, 1, 0, 0, 1, 0},
};
TImage Letter_R(4, __Letter_R);

byte __Letter_T[][HEIGHT] = {
  {0, 1, 0, 0, 0, 0, 0},
  {0, 1, 0, 0, 0, 0, 0},
  {0, 1, 1, 1, 1, 1, 0},
  {0, 1, 0, 0, 0, 0, 0},
};
TImage Letter_T(4, __Letter_T);

} // namespace Font4x7
