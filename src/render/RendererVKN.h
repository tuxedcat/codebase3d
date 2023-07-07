#pragma once
#include "render/Renderer.h"

class RendererVKN: public Renderer{
	virtual void render(const std::vector<RenderRequest>& render_q){ throw "Unimplemented"; };
};