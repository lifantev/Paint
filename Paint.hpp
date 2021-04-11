#pragma once
#include "c:\Program Files\TX\TXLib.h"
#include <cstdlib>
#include <string>
#include <cmath>
#include <ctime>

typedef void (*func_t)(void);
typedef double (*graph_t)(double x);

struct Point
{
	double x, y;
};

void pencil();
void fill(double x, double y, COLORREF current);
void cover_for_fill();

COLORREF color_for_painting = TX_BLACK;
int is_parametrs_changed = 1;
double radius_for_painting = 24;
func_t func_of_drawing = &pencil;
Point coords0_of_but = { 0, 0 };
Point coords1_of_but = { 1500, 100 };
Point coords0_of_main_window = { 0, 100 };
Point coords1_of_main_window = { 1500, 900 };

class Window
{
public:
	Point coords0_, coords1_;

	virtual void draw_window()
	{
		txSetColor(TX_LIGHTGRAY);
		txSetFillColor(TX_WHITE);
		txRectangle(coords0_.x, coords0_.y, coords1_.x, coords1_.y);
	}

	Window(Point coords0, Point coords1) : coords0_(coords0), coords1_(coords1)
	{
	}
};