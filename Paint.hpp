#pragma once
#include "c:\Program Files\TX\TXLib.h"
#include <cstdlib>
#include <string>
#include <cmath>
#include <ctime>
#include <exception>

namespace Paint {
	struct Point
	{
		double x, y;
	};

	void Pencil();
	void Eraser();
	void Fill(double x, double y, COLORREF current);
	void CoverForFill();
	void Spray();
	void Clear();
	void Exit();
	void SaveImage();
	void OpenImage();
	void PrevState();
	void NextState();

	typedef void (*func_t)(void);
	func_t FuncOfDrawing = &Pencil;

	COLORREF colorForPainting = TX_BLACK;
	const COLORREF RED = RGB(255, 36, 0);
	const COLORREF GREEN = RGB(102, 255, 0);
	const COLORREF BLUE = RGB(125, 249, 255);
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
	protected:
		Point coordLT, coordRB;

	public:
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
	protected:
		COLORREF color;

	public:
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
	protected:
		COLORREF rgbColor;
		Point coordLT_OfBar;
		Point coordRB_OfBar;
		double* scroller;
		double difference;
		const int WIDTH = 7;

	public:
		virtual void DrawButton() override
		{
			txSetColor(TX_LIGHTGRAY);
			txSetFillColor(TX_WHITE);
			txRectangle(coordLT.x - WIDTH, coordLT.y, coordRB.x + WIDTH, coordRB.y);

			coordLT_OfBar = { (coordRB.x + coordLT.x) / 2 - WIDTH, coordLT.y };
			coordRB_OfBar = { (coordRB.x + coordLT.x) / 2 + WIDTH, coordRB.y };

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
			txRectangle(coordLT.x - WIDTH, coordLT.y, coordRB.x + WIDTH, coordRB.y);

			if (scroll > coordLT.x - WIDTH && scroll < coordRB.x + WIDTH)
			{
				coordLT_OfBar.x = scroll - WIDTH;
				coordRB_OfBar.x = scroll + WIDTH;

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
	protected:
		COLORREF* scrollerColor;
		double rgbScroller = 0;

	public:
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
	protected:
		const char* name;
		func_t function;

	public:
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
	protected:
		const char* sourceImage;
		func_t function;

	public:
		virtual void DrawButton() override
		{
			txSetColor(TX_LIGHTGRAY, 3);
			txSetFillColor(TX_TRANSPARENT);
			HDC sourceImageHDC = txLoadImage(sourceImage);

			txTransparentBlt(coordLT.x, coordRB.y - 30, sourceImageHDC);
			txDeleteDC(sourceImageHDC);
			txRectangle(coordLT.x, coordLT.y, coordRB.x, coordRB.y);
		}

		void RedrawButton1()
		{
			txSetColor(TX_BLACK, 5);
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
			function(function_), sourceImage(sourceImage_)
		{
		}
	};

	class Canvas : public BasicButton
	{
	private:
		boolean isModified = false;
	public:
		func_t function;
		HDC canvas[NUM_OF_CANVAS] = {};
		int indexOfCanvas = 0;

		void Add(HDC canvas_)
		{
			if (indexOfCanvas < NUM_OF_CANVAS - 2)
			{
				canvas[indexOfCanvas] = txCreateCompatibleDC(COORD_RB_OF_MAIN_WINDOW.x - COORD_LT_OF_MAIN_WINDOW.x,
					COORD_RB_OF_MAIN_WINDOW.y - COORD_LT_OF_MAIN_WINDOW.y);
				txBitBlt(canvas[indexOfCanvas], 0, 0, COORD_RB_OF_MAIN_WINDOW.x - COORD_LT_OF_MAIN_WINDOW.x,
					COORD_RB_OF_MAIN_WINDOW.y - COORD_LT_OF_MAIN_WINDOW.y, canvas_);
				indexOfCanvas++;
			}
		}

		virtual void DrawButton() override
		{
			txSetColor(TX_LIGHTGRAY, 2, virtualCanvas);
			txSetFillColor(TX_WHITE, virtualCanvas);
			txRectangle(coordLT.x, coordLT.y - COORD_LT_OF_MAIN_WINDOW.y, coordRB.x, coordRB.y - COORD_LT_OF_MAIN_WINDOW.y, virtualCanvas);
			txBitBlt(COORD_LT_OF_MAIN_WINDOW.x, COORD_LT_OF_MAIN_WINDOW.y, virtualCanvas);
			Add(virtualCanvas);
		}

		virtual void Action() override
		{
			function();
			if (isModified == true)
			{
				isModified = false;
				for (int i = indexOfCanvas; i < NUM_OF_CANVAS - 1; ++i)
				{
					txDeleteDC(canvas[i]);
				}
			}
			Add(virtualCanvas);
		}

		void ReturnLastCanvas()
		{
			isModified = true;
			if (indexOfCanvas == 1)
			{
				txBitBlt(virtualCanvas, 0, 0, COORD_RB_OF_MAIN_WINDOW.x - COORD_LT_OF_MAIN_WINDOW.x,
					COORD_RB_OF_MAIN_WINDOW.y - COORD_LT_OF_MAIN_WINDOW.y, canvas[0]);
				txBitBlt(COORD_LT_OF_MAIN_WINDOW.x, COORD_LT_OF_MAIN_WINDOW.y, virtualCanvas);
			}
			else
			{
				indexOfCanvas -= 1;
				txBitBlt(virtualCanvas, 0, 0, COORD_RB_OF_MAIN_WINDOW.x - COORD_LT_OF_MAIN_WINDOW.x,
					COORD_RB_OF_MAIN_WINDOW.y - COORD_LT_OF_MAIN_WINDOW.y, canvas[indexOfCanvas - 1]);
				txBitBlt(COORD_LT_OF_MAIN_WINDOW.x, COORD_LT_OF_MAIN_WINDOW.y, virtualCanvas);
			}
		}

		void ReturnNextCanvas()
		{
			if (canvas[indexOfCanvas] != NULL)
			{
				txBitBlt(virtualCanvas, 0, 0, COORD_RB_OF_MAIN_WINDOW.x - COORD_LT_OF_MAIN_WINDOW.x,
					COORD_RB_OF_MAIN_WINDOW.y - COORD_LT_OF_MAIN_WINDOW.y, canvas[indexOfCanvas]);
				txBitBlt(COORD_LT_OF_MAIN_WINDOW.x, COORD_LT_OF_MAIN_WINDOW.y, virtualCanvas);
				indexOfCanvas += 1;
			}
		}

		Canvas(Point coordLT, Point coordRB, func_t function_, int indexOfCanvas_ = 0) : BasicButton(coordLT, coordRB),
			function(function_), indexOfCanvas(indexOfCanvas_)
		{
		}

		~Canvas()
		{
			for (int i = 0; i < NUM_OF_CANVAS; ++i)
			{
				txDeleteDC(canvas[i]);
			}
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
}