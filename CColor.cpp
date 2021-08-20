#include "CColor.h"

void Color::SetColor(int r, int g, int b, int a)
{
	_color[0] = (unsigned char)r;
	_color[1] = (unsigned char)g;
	_color[2] = (unsigned char)b;
	_color[3] = (unsigned char)a;
}
void Color::GetColor(int& r, int& g, int& b, int& a) const
{
	r = _color[0];
	g = _color[1];
	b = _color[2];
	a = _color[3];
}
int Color::r(void) { return _color[0]; }
int Color::g(void) { return _color[1]; }
int Color::b(void) { return _color[2]; }
int Color::a(void) { return _color[3]; }
unsigned char& Color::operator[](int index)
{
	return _color[index];
}
bool Color::operator == (Color& rhs) const
{
	return (_color[0] == rhs._color[0] && _color[1] == rhs._color[1] && _color[2] == rhs._color[2] && _color[3] == rhs._color[3]);
}
bool Color::operator != (Color& rhs) const
{
	return !(operator == (rhs));
}