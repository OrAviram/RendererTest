#include <Windows.h>
#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(int width, int height)
	: width(width), height(height)
{
	colorBuffer.width = width;
	colorBuffer.height = height;
	colorBuffer.bytesPerPixel = 4;

	colorBuffer.info = {};
	colorBuffer.info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	colorBuffer.info.bmiHeader.biWidth = width;
	colorBuffer.info.bmiHeader.biHeight = height;
	colorBuffer.info.bmiHeader.biPlanes = 1;
	colorBuffer.info.bmiHeader.biBitCount = colorBuffer.bytesPerPixel * 8;
	colorBuffer.info.bmiHeader.biCompression = BI_RGB;
	colorBuffer.info.bmiHeader.biXPelsPerMeter = 1;
	colorBuffer.info.bmiHeader.biYPelsPerMeter = 1;

	colorBuffer.data = (Color32*)VirtualAlloc(NULL, width * height * colorBuffer.bytesPerPixel,
		MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (colorBuffer.data == NULL)
	{
		OutputDebugString("Failed to allocate color buffer.\n");
		PrintLastError();
	}

	depthBuffer = (float*)VirtualAlloc(NULL, width * height * sizeof(float),
		MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (depthBuffer == NULL)
	{
		OutputDebugString("Failed to allocate color buffer.\n");
		PrintLastError();
	}
}

FrameBuffer::~FrameBuffer()
{
	VirtualFree(colorBuffer.data, 0, MEM_RELEASE);
	VirtualFree(depthBuffer, 0, MEM_RELEASE);
}

void FrameBuffer::Resize(int width, int height)
{
	ASSERT(width > 0 && height > 0, "Invalid bitmap size: (%1!d!, %2!d!)", width, height);

	this->width = width;
	this->height = height;

	Color32* newColorData = (Color32*)VirtualAlloc(NULL, width * height * colorBuffer.bytesPerPixel,
		MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (newColorData == NULL )
	{
		OutputDebugString("Failed to reallocate color buffer.\n");
		PrintLastError();
	}
	else
	{
		colorBuffer.width = width;
		colorBuffer.height = height;

		colorBuffer.info.bmiHeader.biWidth = width;
		colorBuffer.info.bmiHeader.biHeight = height;

		VirtualFree(colorBuffer.data, 0, MEM_RELEASE);
		colorBuffer.data = newColorData;
	}

	float* newDepthData = (float*)VirtualAlloc(NULL, width * height * sizeof(float),
		MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (newDepthData == NULL)
	{
		OutputDebugString("Failed to reallocate depth buffer.\n");
		PrintLastError();
	}
	else
	{
		VirtualFree(depthBuffer, 0, MEM_RELEASE);
		depthBuffer = newDepthData;
	}
}

void FrameBuffer::Display(const WindowData& window)
{
	StretchDIBits(
		window.deviceContext,
		0, 0, window.width, window.height,
		0, 0, width, height,
		colorBuffer.data, &colorBuffer.info,
		DIB_RGB_COLORS, SRCCOPY);
}

Color32 FrameBuffer::ReadColor(int x, int y)
{
	//ASSERT(x >= 0 && x < bitmap->width && y >= 0 && y < bitmap->height,
	//	"Invalid coordinate in bitmap of size %1!d!x%2!d!: (%3!d!, %4!d!)",
	//	bitmap->width, bitmap->height, x, y);

	// TODO: Think whether this check should happen in release,
	// and if it's better to do the assert above instead.
	if (x >= 0 && x < width && y >= 0 && y < height)
		return colorBuffer.data[x + y * width];

	// TODO: Also consider what to do here
	return 0;
}

float FrameBuffer::ReadDepth(int x, int y)
{
	//ASSERT(x >= 0 && x < bitmap->width && y >= 0 && y < bitmap->height,
	//	"Invalid coordinate in bitmap of size %1!d!x%2!d!: (%3!d!, %4!d!)",
	//	bitmap->width, bitmap->height, x, y);

	// TODO: Think whether this check should happen in release,
	// and if it's better to do the assert above instead.
	if (x >= 0 && x < width && y >= 0 && y < height)
		return depthBuffer[x + y * width];

	// TODO: Again, think what to do here
	return -1;
}

bool FrameBuffer::DepthTest(float fragmentDepth, float bufferValue)
{
	switch (depthTestFunction)
	{
		case DepthTestFunction::Always:				return true;
		case DepthTestFunction::Never:				return false;
		case DepthTestFunction::LessThan:			return fragmentDepth < bufferValue;
		case DepthTestFunction::LessThanOrEqual:	return fragmentDepth <= bufferValue;
		case DepthTestFunction::GreaterThan:		return fragmentDepth > bufferValue;
		case DepthTestFunction::GreaterThanOrEqual: return fragmentDepth >= bufferValue;
		case DepthTestFunction::Equals:				return fragmentDepth == bufferValue;
		case DepthTestFunction::NotEquals:			return fragmentDepth != bufferValue;
	}
	ASSERT(false, "Illegal depth test function (%1!d!)", depthTestFunction);
	return false;
}

Color4f FrameBuffer::Blend(Color4f source, Color4f destination)
{
	return Color4f::Lerp(source, destination, 1 - source.alpha);
}

void FrameBuffer::Write(int x, int y, const Fragment& fragment)
{
	// TODO: Complete post-fragment processing.
	if (DepthTest(fragment.depth, ReadDepth(x, y)))
	{
		float depth = fragment.depth;
		Color4f color = Blend(fragment.color, ReadColor(x, y));

		// We'll add more rigurous testing later, don't worry...
		if (x >= 0 && x < width && y >= 0 && y < height)
		{
			int location = x + y * width;
			depthBuffer[location] = depth;
			colorBuffer.data[location] = (Color32)color;
		}
	}
}

void FrameBuffer::ClearColor(Color32 color)
{
	for (int i = 0; i < colorBuffer.width * colorBuffer.height; i++)
		colorBuffer.data[i] = color;
}

void FrameBuffer::ClearDepth(float value)
{
	for (int i = 0; i < colorBuffer.width * colorBuffer.height; i++)
		depthBuffer[i] = value;
}