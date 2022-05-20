#pragma once
#include <Windows.h>

typedef unsigned int uint32;
typedef unsigned char byte;

struct WindowData
{
	int width;
	int height;
	HDC deviceContext;
};

static void Log(const char* message, ...)
{
	va_list arguments = NULL;
	va_start(arguments, message);

	char* formattedMessage;
	FormatMessage(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ALLOCATE_BUFFER, message, 0, 0, (LPSTR)&formattedMessage, 0, &arguments);
	OutputDebugString(formattedMessage);

	LocalFree(formattedMessage);
	va_end(arguments);
}

static void PrintLastError()
{
	char* message;
	DWORD messageLength = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL, GetLastError(), 0, (LPSTR)&message, 0, NULL);
	OutputDebugString(message);
	LocalFree(message);
}

#define LogLine(message, ...) Log(message "\n", __VA_ARGS__)

#if _DEBUG
	#define ASSERT(condition, message, ...) { if(!(condition)) { LogLine(message, __VA_ARGS__); __debugbreak(); } }
#else
	#define ASSERT(condition, message, ...)
#endif