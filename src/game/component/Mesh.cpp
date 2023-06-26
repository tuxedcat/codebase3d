#include "game/component/Mesh.h"

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
		
		if(assimp_mesh->mPrimitiveTypes & aiPrimitiveType_POINT)
			primitive_type = PrimitiveType::points;
		else if(assimp_mesh->mPrimitiveTypes & aiPrimitiveType_LINE)
			primitive_type = PrimitiveType::lines;
		else if(assimp_mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE)
			primitive_type = PrimitiveType::triangles;
		else
			throw "Unimplemented";

		vertices.resize(assimp_mesh->mNumVertices);
		for(int i=0;i<vertices.size();i++){
			vertices[i]={
				assimp_mesh->mVertices[i].x,
				assimp_mesh->mVertices[i].y,
				assimp_mesh->mVertices[i].z};
		}
		
		faces.resize(assimp_mesh->mNumFaces);
		for(int i=0;i<faces.size();i++){
			faces[i].assign(
				assimp_mesh->mFaces[i].mIndices,
				assimp_mesh->mFaces[i].mIndices+assimp_mesh->mFaces[i].mNumIndices);
		}
	}
}