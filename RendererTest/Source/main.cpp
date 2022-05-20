#include <Windows.h>
#include <stdarg.h>
#include <math.h>
#include "Common.h"
#include "Renderer.h"
#include <RenderingPipeline/Pipeline.h>

WindowData window;
FrameBuffer* framebuffer;
Renderer* renderer;

constexpr int MIN_WINDOW_WIDTH = 50;
constexpr int MIN_WINDOW_HEIGHT = 50;

struct Matrix4x4
{
	Vector4f columns[4];
};

Matrix4x4 Identity()
{
	Matrix4x4 result;
	result.columns[0] = Vector4f(1, 0, 0, 0);
	result.columns[1] = Vector4f(0, 1, 0, 0);
	result.columns[2] = Vector4f(0, 0, 1, 0);
	result.columns[3] = Vector4f(0, 0, 0, 1);
	return result;
}

Matrix4x4 Translate(float x, float y, float z)
{
	Matrix4x4 result;
	result.columns[0] = Vector4f(1, 0, 0, 0);
	result.columns[1] = Vector4f(0, 1, 0, 0);
	result.columns[2] = Vector4f(0, 0, 1, 0);
	result.columns[3] = Vector4f(x, y, z, 1);
	return result;
}

Matrix4x4 RotationZ(float radians)
{
	float c = (float)cos(radians);
	float s = (float)sin(radians);

	Matrix4x4 result;
	result.columns[0] = Vector4f(c, s, 0, 0);
	result.columns[1] = Vector4f(-s, c, 0, 0);
	result.columns[2] = Vector4f(0, 0, 1, 0);
	result.columns[3] = Vector4f(0, 0, 0, 1);
	return result;
}

Matrix4x4 RotationY(float radians)
{
	float c = (float)cos(radians);
	float s = (float)sin(radians);

	Matrix4x4 result;
	result.columns[0] = Vector4f(c, 0, s, 0);
	result.columns[1] = Vector4f(0, 1, 0, 0);
	result.columns[2] = Vector4f(-s, 0, c, 0);
	result.columns[3] = Vector4f(0, 0, 0, 1);
	return result;
}

Vector4f operator*(const Matrix4x4& matrix, const Vector4f& vector)
{
	Vector4f result;
	for (size_t i = 0; i < 4; i++)
		result += vector.components[i] * matrix.columns[i];

	return result;
}

Matrix4x4 operator*(const Matrix4x4& a, const Matrix4x4& b)
{
	Matrix4x4 result;
	for (size_t i = 0; i < 4; i++)
		result.columns[i] = a * b.columns[i];

	return result;
}

Matrix4x4 transform;

Vertex VertexTest(const Vertex& in)
{
	Vertex out = in;
	out.position = transform * out.position;
	return out;
}

Vertex Vartax(const Vertex& in)
{
	Vertex out = in;
	out.position.x /= 2;
	out.position.y /= 2;
	return out;
}

Fragment FragmentTest(const FragmentInput& frag)
{
	Fragment f;
	f.color = frag.color;
	f.depth = frag.depth;
	return f;
}

Fragment Blacker(const FragmentInput& frag)
{
	Fragment f;
	f.color = Color4f::Black;
	f.depth = 1;
	return f;
}

void DrawTestLineCube(Pipeline& pipeline, ShaderProgram shaders, float angle, bool addTriangle)
{
	float f = 10;
	float n = 0.1f;

	transform.columns[0] = Vector4f(1, 0, 0, 0);
	transform.columns[1] = Vector4f(0, 1, 0, 0);
	transform.columns[2] = Vector4f(0, 0, -(f + n) / (f - n), 1);
	transform.columns[3] = Vector4f(0, 0, 2 * f * n / (f - n), 0);

	transform = transform * Translate(0, 0, 2);
	transform = transform * RotationY(angle) * RotationZ(angle);

	Vertex cube[] =
	{
		{ Vector4f(-0.5f, -0.5f, -0.5f, 1), Color4f(1, 0, 0, 1) },
		{ Vector4f(0.5f, -0.5f, -0.5f, 1), Color4f(0, 1, 0, 1) },
		{ Vector4f(-0.5f, 0.5f, -0.5f, 1), Color4f(0, 0, 1, 1) },
		{ Vector4f(0.5f, 0.5f, -0.5f, 1), Color4f::Zero },

		{ Vector4f(-0.5f, -0.5f, 0.5f, 1), Color4f::Zero },
		{ Vector4f(0.5f, -0.5f, 0.5f, 1), Color4f(0, 0, 1, 1) },
		{ Vector4f(-0.5f, 0.5f, 0.5f, 1), Color4f(0, 1, 0, 1) },
		{ Vector4f(0.5f, 0.5f, 0.5f, 1), Color4f(1, 0, 0, 1) },
	};

	// First face
	pipeline.DrawLine(cube[0], cube[1], shaders);
	pipeline.DrawLine(cube[1], cube[3], shaders);
	pipeline.DrawLine(cube[3], cube[2], shaders);
	pipeline.DrawLine(cube[2], cube[0], shaders);

	// Second face
	pipeline.DrawLine(cube[4], cube[5], shaders);
	pipeline.DrawLine(cube[5], cube[7], shaders);
	pipeline.DrawLine(cube[7], cube[6], shaders);
	pipeline.DrawLine(cube[6], cube[4], shaders);

	// Connections
	pipeline.DrawLine(cube[0], cube[4], shaders);
	pipeline.DrawLine(cube[1], cube[5], shaders);
	pipeline.DrawLine(cube[2], cube[6], shaders);
	pipeline.DrawLine(cube[3], cube[7], shaders);

	if (addTriangle)
	{
		transform = Identity();
		Vertex triangle[] =
		{
			{ Vector4f(-0.5f, -0.8f, -0.9f, 1), Color4f::Red },
			{ Vector4f(0.5f, -0.5f, -0.9f, 1), Color4f::Green },
			{ Vector4f(0, 0.5f, -0.8f, 1), Color4f::Blue },
		};

		pipeline.DrawLine(triangle[0], triangle[1], ShaderProgram(Vartax, Blacker));
		pipeline.DrawLine(triangle[1], triangle[2], ShaderProgram(Vartax, Blacker));
		pipeline.DrawLine(triangle[2], triangle[0], ShaderProgram(Vartax, Blacker));

		Rasterizer r;
		r.SetTarget(framebuffer);

		VertexPostProcessor vpp;
		vpp.viewport = pipeline.GetViewport();
		vpp.NormalizedToViewport(triangle[0].position);
		vpp.NormalizedToViewport(triangle[1].position);
		vpp.NormalizedToViewport(triangle[2].position);

		r.DrawTriangle(triangle[0], triangle[1], triangle[2], FragmentTest);
	}
}

void LinesTest(Pipeline& pipeline, ShaderProgram shaders)
{
	transform = Identity();

	// L1 - Normal
	Vertex a;
	a.position = Vector4f(0.5f, 0.75f, 0, 1);
	a.color = Color4f(1, 0, 0, 1);

	Vertex b;
	b.position = Vector4f(-0.25f, -0.5f, 0, 1);
	b.color = Color4f(0, 1, 0, 1);

	pipeline.DrawLine(a, b, shaders);

	// L2 - Normal flipped
	a.position = Vector4f(-0.5f, -0.75f, 0, 1);
	b.position = Vector4f(0.25f, 0.5f, 0, 1);
	pipeline.DrawLine(a, b, shaders);

	// L3 - Vertical
	a.position = Vector4f(0.5f, 0.75f, 0, 1);
	b.position = Vector4f(0.5f, -0.5f, 0, 1);
	pipeline.DrawLine(a, b, shaders);

	// L4 - Vertical flipped
	a.position = Vector4f(0.75f, -0.75f, 0, 1);
	b.position = Vector4f(0.75f, 0.5f, 0, 1);
	pipeline.DrawLine(a, b, shaders);

	// L5 - Horizontal
	a.position = Vector4f(-0.5f, -0.75f, 0, 1);
	b.position = Vector4f(0.75f, -0.75f, 0, 1);
	pipeline.DrawLine(a, b, shaders);

	// L6 - Horizontal flipped
	a.position = Vector4f(0.5f, -0.85f, 0, 1);
	b.position = Vector4f(-0.75f, -0.85f, 0, 1);
	pipeline.DrawLine(a, b, shaders);
}

float t = 0;
void Render()
{
	framebuffer->ClearColor(RGBA(0, 0, 0, 0));
	framebuffer->ClearDepth(0);
	t += 0.002f;

	framebuffer->depthTestFunction = DepthTestFunction::GreaterThanOrEqual;
	Pipeline pipeline;
	pipeline.SetViewport(Viewport(0, 0, framebuffer->Width(), framebuffer->Height()));
	pipeline.SetTarget(framebuffer);

	ShaderProgram shaders;
	shaders.fragmentShader = FragmentTest;
	shaders.vertexShader = VertexTest;

	DrawTestLineCube(pipeline, shaders, t, true);
	LinesTest(pipeline, shaders);
}

void UpdateScreen()
{
	framebuffer->Display(window);
}

LRESULT CALLBACK WindowProc(_In_ HWND windowHandle, _In_ UINT message, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		window.deviceContext = GetDC(windowHandle);
		framebuffer = new FrameBuffer(window.width, window.height);
		renderer = new Renderer(framebuffer);

		framebuffer->depthTestFunction = DepthTestFunction::Always;
		break;

	case WM_GETMINMAXINFO:
	{
		MINMAXINFO* minMaxInfo = (LPMINMAXINFO)lParam;
		minMaxInfo->ptMinTrackSize.x = MIN_WINDOW_WIDTH;
		minMaxInfo->ptMinTrackSize.y = MIN_WINDOW_HEIGHT;
		break;
	}

	case WM_SIZE:
		window.width = LOWORD(lParam);
		window.height = HIWORD(lParam);
		if (wParam != SIZE_MINIMIZED)
			framebuffer->Resize(window.width, window.height);

		break;

	case WM_PAINT:
		Render();
		UpdateScreen();
		break;

	case WM_DESTROY:
		ReleaseDC(windowHandle, window.deviceContext);
		
		delete renderer;
		delete framebuffer;

		renderer = nullptr;
		framebuffer = nullptr;

		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(windowHandle, message, wParam, lParam);
}

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE previousInstance, PSTR cmdLine, int cmdShow)
{
	WNDCLASS windowClass = {};
	windowClass.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = instance;
	windowClass.hIcon = NULL;
	windowClass.hCursor = NULL;
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = "FunWindowClass";
	if (!RegisterClass(&windowClass))
		PrintLastError();

	window.width = 1000;
	window.height = 800;
	HWND windowHandle = CreateWindow(
		"FunWindowClass", "FunWindow",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		window.width, window.height,
		NULL, NULL, instance, NULL);

	if (windowHandle == NULL)
		PrintLastError();

	SetStretchBltMode(window.deviceContext, STRETCH_DELETESCANS);
	bool running = true;
	while (running)
	{
		MSG message = {};
		while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);

			if (message.message == WM_QUIT)
				running = false;
		}

		if (renderer)
		{
			Render();
			UpdateScreen();
		}
	}

	return 0;
}