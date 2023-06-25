#pragma once
#include <vector>
#include "common/Mat44.h"
#include "common/Vec3.h"

enum class PrimitiveType{
	points,
	lines,
	triangles,
};

struct RenderRequest{
	Mat44 to_world;
	PrimitiveType primitive_type;
	std::vector<Vec3> vertices;
	std::vector<std::vector<uint>> faces;
};

class Renderer{
	virtual void render(const std::vector<RenderRequest>& render_q)=0;
};