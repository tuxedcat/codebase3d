#pragma once
#include <string>
#include <vector>
#include <GL/gl.h>
#include "common/Vec3.h"
#include "common/Mat44.h"
#include "game/Component.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
class Mesh:public Component{
public:
	unsigned int primitive_type;
	std::vector<Vec3> vertices;
	Mesh(): Component(), primitive_type(GL_POINTS){}
	Mesh(const std::string& file_path, Entity* owner=nullptr);
	void draw(Mat44 parent_space, std::vector<std::pair<Mat44,std::vector<Vec3>>>& render_q)const;
};