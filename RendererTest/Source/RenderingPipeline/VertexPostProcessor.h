#pragma once
#include <Math/Geometry.h>

struct Viewport
{
	int x;
	int y;
	int width;
	int height;
	float minDepth;
	float maxDepth;

	Viewport()
		: x(0), y(0), width(100), height(100), minDepth(0), maxDepth(1)
	{
	}

	Viewport(int x, int y, int width, int height, float minDepth = 0, float maxDepth = 1)
		: x(x), y(y), width(width), height(height), minDepth(minDepth), maxDepth(maxDepth)
	{
	}
};

class VertexPostProcessor
{
public:
	Viewport viewport;

private:
	float TestComponent(float component, float w, float directionComponent, float directionW);
	void ClipVertexInLine(Vertex& vertex, const Vertex& other);

public:
	void ClipLine(Line& line);

	void PerspectiveDivide(Vector4f& point);
	void NormalizedToViewport(Vector4f& position);

	bool ProcessLine(Line& line);
};