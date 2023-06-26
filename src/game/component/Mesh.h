#pragma once
#include <string>
#include <vector>
#include <GL/gl.h>
#include "common/Vec3.h"
#include "common/Mat44.h"
#include "game/component/Component.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "render/Renderer.h"
class Mesh:public Component{
public:
	PrimitiveType primitive_type;
	std::vector<Vec3> vertices, normals;
	std::vector<std::vector<uint>> faces;
	Mesh(): Component(), primitive_type(PrimitiveType::points){}
	Mesh(const std::string& file_path, Entity* owner=nullptr);
};