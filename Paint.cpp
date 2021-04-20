#include "Paint.hpp"

int main()
{
	txCreateWindow(1200, 600);
	ManagerButton manager;

	manager.Add(&windowForPainting);

	manager.Add(new SetColorButton({ 505, 10 }, { 530, 35 }, TX_BLACK));
	manager.Add(new SetColorButton({ 530, 10 }, { 555, 35 }, TX_WHITE));
	manager.Add(new SetColorButton({ 555, 10 }, { 580, 35 }, TX_LIGHTBLUE));
	manager.Add(new SetColorButton({ 580, 10 }, { 605, 35 }, TX_LIGHTRED));
	manager.Add(new SetColorButton({ 605, 10 }, { 630, 35 }, TX_LIGHTGREEN));
	manager.Add(new SetColorButton({ 630, 10 }, { 655, 35 }, TX_YELLOW));
	manager.Add(new SetColorButton({ 655, 10 }, { 680, 35 }, TX_ORANGE));
	manager.Add(new SetColorButton({ 680, 10 }, { 705, 35 }, TX_PINK));
	manager.Add(new SetColorButton({ 705, 10 }, { 730, 35 }, TX_MAGENTA));

	manager.Add(new FunctionalButton({ 0, 0 }, { 100, 50 }, "open", OpenImage));
	manager.Add(new FunctionalButton({ 100, 0 }, { 200, 50 }, "save", SaveImage));
	manager.Add(new FunctionalButton({ 100, 50 }, { 200, 100 }, "exit", ExitFromProgram));
	manager.Add(new FunctionalButton({ 1020, 20 }, { 1120, 80 }, "clear", Clear));
	manager.Add(new FunctionalButton({ 0, 50 }, { 50, 100 }, "<<", PrevState));
	manager.Add(new FunctionalButton({ 50, 50 }, { 100, 100 }, ">>", NextState));

	manager.Add(new InstrumentalButton({ 225, 10 }, { 255, 40 }, Eraser, "eraser.bmp"));
	manager.Add(new InstrumentalButton({ 260, 10 }, { 290, 40 }, Pencil, "pencil.bmp"));
	manager.Add(new InstrumentalButton({ 295, 10 }, { 325, 40 }, Spray, "spray.bmp"));
	
	// TODO: filling, now works with stack corruption most of the time
	//manager.Add(new InstrumentalButton({ 320, 10 }, { 350, 40 }, CoverForFill, "filling.bmp"));

	manager.Add(new ScrollBar({ 267, 60 }, { 507, 90 }, &radiusForPainting, 6));

	manager.Add(new RGB_ScrollBar({ 790, 3 }, { 900, 32 }, &colorForPainting, RGB(255, 36, 0)));
	manager.Add(new RGB_ScrollBar({ 790, 35 }, { 900, 64 }, &colorForPainting, RGB(102, 255, 0)));
	manager.Add(new RGB_ScrollBar({ 790, 67 }, { 900, 96 }, &colorForPainting, RGB(125, 249, 255)));

	windowForButtons.DrawWindow();

	manager.DrawButtons();

	manager.Run();
}

void Clear()
{
	txBitBlt(windowForPainting.canvas[windowForPainting.indexOfCanvas], 0, 0, COORD_RB_OF_MAIN_WINDOW.x - COORD_LT_OF_MAIN_WINDOW.x,
		COORD_RB_OF_MAIN_WINDOW.y - COORD_LT_OF_MAIN_WINDOW.y, windowForPainting.canvas[0]);
	txBitBlt(COORD_LT_OF_MAIN_WINDOW.x, COORD_LT_OF_MAIN_WINDOW.y, windowForPainting.canvas[0]);
}

void ExitFromProgram()
{
	windowForPainting.~Canvas();
	exit(EXIT_SUCCESS);
}

void Pencil()
{
	Point coords1 = { txMouseX(), txMouseY() };
	while (txMouseButtons() == LEFT_MOUSE_BUTTON)
	{
		Point coords2 = { txMouseX(), txMouseY() };
		txSetColor(colorForPainting, radiusForPainting, windowForPainting.ReturnLastCanvas());
		txLine(coords1.x, coords1.y - COORD_LT_OF_MAIN_WINDOW.y, coords2.x, coords2.y - COORD_LT_OF_MAIN_WINDOW.y, windowForPainting.ReturnLastCanvas());
		txBitBlt(COORD_LT_OF_MAIN_WINDOW.x, COORD_LT_OF_MAIN_WINDOW.y, windowForPainting.ReturnLastCanvas());
		txSleep();
		coords1 = coords2;
	}
}

void Eraser()
{
	COLORREF previousColor = colorForPainting;
	colorForPainting = TX_WHITE;
	Pencil();
	colorForPainting = previousColor;
}

void Spray()
{
	srand(time(0));
	int random = rand();
	while (txMouseButtons() == LEFT_MOUSE_BUTTON)
	{
		Point coords = { txMouseX(), txMouseY() };
		for (int x = -radiusForPainting / 2; x <= radiusForPainting / 2; x++)
		{
			for (int y = -radiusForPainting / 2; y <= radiusForPainting / 2; y++)
			{
				if ((sqrt(x * x + y * y) <= radiusForPainting / 2) && (coords.y - y > COORD_LT_OF_MAIN_WINDOW.y))
				{
					srand(random);
					random = rand();
					if (random % 2 == 1)
					{
						txSetPixel(coords.x - x, coords.y - y, colorForPainting);
						txSetPixel(coords.x - x, coords.y - y - COORD_LT_OF_MAIN_WINDOW.y, colorForPainting, windowForPainting.ReturnLastCanvas());
					}
					int time = 0;
					while (time < 7000)
						time++;
				}
				if (txMouseButtons() == 0)
					break;
			}
		}
		txSleep(1);
	}
}

void SaveImage()
{
	txSaveImage("image.bmp");
}

void OpenImage()
{
	HDC sourceImage = txLoadImage("image.bmp");
	txTransparentBlt(0, 0, sourceImage);
	txDeleteDC(sourceImage);
}

void PrevState()
{
	if (windowForPainting.canvas[windowForPainting.indexOfCanvas + 1] == NULL)
		windowForPainting.indexOfCanvas -= 2;
	else
		windowForPainting.indexOfCanvas -= 1;
	txBitBlt(COORD_LT_OF_MAIN_WINDOW.x, COORD_LT_OF_MAIN_WINDOW.y, windowForPainting.ReturnCurrentCanvas());
}

void NextState()
{
	if (windowForPainting.canvas[windowForPainting.indexOfCanvas + 1] != NULL)
		windowForPainting.indexOfCanvas += 1;
	txBitBlt(COORD_LT_OF_MAIN_WINDOW.x, COORD_LT_OF_MAIN_WINDOW.y, windowForPainting.ReturnCurrentCanvas());
	if (windowForPainting.canvas[windowForPainting.indexOfCanvas + 1] == NULL)
		windowForPainting.indexOfCanvas -= 1;
}