#pragma once
#include <string>
#include <vector>
#include <memory>
#include <list>
#include <GL/gl.h>
#include "common/Vec3.h"
#include "common/Mat44.h"
#include "common/Quaternion.h"
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
public:
	Mat44 offset;
	std::vector<std::pair<float,Vec3>> positions;
	std::vector<std::pair<float,Quaternion>> rotations;
	std::vector<std::pair<float,Vec3>> scales;
	Bone(){
		// throw "It should be implemented!!!";
	}
	Bone(const std::string& name){

	}
};
struct BoneInfluence{
	float weight=0;
	Bone* bone=nullptr;
};
const int MAX_BONE_INFLUENCE=8;

class Mesh:public Component{
public:
	PrimitiveType primitive_type;
	std::vector<Vec3> vertices, normals, texcoord;
	std::vector<std::vector<uint>> faces;
	std::shared_ptr<Material> material;
	std::vector<Bone> bones;
	std::vector<std::array<BoneInfluence,MAX_BONE_INFLUENCE>> bone_influences;
	Mesh();
	~Mesh();
};