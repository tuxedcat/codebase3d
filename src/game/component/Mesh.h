#pragma once
#include <string>
#include <vector>
#include <memory>
#include <list>
#include <GL/gl.h>
#include "common/Vec3.h"
#include "common/Mat44.h"
#include "game/component/Component.h"
#include "render/Renderer.h"

class Material{
public:
	static std::shared_ptr<Material> null(){
		static std::shared_ptr<Material> ret(new Material);
		return ret;
	}
	Material():textureID(-1){}
	Material(const std::string& texture_path);
	~Material();
	GLuint getTextureID()const{ return textureID; };
private:
	GLuint textureID;
};

class Bone{
	
};

class Mesh:public Component{
public:
	PrimitiveType primitive_type;
	std::vector<Vec3> vertices, normals, texcoord;
	std::vector<std::list<std::shared_ptr<Bone>>> bones_per_vertex;
	std::vector<std::vector<uint>> faces;
	std::shared_ptr<Material> material;
	Mesh();
	~Mesh();
};