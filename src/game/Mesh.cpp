#include "Mesh.h"

Mesh::Mesh(const std::string& file_path, Entity* owner): Component(owner){
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(file_path,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);
	if(scene==nullptr){
		throw importer.GetErrorString();
	}
	if(scene->HasMeshes()){
		auto assimp_mesh = scene->mMeshes[0];
		primitive_type = assimp_mesh->mPrimitiveTypes;
		vertices.resize(assimp_mesh->mNumVertices);
		for(int i=0;i<assimp_mesh->mNumVertices;i++){
			vertices[i]={
				assimp_mesh->mVertices[i].x,
				assimp_mesh->mVertices[i].y,
				assimp_mesh->mVertices[i].z};
		}
	}
}

void Mesh::draw(Mat44 parent_space, std::vector<std::pair<Mat44,std::vector<Vec3>>>& render_q)const{
	//TODO:
	//render_q.emplace_back(local_space, vertices)
}