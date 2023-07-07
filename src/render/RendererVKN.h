#pragma once
#include "render/Renderer.h"

class RendererVKN: public Renderer{
	virtual void render(const std::vector<RenderRequest>& render_q) override { throw "Unimplemented"; };
	virtual void setProjection(float fovyInDegrees, float aspectRatio, float znear, float zfar) override { throw "Unimplemented"; };
	virtual void setViewport(int width, int height) override { throw "Unimplemented"; };
};