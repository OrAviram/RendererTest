#include "Rasterizer.h"

Rasterizer::Rasterizer() : target(nullptr)
{
}

Rasterizer::Rasterizer(FrameBuffer* target) : target(target)
{
}

void Rasterizer::DrawHorizontalLine(int x1, int x2, int y, float depth1, float depth2, Color4f color1, Color4f color2, FragmentShader shader)
{
	if (x1 > x2)
	{
		int temp = x1;
		x1 = x2;
		x2 = temp;

		Color4f tempColor = color1;
		color1 = color2;
		color2 = tempColor;

		float tempDepth = depth1;
		depth1 = depth2;
		depth2 = tempDepth;
	}

	for (int x = x1; x <= x2; x++)
	{
		float t = (float)(x - x1) / (x2 - x1);

		FragmentInput fragment;
		fragment.color = color1.Lerp(color2, t);
		fragment.screenX = x;
		fragment.screenY = y;
		fragment.depth = depth1 + t * (depth2 - depth1);

		target->Write(x, y, shader(fragment));
	}
}

void Rasterizer::DrawVerticalLine(int x, int y1, int y2, float depth1, float depth2, Color4f color1, Color4f color2, FragmentShader shader)
{
	if (y1 > y2)
	{
		int temp = y1;
		y1 = y2;
		y2 = temp;

		Color4f tempColor = color1;
		color1 = color2;
		color2 = tempColor;

		float tempDepth = depth1;
		depth1 = depth2;
		depth2 = tempDepth;
	}

	for (int y = y1; y <= y2; y++)
	{
		float t = (float)(y - y1) / (y2 - y1);

		FragmentInput fragment;
		fragment.color = color1.Lerp(color2, t);
		fragment.screenX = x;
		fragment.screenY = y;
		fragment.depth = depth1 + t * (depth2 - depth1);

		target->Write(x, y, shader(fragment));
	}
}

void Rasterizer::DrawLowLine(int x1, int y1, int x2, int y2, float depth1, float depth2, Color4f color1, Color4f color2, FragmentShader shader)
{
	if (x2 < x1)
	{
		int temp = x1;
		x1 = x2;
		x2 = temp;

		temp = y1;
		y1 = y2;
		y2 = temp;

		Color4f tempColor = color1;
		color1 = color2;
		color2 = tempColor;

		float tempDepth = depth1;
		depth1 = depth2;
		depth2 = tempDepth;
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

		float t = (float)(x - x1) / (x2 - x1);

		FragmentInput fragment;
		fragment.color = color1.Lerp(color2, t);
		fragment.screenX = x;
		fragment.screenY = yActual;
		fragment.depth = depth1 + t * (depth2 - depth1);

		target->Write(x, yActual, shader(fragment));
		ydxIdeal += dy;
	}
}

void Rasterizer::DrawHighLine(int x1, int y1, int x2, int y2, float depth1, float depth2, Color4f color1, Color4f color2, FragmentShader shader)
{
	if (y2 < y1)
	{
		int temp = x1;
		x1 = x2;
		x2 = temp;
	
		temp = y1;
		y1 = y2;
		y2 = temp;

		Color4f tempColor = color1;
		color1 = color2;
		color2 = tempColor;

		float tempDepth = depth1;
		depth1 = depth2;
		depth2 = tempDepth;
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
	
		float t = (float)(y - y1) / (y2 - y1);

		FragmentInput fragment;
		fragment.color = color1.Lerp(color2, t);
		fragment.screenX = xActual;
		fragment.screenY = y;
		fragment.depth = depth1 + t * (depth2 - depth1);

		target->Write(xActual, y, shader(fragment));

		xdyIdeal += dx;
	}
}

void Rasterizer::DrawLine(const Vertex& p1, const Vertex& p2, FragmentShader shader)
{
	if (target == nullptr)
		return;

	int x1 = (int)p1.position.x;
	int y1 = (int)p1.position.y;
	int x2 = (int)p2.position.x;
	int y2 = (int)p2.position.y;

	const Color4f& color1 = p1.color;
	const Color4f& color2 = p2.color;

	if (x1 == x2)
		DrawVerticalLine(x1, y1, y2, p1.position.z, p2.position.z, color1, color2, shader);
	else if (y1 == y2)
		DrawHorizontalLine(x1, x2, y1, p1.position.z, p2.position.z, color1, color2, shader);
	else if (abs(y2 - y1) <= abs(x2 - x1))
		DrawLowLine(x1, y1, x2, y2, p1.position.z, p2.position.z, color1, color2, shader);
	else
		DrawHighLine(x1, y1, x2, y2, p1.position.z, p2.position.z, color1, color2, shader);
}

void Rasterizer::DrawFlatBottomTriangle(int x1, int x2, int bottom, const Vertex& pTop, float depth1, float depth2, Color4f color1, Color4f color2, FragmentShader shader)
{
	for (int y = bottom; y <= pTop.position.y; y++)
	{
		float t = (y - bottom) / (pTop.position.y - bottom);
		int xStart = (int)(x1 + t * (pTop.position.x - x1));
		int xEnd = (int)(x2 + t * (pTop.position.x - x2));

		Color4f c1 = color1.Lerp(pTop.color, t);
		Color4f c2 = color2.Lerp(pTop.color, t);

		float d1 = depth1 + t * (pTop.position.z - depth1);
		float d2 = depth2 + t * (pTop.position.z - depth2);

		DrawHorizontalLine(xStart, xEnd, y, d1, d2, c1, c2, shader);
	}
}

void Rasterizer::DrawFlatTopTriangle(int x1, int x2, int top, const Vertex& pBottom, float depth1, float depth2, Color4f color1, Color4f color2, FragmentShader shader)
{
	for (int y = (int)pBottom.position.y; y <= top; y++)
	{
		float t = (y - pBottom.position.y) / (top - pBottom.position.y);
		int xStart = (int)(pBottom.position.x + t * (x1 - pBottom.position.x));
		int xEnd = (int)(pBottom.position.x + t * (x2 - pBottom.position.x));

		Color4f c1 = pBottom.color.Lerp(color1, t);
		Color4f c2 = pBottom.color.Lerp(color2, t);

		float d1 = pBottom.position.z + t * (depth1 - pBottom.position.z);
		float d2 = pBottom.position.z + t * (depth2 - pBottom.position.z);

		DrawHorizontalLine(xStart, xEnd, y, d1, d2, c1, c2, shader);
	}
}

void Rasterizer::DrawTriangle(Vertex p1, Vertex p2, Vertex p3, FragmentShader shader)
{
	Vertex temp;
	if (p3.position.y < p1.position.y)
	{
		temp = p1;
		p1 = p3;
		p3 = temp;
	}

	if (p2.position.y > p3.position.y)
	{
		temp = p3;
		p3 = p2;
		p2 = temp;
	}

	if (p2.position.y < p1.position.y)
	{
		temp = p1;
		p1 = p2;
		p2 = temp;
	}

	// TODO: Maybe draw a line?
	if (p3.position.y == p1.position.y)
		return;

	float t = (p2.position.y - p1.position.y) / (p3.position.y - p1.position.y);

	int x1 = (int)p2.position.x;
	int x2 = (int)(p1.position.x + (p3.position.x - p1.position.x) * t);
	
	float depth1 = p2.position.z;
	float depth2 = p1.position.z + t * (p3.position.z - p1.position.z);

	Color4f color1 = p2.color;
	Color4f color2 = p1.color.Lerp(p3.color, t);

	if (x2 < x1)
	{
		int tempX = x1;
		x1 = x2;
		x2 = tempX;

		float tempZ = depth1;
		depth1 = depth2;
		depth2 = tempZ;

		Color4f tempColor = color1;
		color1 = color2;
		color2 = tempColor;
	}

	// TODO: Calculate colors and depths
	DrawFlatBottomTriangle(x1, x2, (int)p2.position.y, p3, depth1, depth2, color1, color2, shader);
	DrawFlatTopTriangle(x1, x2, (int)p2.position.y, p1, depth1, depth2, color1, color2, shader);
}