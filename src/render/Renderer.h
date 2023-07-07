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
	std::vector<Vec3> vertices, normals, texcoord;
	std::vector<std::vector<uint>> faces;
	unsigned int textureID; //if not exist: -1
};

class Renderer{
public:
	virtual void render(const std::vector<RenderRequest>& render_q) = 0;
	virtual void setProjection(float fovyInDegrees, float aspectRatio, float znear, float zfar) = 0;
	virtual void setViewport(int width, int height) = 0;
};