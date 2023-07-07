#pragma once
#include "render/Renderer.h"

class RendererOGL: public Renderer{
public:
	virtual void render(const std::vector<RenderRequest>& render_q) override;
};