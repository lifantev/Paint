#include "Paint.hpp"

int main()
{
	using namespace Paint;

	ManagerButton manager;
	try {
		txCreateWindow(1200, 600);
		manager.Add(&windowForPainting);

		manager.Add(new InstrumentalButton({ 210, 10 }, { 240, 40 }, Eraser, "Lib\\eraser.bmp"));
		manager.Add(new InstrumentalButton({ 245, 10 }, { 275, 40 }, Pencil, "Lib\\pencil.bmp"));
		manager.Add(new InstrumentalButton({ 280, 10 }, { 310, 40 }, Spray, "Lib\\spray.bmp"));
		manager.Add(new InstrumentalButton({ 315, 10 }, { 345, 40 }, Pipette, "Lib\\pipette.bmp"));
		manager.Add(new InstrumentalButton({ 350, 10 }, { 380, 40 }, Line, "Lib\\line.bmp"));
		manager.Add(new InstrumentalButton({ 385, 10 }, { 415, 40 }, Circle, "Lib\\circle.bmp"));
		manager.Add(new InstrumentalButton({ 420, 10 }, { 450, 40 }, Rectangle, "Lib\\rectangle.bmp"));

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
		manager.Add(new FunctionalButton({ 100, 50 }, { 200, 100 }, "exit", Exit));
		manager.Add(new FunctionalButton({ 1020, 20 }, { 1120, 80 }, "clear", Clear));
		manager.Add(new FunctionalButton({ 0, 50 }, { 50, 100 }, "<<", PrevState));
		manager.Add(new FunctionalButton({ 50, 50 }, { 100, 100 }, ">>", NextState));

		// TODO: filling, now works with stack corruption most of the time
		//manager.Add(new InstrumentalButton({ 455, 10 }, { 485, 40 }, Fill, "Lib\\filling.bmp"));

		manager.Add(new ScrollBar({ 267, 60 }, { 507, 90 }, &radiusForPainting, 6));

		manager.Add(new RGB_ScrollBar({ 790, 3 }, { 900, 32 }, &colorForPainting, RED));
		manager.Add(new RGB_ScrollBar({ 790, 35 }, { 900, 64 }, &colorForPainting, BLUE));
		manager.Add(new RGB_ScrollBar({ 790, 67 }, { 900, 96 }, &colorForPainting, GREEN));


		windowForButtons.DrawWindow();
		manager.DrawButtons();

		manager.Run();
	}
	catch (std::exception e)
	{
		Exit();
	}
}


namespace Paint
{
	void Clear()
	{
		txBitBlt(virtualCanvas, 0, 0, COORD_RB_OF_MAIN_WINDOW.x - COORD_LT_OF_MAIN_WINDOW.x,
			COORD_RB_OF_MAIN_WINDOW.y - COORD_LT_OF_MAIN_WINDOW.y, windowForPainting.canvas[0]);
		txBitBlt(COORD_LT_OF_MAIN_WINDOW.x, COORD_LT_OF_MAIN_WINDOW.y, virtualCanvas);
	}

	void Exit()
	{
		windowForPainting.~Canvas();
		txDeleteDC(virtualCanvas);
		exit(EXIT_SUCCESS);
	}

	void Pipette()
	{
		colorForPainting = txGetPixel(txMouseX(), txMouseY());
		isParametrsChanged = true;
	}

	//void Fill()
	//{
	//	txSetFillColor(colorForPainting);
	//	//COLORREF color = txGetPixel(txMouseX(), txMouseY());
	//	txSleep(100);
	//	//txSetColor(color);
	//	//txLine(200, 200, 100, 200, virtualCanvas);
	//	Point coords;
	//	while (txMouseButtons() == LEFT_MOUSE_BUTTON) 
	//	{
	//		coords = { txMouseX(), txMouseY() };
	//	}
	//	txFloodFill(coords.x, coords.y - COORD_LT_OF_MAIN_WINDOW.y, colorForPainting);
	//	txBitBlt(COORD_LT_OF_MAIN_WINDOW.x, COORD_LT_OF_MAIN_WINDOW.y, virtualCanvas);
	//	//txSleep();
	//	/*txSetFillColor(TX_PINK);
	//	txLine(100, 200, 150, 100);
	//	txLine(150, 100, 200, 200);
	//	txLine(200, 200, 100, 200);
	//	txFloodFill(150, 150);*/
	//}

	void Pencil()
	{
		Point coords1 = { txMouseX(), txMouseY() };
		while (txMouseButtons() == LEFT_MOUSE_BUTTON)
		{
			Point coords2 = { txMouseX(), txMouseY() };
			txSetColor(colorForPainting, radiusForPainting, virtualCanvas);
			txLine(coords1.x, coords1.y - COORD_LT_OF_MAIN_WINDOW.y, coords2.x, coords2.y - COORD_LT_OF_MAIN_WINDOW.y, virtualCanvas);
			txBitBlt(COORD_LT_OF_MAIN_WINDOW.x, COORD_LT_OF_MAIN_WINDOW.y, virtualCanvas);
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
							txSetPixel(coords.x - x, coords.y - y - COORD_LT_OF_MAIN_WINDOW.y, colorForPainting, virtualCanvas);
						}
						int time = 0;
						while (time < 3000)
							time++;
					}
					if (txMouseButtons() == 0)
						break;
				}
			}
			txSleep(1);
		}
	}

	void Line() 
	{
		txSetColor(colorForPainting, radiusForPainting, virtualCanvas);
		Point coords1 = { txMouseX(), txMouseY() };
		Point coords2;
		while (txMouseButtons() == LEFT_MOUSE_BUTTON) {
		}
		coords2 = { txMouseX(), txMouseY() };
		txLine(coords1.x, coords1.y - COORD_LT_OF_MAIN_WINDOW.y, coords2.x, coords2.y - COORD_LT_OF_MAIN_WINDOW.y, virtualCanvas);
		txBitBlt(COORD_LT_OF_MAIN_WINDOW.x, COORD_LT_OF_MAIN_WINDOW.y, virtualCanvas);
		txSleep();
	}

	void Circle() 
	{
		txSetColor(colorForPainting, radiusForPainting, virtualCanvas);
		
		Point coords1 = { txMouseX(), txMouseY() };
		Point coords2;

		while (txMouseButtons() > 0)
		{
		}
		if (GetAsyncKeyState(VK_LSHIFT))
		{
			txSetFillColor(colorForPainting, virtualCanvas);
		}
		else
		{
			txSetFillColor(TX_TRANSPARENT, virtualCanvas);
		}

		coords2 = { txMouseX(), txMouseY() };
		double displacement = sqrt((coords1.x - coords2.x) * (coords1.x - coords2.x) + (coords2.y - coords1.y) * (coords2.y - coords1.y));
		txEllipse(coords1.x - displacement, coords1.y - displacement - COORD_LT_OF_MAIN_WINDOW.y,
			coords1.x + displacement, coords1.y + displacement - COORD_LT_OF_MAIN_WINDOW.y, virtualCanvas);
		txBitBlt(COORD_LT_OF_MAIN_WINDOW.x, COORD_LT_OF_MAIN_WINDOW.y, virtualCanvas);
		txSleep(1);
	}

	void Rectangle() 
	{
		txSetColor(colorForPainting, radiusForPainting, virtualCanvas);

		Point coords1 = { txMouseX(), txMouseY() };
		Point coords2;

		while (txMouseButtons() > 0)
		{
		}
		if (GetAsyncKeyState(VK_LSHIFT))
		{
			txSetFillColor(colorForPainting, virtualCanvas);
		}
		else
		{
			txSetFillColor(TX_TRANSPARENT, virtualCanvas);
		}

		coords2 = { txMouseX(), txMouseY() };
		txRectangle(coords1.x, coords1.y - COORD_LT_OF_MAIN_WINDOW.y, coords2.x, coords2.y - COORD_LT_OF_MAIN_WINDOW.y, virtualCanvas);
		txBitBlt(COORD_LT_OF_MAIN_WINDOW.x, COORD_LT_OF_MAIN_WINDOW.y, virtualCanvas);
		txSleep(1);
	}

	void SaveImage()
	{
		HDC savingImage = txCreateCompatibleDC(COORD_RB_OF_MAIN_WINDOW.x - COORD_LT_OF_MAIN_WINDOW.x,
			COORD_RB_OF_MAIN_WINDOW.y - COORD_LT_OF_MAIN_WINDOW.y);
		txBitBlt(savingImage, 0, 0, COORD_RB_OF_MAIN_WINDOW.x - COORD_LT_OF_MAIN_WINDOW.x,
			COORD_RB_OF_MAIN_WINDOW.y - COORD_LT_OF_MAIN_WINDOW.y, virtualCanvas);
		const char* name = txInputBox ("File name: ", "Save image...", "Lib\\image.bmp");
		if (!txSaveImage(name, savingImage))
		{
			txMessageBox("Warning : image wasn't saved!");
		}
		txDeleteDC(savingImage);
	}

	void OpenImage()
	{
		const char* name = txInputBox("File name: ", "Open image...", "Lib\\image.bmp");
		HDC sourceImage = txLoadImage(name);
		if (sourceImage == NULL)
		{
			txMessageBox("Warning : image wasn't opened! Please, enter the correct filename. Only .bmp format supported.");
		} 
		else
		{
			txBitBlt(virtualCanvas, 0, 0, COORD_RB_OF_MAIN_WINDOW.x - COORD_LT_OF_MAIN_WINDOW.x,
				COORD_RB_OF_MAIN_WINDOW.y - COORD_LT_OF_MAIN_WINDOW.y, sourceImage);
			txBitBlt(COORD_LT_OF_MAIN_WINDOW.x, COORD_LT_OF_MAIN_WINDOW.y, virtualCanvas);
			isModified = true;
			windowForPainting.Add(virtualCanvas);
		}
		txDeleteDC(sourceImage);
	}

	void PrevState()
	{
		windowForPainting.ReturnLastCanvas();
	}

	void NextState()
	{
		windowForPainting.ReturnNextCanvas();
	}
}
