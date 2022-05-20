#include "VertexPostProcessor.h"
#include <math.h>

float VertexPostProcessor::TestComponent(float component, float w, float directionComponent, float directionW)
{
	if (component > w)
		return (component - w) / (directionW - directionComponent);

	if (component < -w)
		return -(component + w) / (directionW + directionComponent);

	return 0;
}

void VertexPostProcessor::ClipVertexInLine(Vertex& vertex, const Vertex& other)
{
	Vector4f direction = other.position - vertex.position;
	float t = max(0, TestComponent(vertex.position.x, vertex.position.w, direction.x, direction.w));
	t = max(t, TestComponent(vertex.position.y, vertex.position.w, direction.y, direction.w));
	t = max(t, TestComponent(vertex.position.z, vertex.position.w, direction.z, direction.w));

	vertex.position += t * direction;
	vertex.color = vertex.color.Lerp(other.color, t);
}

void VertexPostProcessor::ClipLine(Line& line)
{
	ClipVertexInLine(line.a, line.b);
	ClipVertexInLine(line.b, line.a);
}

void VertexPostProcessor::PerspectiveDivide(Vector4f& point)
{
	point /= point.w;
}

void VertexPostProcessor::NormalizedToViewport(Vector4f& position)
{
	position.x = (position.x + 1) * viewport.width / 2;
	position.y = (position.y + 1) * viewport.height / 2;
	position.z =
		(viewport.maxDepth - viewport.minDepth) * position.z / 2
		+ (viewport.maxDepth + viewport.minDepth) / 2;
}

bool VertexPostProcessor::ProcessLine(Line& line)
{
	ClipLine(line);
	
	if (line.a.position.w == 0 || line.b.position.w == 0)
		return false;

	PerspectiveDivide(line.a.position);
	PerspectiveDivide(line.b.position);

	NormalizedToViewport(line.a.position);
	NormalizedToViewport(line.b.position);

	return true;
}