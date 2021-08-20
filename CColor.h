#pragma once
#ifndef _COLOR_H
#define _COLOR_H
class Color
{
public:
	Color(void) { SetColor(0, 0, 0, 0); }
	Color(int r, int g, int b) { SetColor(r, g, b, 0); }
	Color(int r, int g, int b, int a) { SetColor(r, g, b, a); }
public:
	virtual void SetColor(int r, int g, int b, int a);
	virtual void GetColor(int& r, int& g, int& b, int& a) const;
	virtual int r(void);
	virtual int g(void);
	virtual int b(void);
	virtual int a(void);
	virtual unsigned char& operator[](int index);
	virtual bool operator == (Color& rhs) const;
	virtual bool operator != (Color& rhs) const;
private:
	unsigned char _color[4];
};
#endif