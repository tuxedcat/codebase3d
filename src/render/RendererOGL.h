#pragma once
#include "render/Renderer.h"

class RendererOGL: public Renderer{
public:
	RendererOGL();
	virtual void render(const std::vector<RenderRequest>& render_q) override;
	virtual void setProjection(float fovyInDegrees, float aspectRatio, float znear, float zfar) override;
	virtual void setViewport(int width, int height) override;
};