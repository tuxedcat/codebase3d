#include "game/component/Mesh.h"
#include <iostream>
#include <bit>
#define STB_IMAGE_IMPLEMENTATION
#include "render/stb_image.h"
#include <string>
using namespace std;

Material::Material(const std::string& texture_path){
	int width, height, channels;
	auto path = string(texture_path);
	std::replace(path.begin(), path.end(), '\\', '/');
	unsigned char* image = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb);
	if(!image)
		throw stbi_failure_reason();
	if(width!=height)
		throw "Non square images are not supported.";
	if(popcount<unsigned>(width)!=1)
		throw "Non power of 2 sizes are not supported.";
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	stbi_image_free(image);
}
Material::~Material(){
	glDeleteTextures(1, &textureID);
}

Mesh::Mesh(Entity* owner): Component(owner), primitive_type(PrimitiveType::points), material(Material::null()){
}

Mesh::~Mesh(){
}