#include "Renderer.h"
#include <math.h>
#include <Math/Color.h>

Renderer::Renderer(FrameBuffer* target)
	: target(target)
{
}

void Renderer::SetPixel(int x, int y, Color32 color)
{
	//ASSERT(x >= 0 && x < bitmap->width && y >= 0 && y < bitmap->height,
	//	"Invalid coordinate in bitmap of size %1!d!x%2!d!: (%3!d!, %4!d!)",
	//	bitmap->width, bitmap->height, x, y);

	// TODO: Think whether this check should happen in release,
	// and if it's better to do the assert above instead.
	target->Write(x, y, { color, 0 });
}

Color32 Renderer::GetPixel(int x, int y)
{
	//ASSERT(x >= 0 && x < bitmap->width && y >= 0 && y < bitmap->height,
	//	"Invalid coordinate in bitmap of size %1!d!x%2!d!: (%3!d!, %4!d!)",
	//	bitmap->width, bitmap->height, x, y);

	// TODO: Think whether this check should happen in release,
	// and if it's better to do the assert above instead.
	return target->ReadColor(x, y);
}

void Renderer::Clear(Color32 color)
{
	target->ClearColor(color);
}

void Renderer::DrawHorizontalLine(int x1, int x2, int y, Color32 color)
{
	if (x1 > x2)
	{
		int temp = x1;
		x1 = x2;
		x2 = temp;
	}

	for (int x = x1; x <= x2; x++)
		SetPixel(x, y, color);
}

void Renderer::DrawVerticalLine(int x, int y1, int y2, Color32 color)
{
	if (y1 > y2)
	{
		int temp = y1;
		y1 = y2;
		y2 = temp;
	}

	for (int y = y1; y <= y2; y++)
		SetPixel(x, y, color);
}

void Renderer::DrawLowLine(int x1, int y1, int x2, int y2, Color32 color)
{
	if (x2 < x1)
	{
		int temp = x1;
		x1 = x2;
		x2 = temp;

		temp = y1;
		y1 = y2;
		y2 = temp;
	}

	int yDirection = 1;
	if (y1 > y2)
		yDirection = -1;

	int dx = x2 - x1;
	int dy = y2 - y1;

	int yActual = y1;
	int ydxIdeal = y1 * dx;

	for (int x = x1; x <= x2; x++)
	{
		int potentialNext = yActual + yDirection;
		int potentialError = (potentialNext * dx - ydxIdeal) * yDirection;
		int currentError = (ydxIdeal - yActual * dx) * yDirection;

		if (potentialError <= currentError)
			yActual = potentialNext;

		if (yActual < target->Height() && yActual >= 0)
			SetPixel(x, yActual, color);

		ydxIdeal += dy;
	}
}

void Renderer::DrawHighLine(int x1, int y1, int x2, int y2, Color32 color)
{
	if (y2 < y1)
	{
		int temp = x1;
		x1 = x2;
		x2 = temp;

		temp = y1;
		y1 = y2;
		y2 = temp;
	}

	int xDirection = 1;
	if (x1 > x2)
		xDirection = -1;

	int dx = x2 - x1;
	int dy = y2 - y1;

	int xActual = x1;
	int xdyIdeal = x1 * dy;
	
	for (int y = y1; y <= y2; y++)
	{
		int potentialNext = xActual + xDirection;
		int potentialError = (potentialNext * dy - xdyIdeal) * xDirection;
		int currentError = (xdyIdeal - xActual * dy) * xDirection;

		if (potentialError <= currentError)
			xActual = potentialNext;

		if (xActual < target->Width() && xActual >= 0)
			SetPixel(xActual, y, color);

		xdyIdeal += dx;
	}
}

void Renderer::DrawLine(int x1, int y1, int x2, int y2, Color32 color)
{
	if (x1 == x2)
		DrawVerticalLine(x1, y1, y2, color);
	else if (y1 == y2)
		DrawHorizontalLine(x1, x2, y1, color);
	else if (abs(y2 - y1) <= abs(x2 - x1))
		DrawLowLine(x1, y1, x2, y2, color);
	else
		DrawHighLine(x1, y1, x2, y2, color);
}

void Renderer::FillCircle(int x, int y, int r, Color32 color)
{
	// To avoid overlapping, we don't draw the axes in the algorithm, so we do it separately.
	DrawHorizontalLine(x - r, x + r, y, color);
	DrawVerticalLine(x, y - r, y - 1, color);
	DrawVerticalLine(x, y + 1, y + r, color);

	// Calculate coordinates when centered at (0, 0) and add (x, y) when drawing.
	int xi = -r;
	int yi = 1;
	int rSqr = r * r;

	// yi from 0 to r/sqrt(2).
	while (2 * yi * yi < rSqr)
	{
		int xPotential = xi + 1;
		int xIdealSqr = rSqr - yi * yi;

		int error = abs(xIdealSqr - xi * xi);
		int potentialError = abs(xPotential * xPotential - xIdealSqr);

		if (potentialError < error)
			xi = xPotential;

		for (int xFill = xi; xFill < 0; xFill++)
		{
			SetPixel(x + xFill, y + yi, color);
			SetPixel(x + xFill, y - yi, color);
			SetPixel(x - xFill, y + yi, color);
			SetPixel(x - xFill, y - yi, color);
		}

		yi++;
	}

	// xi from -r/sqrt(2) to 0
	int yFillStart = yi;
	while (xi < 0)
	{
		int yPotential = yi + 1;
		int yIdealSqr = rSqr - xi * xi;

		int error = abs(yIdealSqr - yi * yi);
		int potentialError = abs(yPotential * yPotential - yIdealSqr);

		if (potentialError < error)
			yi = yPotential;

		for (int yFill = yFillStart; yFill <= yi; yFill++)
		{
			SetPixel(x + xi, y + yFill, color);
			SetPixel(x + xi, y - yFill, color);
			SetPixel(x - xi, y + yFill, color);
			SetPixel(x - xi, y - yFill, color);
		}

		xi++;
	}
}