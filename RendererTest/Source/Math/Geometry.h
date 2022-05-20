#pragma once
#include "Vector.h"
#include "Color.h"

struct Vertex
{
	Vector4f position;

	// TODO: Make this a vertex attribute, and allow more freedom with it.
	Color4f color;
};

struct Line
{
	Vertex a, b;

	Line() : a(), b() { }
	Line(const Vertex& a, const Vertex& b) : a(a), b(b) { }
};