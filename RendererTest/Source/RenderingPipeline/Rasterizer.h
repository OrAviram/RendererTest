#pragma once
// TODO: Maybe move Fragment so that it's not here
#include <Objects/FrameBuffer.h>
#include <Math/Geometry.h>

struct FragmentInput
{
	int screenX, screenY;
	float depth;

	// TODO: Make this a vertex attribute, and allow more freedom with it.
	Color4f color;
};

typedef Fragment(*FragmentShader)(const FragmentInput& input);

class Rasterizer
{
	// TODO: Allow for multiple? Maybe make these fucntions not on a class?
	FrameBuffer* target;

public:
	Rasterizer();
	Rasterizer(FrameBuffer* target);
	inline void SetTarget(FrameBuffer* target) { this->target = target; }

private:
	void DrawHorizontalLine(int x1, int x2, int y, float depth1, float depth2, Color4f color1, Color4f color2, FragmentShader shader);
	void DrawVerticalLine(int x, int y1, int y2, float depth1, float depth2, Color4f color1, Color4f color2, FragmentShader shader);
	void DrawLowLine(int x1, int y1, int x2, int y2, float depth1, float depth2, Color4f color1, Color4f color2, FragmentShader shader);
	void DrawHighLine(int x1, int y1, int x2, int y2, float depth1, float depth2, Color4f color1, Color4f color2, FragmentShader shader);

public:
	void DrawLine(const Vertex& p1, const Vertex& p2, FragmentShader shader);

private:
	void DrawFlatBottomTriangle(int x1, int x2, int bottom, const Vertex& pTop, float depth1, float depth2, Color4f color1, Color4f color2, FragmentShader shader);
	void DrawFlatTopTriangle(int x1, int x2, int top, const Vertex& pBottom, float depth1, float depth2, Color4f color1, Color4f color2, FragmentShader shader);

public:
	void DrawTriangle(Vertex p1, Vertex p2, Vertex p3, FragmentShader shader);
};