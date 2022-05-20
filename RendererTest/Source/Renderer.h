#pragma once

#include <Windows.h>
#include "Common.h"
#include "Objects/FrameBuffer.h"

class Renderer
{
	FrameBuffer* target;

public:
	Renderer(FrameBuffer* target);

	void SetPixel(int x, int y, Color32 color);
	Color32 GetPixel(int x, int y);
	void Clear(Color32 color);

	void DrawHorizontalLine(int x1, int x2, int y, Color32 color);
	void DrawVerticalLine(int x, int y1, int y2, Color32 color);
	void DrawLowLine(int x1, int y1, int x2, int y2, Color32 color);
	void DrawHighLine(int x1, int y1, int x2, int y2, Color32 color);
	void DrawLine(int x1, int y1, int x2, int y2, Color32 color);

	void FillCircle(int x, int y, int r, Color32 color);
};