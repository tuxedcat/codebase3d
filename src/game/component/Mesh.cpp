#include "game/component/Mesh.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "render/stb_image.h"
#include <string>
#include <unistd.h>
using namespace std;

Material::Material(const std::string& texture_path){
	int width, height, channels;
	auto path = string("models\\"+texture_path);
	std::replace(path.begin(), path.end(), '\\', '/');
	unsigned char* image = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb);
	if(!image)
		throw stbi_failure_reason();
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

Mesh::Mesh(const std::string& file_path, Entity* owner): Component(owner){
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(file_path,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);
	if(scene==nullptr){
		throw std::string(importer.GetErrorString());
	}
	if(scene->HasMeshes()){
		auto assimp_mesh = scene->mMeshes[0];
		
		if(assimp_mesh->mPrimitiveTypes & aiPrimitiveType_POINT)
			primitive_type = PrimitiveType::points;
		else if(assimp_mesh->mPrimitiveTypes & aiPrimitiveType_LINE)
			primitive_type = PrimitiveType::lines;
		else if(assimp_mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE)
			primitive_type = PrimitiveType::triangles;
		else
			throw "Unimplemented";

		vertices.resize(assimp_mesh->mNumVertices);
		normals.resize(assimp_mesh->mNumVertices);
		texcoord.resize(assimp_mesh->mNumVertices);
		for(int i=0;i<vertices.size();i++){
			vertices[i]={
				assimp_mesh->mVertices[i].x,
				assimp_mesh->mVertices[i].y,
				assimp_mesh->mVertices[i].z};
			normals[i]={
				assimp_mesh->mNormals[i].x,
				assimp_mesh->mNormals[i].y,
				assimp_mesh->mNormals[i].z};
			texcoord[i]={
				assimp_mesh->mTextureCoords[0][i].x,
				assimp_mesh->mTextureCoords[0][i].y,
				assimp_mesh->mTextureCoords[0][i].z};
		}
		
		faces.resize(assimp_mesh->mNumFaces);
		for(int i=0;i<faces.size();i++){
			faces[i].assign(
				assimp_mesh->mFaces[i].mIndices,
				assimp_mesh->mFaces[i].mIndices+assimp_mesh->mFaces[i].mNumIndices);
		}
		if(scene->HasMaterials()){
			auto assimp_material = scene->mMaterials[assimp_mesh->mMaterialIndex];
			int diffuse_count = assimp_material->GetTextureCount(aiTextureType_DIFFUSE);
			if(diffuse_count > 0){
				aiString assimp_textureName;
				assimp_material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), assimp_textureName);
				material = new Material(assimp_textureName.data);
			}
		}
	}
}

Mesh::~Mesh(){
	if(material)
		delete material;
}