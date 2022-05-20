#pragma once
#include "VertexPostProcessor.h"
#include "Rasterizer.h"
#include <Objects/FrameBuffer.h>

typedef Vertex(*VertexShader)(const Vertex& in);

struct ShaderProgram
{
	VertexShader vertexShader;
	FragmentShader fragmentShader;

	ShaderProgram() {}

	ShaderProgram(VertexShader vertexShader, FragmentShader fragmentShader)
		: vertexShader(vertexShader), fragmentShader(fragmentShader)
	{
	}
};

class Pipeline
{
	VertexPostProcessor vertexPostProcessor;
	Rasterizer rasterizer;

public:
	inline void SetViewport(const Viewport& viewport) { vertexPostProcessor.viewport = viewport; }
	inline const Viewport& GetViewport() const { return vertexPostProcessor.viewport; }

	inline void SetTarget(FrameBuffer* target) { rasterizer.SetTarget(target); }

	void DrawLine(const Vertex& p1, const Vertex& p2, ShaderProgram shader);
};