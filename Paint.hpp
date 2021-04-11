#pragma once
#include "c:\Program Files\TX\TXLib.h"
#include <cstdlib>
#include <string>
#include <cmath>
#include <ctime>

#define RED RGB(255, 36, 0)
#define GREEN RGB(102, 255, 0)
#define BLUE RGB(125, 249, 255)

typedef void (*func_t)(void);
typedef double (*graph_t)(double x);

struct Point
{
	double x, y;
};

void pencil();
void fill(double x, double y, COLORREF current);
void cover_for_fill();

func_t funcOfDrawing = &pencil;

COLORREF colorForPainting = TX_BLACK;
bool isParametrsChanged = true;
double radiusForPainting = 24;
const Point coordLT_OfBut = { 0, 0 };
const Point coordRB_OfBut = { 1200, 100 };
const Point coordLT_OfMainWindow = { 0, 100 };
const Point coordRB_OfMainWindow = { 1200, 600 };

class Window
{
public:
	Point coordLT, coordRB;

	virtual void DrawWindow()
	{
		txSetColor(TX_LIGHTGRAY);
		txSetFillColor(TX_WHITE);
		txRectangle(coordLT.x, coordLT.y, coordRB.x, coordRB.y);
	}

	Window(Point coordLT_, Point coordRB_) : coordLT(coordLT_), coordRB(coordRB_)
	{
	}
};

class ColorWindow : public Window
{
public:
	virtual void DrawWindow() override
	{
		txSetColor(TX_LIGHTGRAY);
		txSetFillColor(TX_WHITE);
		txRectangle(coordLT.x, coordLT.y, coordRB.x, coordRB.y);

		txSetFillColor(colorForPainting);
		txCircle((coordLT.x + coordRB.x) / 2, (coordLT.y + coordRB.y) / 2, radiusForPainting / 2);
	}

	ColorWindow(Point coordLT_, Point coordRB_) : Window(coordLT_, coordRB_)
	{
	}
};

class BasicButton : public Window
{
public:
	virtual void DrawButton()
	{
		txSetColor(TX_LIGHTGRAY);
		txSetFillColor(TX_WHITE);
		txRectangle(coordLT.x, coordLT.y, coordRB.x, coordRB.y);
	}

	virtual bool IfButtonPressed()
	{
		const int leftMouseButton = 1;

		if (txMouseButtons() == leftMouseButton &&
			txMouseX() >= coordLT.x &&
			txMouseX() <= coordRB.x &&
			txMouseY() >= coordLT.y &&
			txMouseY() <= coordRB.y)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	virtual void Action()
	{
	}

	BasicButton(Point coordLT_, Point coordRB_) : Window(coordLT_, coordRB_)
	{
	}
};

class SetColorButton : public BasicButton
{
public:
	COLORREF color;

	virtual void DrawButton() override
	{
		txSetColor(TX_LIGHTGRAY);
		txSetFillColor(color);
		txRectangle(coordLT.x, coordLT.y, coordRB.x, coordRB.y);
	}

	virtual void Action() override
	{
		isParametrsChanged = true;
		colorForPainting = color;
	}

	SetColorButton(Point coordLT_, Point coordRB_, COLORREF color_) : BasicButton(coordLT_, coordRB_),
		color(color_)
	{
	}
};

class ScrollBar : public BasicButton
{
public:
	COLORREF rgbColor;
	Point coordLT_OfBar;
	Point coordRB_OfBar;
	double* scroller;
	double difference;
	const int width = 7;

	virtual void DrawButton() override
	{
		txSetColor(TX_LIGHTGRAY);
		txSetFillColor(TX_WHITE);
		txRectangle(coordLT.x - width, coordLT.y, coordRB.x + width, coordRB.y);

		coordLT_OfBar = { (coordRB.x + coordLT.x) / 2 - width, coordLT.y };
		coordRB_OfBar = { (coordRB.x + coordLT.x) / 2 + width, coordRB.y };

		txSetColor(TX_LIGHTGRAY);
		txSetFillColor(rgbColor);
		txRectangle(coordLT_OfBar.x, coordLT_OfBar.y, coordRB_OfBar.x, coordRB_OfBar.y);
	}

	virtual void ActionWithRevolvable()
	{
		*scroller = ((coordRB_OfBar.x + coordLT_OfBar.x) / 2 - coordLT.x) / difference + 2;
	}

	virtual void Action() override
	{
		isParametrsChanged = true;
		double scroll = txMouseX();

		txSetColor(TX_LIGHTGRAY);
		txSetFillColor(TX_WHITE);
		txRectangle(coordLT.x - width, coordLT.y, coordRB.x + width, coordRB.y);

		if (scroll > coordLT.x - width && scroll < coordRB.x + width)
		{
			coordLT_OfBar.x = scroll - width;
			coordRB_OfBar.x = scroll + width;

			ActionWithRevolvable();

			txSetFillColor(rgbColor);
			txRectangle(coordLT_OfBar.x, coordLT_OfBar.y, coordRB_OfBar.x, coordRB_OfBar.y);
			txSleep(0.1);
		}
	}

	ScrollBar(Point coordLT_, Point coordRB_, double* scroller_, double difference_, COLORREF rgbColor_ = TX_LIGHTGRAY) : BasicButton(coordLT_, coordRB_),
		scroller(scroller_), difference(difference_), rgbColor(rgbColor_)
	{
	}
};

class RGB_ScrollBar : public ScrollBar
{
public:
	COLORREF* scrollerColor;
	double rgbScroller;

	virtual void ActionWithRevolvable() override
	{
		rgbScroller = 255 * ((coordRB_OfBar.x + coordLT_OfBar.x) / 2 - coordLT.x) / (coordRB.x - coordLT.x);

		if (rgbColor == RED)
		{
			*scrollerColor = RGB(rgbScroller, txExtractColor(*scrollerColor, TX_GREEN), txExtractColor(*scrollerColor, TX_BLUE));
		}
		else if (rgbColor == GREEN)
		{
			*scrollerColor = RGB(txExtractColor(*scrollerColor, TX_RED), rgbScroller, txExtractColor(*scrollerColor, TX_BLUE));
		}
		else if (rgbColor == BLUE)
		{
			*scrollerColor = RGB(txExtractColor(*scrollerColor, TX_RED), txExtractColor(*scrollerColor, TX_GREEN), rgbScroller);
		}
	}

	RGB_ScrollBar(Point coordLT_, Point coordRB_, COLORREF* scroller_, COLORREF rgbColor_) : ScrollBar(coordLT_, coordRB_, NULL, NULL, rgbColor_),
		scrollerColor(scroller_)
	{
	}
};