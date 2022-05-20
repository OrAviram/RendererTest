#pragma once

#include "Common.h"
#include "Math/Color.h"

struct BitmapData
{
	int width;
	int height;
	Color32* data;
	BITMAPINFO info;

	int bytesPerPixel = 4;
};

struct Fragment
{
	Color4f color;
	float depth;
};

enum class DepthTestFunction
{
	Always, Never,
	LessThan, LessThanOrEqual,
	GreaterThan, GreaterThanOrEqual,
	Equals, NotEquals
};

class FrameBuffer
{
	int width;
	int height;

	BitmapData colorBuffer;
	float* depthBuffer;
	
public:
	DepthTestFunction depthTestFunction = DepthTestFunction::Always;

public:
	inline int Width() const { return width; };
	inline int Height() const { return height; };

public:
	FrameBuffer(int width, int height);
	~FrameBuffer();

	void Resize(int width, int height);
	void Display(const WindowData& window);

	Color32 ReadColor(int x, int y);
	float ReadDepth(int x, int y);

private:
	// To add: Scissor test, stencil test, proper blending, logical operation and write mask.
	// Look into multisampling, occlusion query updating, sRGB conversion, dithering
	bool DepthTest(float fragmentDepth, float bufferValue);
	Color4f Blend(Color4f source, Color4f destination);

public:
	void Write(int x, int y, const Fragment& fragment);
	void ClearColor(Color32 color);
	void ClearDepth(float value = 0);
};