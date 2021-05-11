namespace Paint
{
	void Clear()
	{
		txBitBlt(virtualCanvas, 0, 0, COORD_RB_OF_MAIN_WINDOW.x - COORD_LT_OF_MAIN_WINDOW.x,
			COORD_RB_OF_MAIN_WINDOW.y - COORD_LT_OF_MAIN_WINDOW.y, windowForPainting.canvas[0]);
		txBitBlt(COORD_LT_OF_MAIN_WINDOW.x, COORD_LT_OF_MAIN_WINDOW.y, virtualCanvas);
	}

	void ExitFromProgram()
	{
		windowForPainting.~Canvas();
		txDeleteDC(virtualCanvas);
		exit(EXIT_SUCCESS);
	}

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
						while (time < 5000)
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
		HDC savingImage = txCreateCompatibleDC(COORD_RB_OF_MAIN_WINDOW.x - COORD_LT_OF_MAIN_WINDOW.x,
			COORD_RB_OF_MAIN_WINDOW.y - COORD_LT_OF_MAIN_WINDOW.y);
		txBitBlt(savingImage, 0, 0, COORD_RB_OF_MAIN_WINDOW.x - COORD_LT_OF_MAIN_WINDOW.x,
			COORD_RB_OF_MAIN_WINDOW.y - COORD_LT_OF_MAIN_WINDOW.y, virtualCanvas);
		txSaveImage("Lib\\image.bmp", savingImage);
		txDeleteDC(savingImage);
	}

	void OpenImage()
	{
		HDC sourceImage = txLoadImage("Lib\\image.bmp");
		txBitBlt(virtualCanvas, 0, 0, COORD_RB_OF_MAIN_WINDOW.x - COORD_LT_OF_MAIN_WINDOW.x,
			COORD_RB_OF_MAIN_WINDOW.y - COORD_LT_OF_MAIN_WINDOW.y, sourceImage);
		txBitBlt(COORD_LT_OF_MAIN_WINDOW.x, COORD_LT_OF_MAIN_WINDOW.y, virtualCanvas);
		txDeleteDC(sourceImage);
	}

	void PrevState()
	{
		virtualCanvas = windowForPainting.ReturnLastCanvas();
		txBitBlt(COORD_LT_OF_MAIN_WINDOW.x, COORD_LT_OF_MAIN_WINDOW.y, virtualCanvas);
	}

	void NextState()
	{
		virtualCanvas = windowForPainting.ReturnNextCanvas();
		txBitBlt(COORD_LT_OF_MAIN_WINDOW.x, COORD_LT_OF_MAIN_WINDOW.y, virtualCanvas);
	}
}