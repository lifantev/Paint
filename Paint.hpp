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

void Pencil();
void Fill(double x, double y, COLORREF current);
void CoverForFill();
void Spray();
void Clear();
void ExitFromProgram();
void SaveImage();
void OpenImage();
void PrevState();
void NextState();

func_t FuncOfDrawing = &Pencil;

COLORREF colorForPainting = TX_BLACK;
bool isParametrsChanged = true;
double radiusForPainting = 24;
const Point COORD_LT_OF_BUT = { 0, 0 };
const Point COORD_RB_OF_BUT = { 1200, 100 };
const Point COORD_LT_OF_MAIN_WINDOW = { 0, 100 };
const Point COORD_RB_OF_MAIN_WINDOW = { 1200, 600 };
const int LEFT_MOUSE_BUTTON = 1;
const int NUM_OF_CANVAS = 5000;
const int NUM_OF_BUTTONS = 50;
HDC virtualCanvas = txCreateCompatibleDC(COORD_RB_OF_MAIN_WINDOW.x - COORD_LT_OF_MAIN_WINDOW.x, COORD_RB_OF_MAIN_WINDOW.y - COORD_LT_OF_MAIN_WINDOW.y);


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
		if (txMouseButtons() == LEFT_MOUSE_BUTTON &&
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

class FunctionalButton : public BasicButton
{
public:
	const char* name;
	func_t function;

	virtual void DrawButton() override
	{
		txSetColor(TX_LIGHTGRAY);
		txSetFillColor(TX_WHITE);
		txRectangle(coordLT.x, coordLT.y, coordRB.x, coordRB.y);
		txDrawText(coordLT.x, coordLT.y, coordRB.x, coordRB.y, name);
	}

	virtual bool IfButtonPressed() override
	{
		if (txMouseButtons() == LEFT_MOUSE_BUTTON &&
			txMouseX() >= coordLT.x &&
			txMouseX() <= coordRB.x &&
			txMouseY() >= coordLT.y &&
			txMouseY() <= coordRB.y)
		{
			while (txMouseButtons() == LEFT_MOUSE_BUTTON)
			{
				txSleep(0.1);
			}
			return true;
		}
		else
		{
			return false;
		}
	}

	virtual void Action() override
	{
		function();
	}

	FunctionalButton(Point coordLT_, Point coordRB_, const char* name_, func_t function_) : BasicButton(coordLT_, coordRB_),
		function(function_),
		name(name_)
	{
	}
};

class InstrumentalButton : public BasicButton
{
public:
	const char* sourceImage;
	func_t function;

	virtual void DrawButton() override
	{
		txSetColor(TX_LIGHTGRAY, 3);
		txSetFillColor(TX_TRANSPARENT);
		HDC sourceImageHDC = txLoadImage(sourceImage);

		txTransparentBlt(coordLT.x, coordRB.y, sourceImageHDC);
		txDeleteDC(sourceImageHDC);
		txRectangle(coordLT.x, coordLT.y, coordRB.x, coordRB.y);
	}

	void RedrawButton1()
	{
		txSetColor(TX_GRAY, 5);
		txSetFillColor(TX_TRANSPARENT);
		txRectangle(coordLT.x, coordLT.y, coordRB.x, coordRB.y);
	}

	void RedrawButton2()
	{
		txSetColor(TX_WHITE, 5);
		txSetFillColor(TX_TRANSPARENT);
		txSleep();

		txRectangle(coordLT.x, coordLT.y, coordRB.x, coordRB.y);
		txSetColor(TX_LIGHTGRAY, 3);
		txRectangle(coordLT.x, coordLT.y, coordRB.x, coordRB.y);
	}

	virtual void Action() override
	{
		RedrawButton1();
		FuncOfDrawing = function;
		RedrawButton2();
	}

	InstrumentalButton(Point coordLT_, Point coordRB_, func_t function_, const char* sourceImage_) : BasicButton(coordLT_, coordRB_),
		function(function_),
		sourceImage(sourceImage_)
	{
	}
};

class Canvas : public BasicButton
{
public:
	HDC canvas[NUM_OF_CANVAS] = {};
	func_t function;
	int indexOfCanvas;

	void Add(HDC canvas_)
	{
		int i;
		for (i = 0; canvas[i] != NULL; ++i)
		{
		}
		indexOfCanvas = i - 1;
		canvas[i] = txCreateCompatibleDC(COORD_RB_OF_MAIN_WINDOW.x - COORD_LT_OF_MAIN_WINDOW.x,
			COORD_RB_OF_MAIN_WINDOW.y - COORD_LT_OF_MAIN_WINDOW.y);
		txBitBlt(canvas[i], 0, 0, COORD_RB_OF_MAIN_WINDOW.x - COORD_LT_OF_MAIN_WINDOW.x,
			COORD_RB_OF_MAIN_WINDOW.y - COORD_LT_OF_MAIN_WINDOW.y, canvas_);
	}

	virtual void DrawButton() override
	{
		txSetColor(TX_LIGHTGRAY, 2, virtualCanvas);
		txSetFillColor(TX_WHITE, virtualCanvas);
		txRectangle(coordLT.x, coordLT.y - COORD_LT_OF_MAIN_WINDOW.y, coordRB.x, coordRB.y - COORD_LT_OF_MAIN_WINDOW.y, virtualCanvas);
		txBitBlt(COORD_LT_OF_MAIN_WINDOW.x, COORD_LT_OF_MAIN_WINDOW.y, virtualCanvas);
		Add(virtualCanvas);
		Add(virtualCanvas);
	}

	virtual void Action() override
	{
		virtualCanvas = txCreateCompatibleDC(COORD_RB_OF_MAIN_WINDOW.x - COORD_LT_OF_MAIN_WINDOW.x, COORD_RB_OF_MAIN_WINDOW.y - COORD_LT_OF_MAIN_WINDOW.y);
		int i;
		for (i = indexOfCanvas + 2; canvas[i] != NULL; ++i)
		{
			canvas[i] = NULL;
		}
		txBitBlt(canvas[indexOfCanvas + 1], 0, 0, COORD_RB_OF_MAIN_WINDOW.x - COORD_LT_OF_MAIN_WINDOW.x,
			COORD_RB_OF_MAIN_WINDOW.y - COORD_LT_OF_MAIN_WINDOW.y, canvas[indexOfCanvas]);
		function();
		txBitBlt(virtualCanvas, 0, 0, COORD_RB_OF_MAIN_WINDOW.x - COORD_LT_OF_MAIN_WINDOW.x,
			COORD_RB_OF_MAIN_WINDOW.y - COORD_LT_OF_MAIN_WINDOW.y, ReturnLastCanvas());
		Add(virtualCanvas);
	}

	HDC ReturnLastCanvas()
	{
		int i;
		for (i = 0; canvas[i] != NULL; ++i)
		{
		}
		indexOfCanvas = i - 1;
		return canvas[i - 1];
	}

	HDC ReturnCurrentCanvas()
	{
		if (indexOfCanvas <= 0)
		{
			indexOfCanvas = 0;
			return canvas[0];
		}
		else if (canvas[indexOfCanvas] == NULL)
		{
			indexOfCanvas = indexOfCanvas - 1;
			return ReturnLastCanvas();
		}
		else
		{
			return canvas[indexOfCanvas];
		}
	}

	Canvas(Point coordLT, Point coordRB, func_t function_, int indexOfCanvas_ = 0) : BasicButton(coordLT, coordRB),
		function(function_), indexOfCanvas(indexOfCanvas_)
	{
	}

	~Canvas()
	{
		for (int i = 0; canvas[i] != NULL; ++i)
		{
			txDeleteDC(canvas[i]);
		}
		txDeleteDC(virtualCanvas);
	}
};

Canvas windowForPainting({ 0, 100 }, { 1200, 600 }, FuncOfDrawing);
Window windowForButtons({ 0, 0 }, { 1200, 100 });
ColorWindow indicator({ 910, 3 }, { 1003, 96 });

class ManagerButton
{
public:
	BasicButton* buttons[NUM_OF_BUTTONS];
	void Add(BasicButton* button)
	{
		int i;
		for (i = 0; buttons[i] != NULL; ++i)
		{
		}
		buttons[i] = button;
	}

	void DrawButtons()
	{
		for (int i = 0; buttons[i] != NULL; i++)
		{
			(*buttons[i]).DrawButton();
		}
	}

	void Run()
	{
		while (true)
		{
			for (int i = 0; buttons[i] != NULL; i++)
			{
				if ((*buttons[i]).IfButtonPressed())
				{
					(*buttons[i]).Action();
				}
			}
			windowForPainting.function = FuncOfDrawing;
			if (isParametrsChanged == true)
			{
				indicator.DrawWindow();
				isParametrsChanged = false;
			}
		}
	}

	ManagerButton()
	{
		for (int i = 0; i < NUM_OF_BUTTONS; i++)
		{
			buttons[i] = NULL;
		}
	}

	~ManagerButton()
	{
		for (int i = 0; buttons[i] != NULL; i++)
		{
			delete buttons[i];
		}
	}
};

ManagerButton manager;
