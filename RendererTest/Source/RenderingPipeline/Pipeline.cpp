#include "Pipeline.h"

void Pipeline::DrawLine(const Vertex& p1, const Vertex& p2, ShaderProgram shader)
{
	Line l = { shader.vertexShader(p1), shader.vertexShader(p2) };
	if (vertexPostProcessor.ProcessLine(l))
		rasterizer.DrawLine(l.a, l.b, shader.fragmentShader);
}