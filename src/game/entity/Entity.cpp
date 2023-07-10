#include "game/entity/Entity.h"
#include "game/component/Mesh.h"
#include "render/Renderer.h"
#include <iostream>
using namespace std;

Entity* Entity::loadFromFileImpl(const string& model_directory, aiNode* node, aiMesh** meshes, aiMaterial** materials){
	if(!node)
		return nullptr;

	auto ret = new Entity;	
	ret->name = node->mName.C_Str();
	auto mtx = node->mTransformation;
	ret->position({mtx.a4,mtx.b4,mtx.c4});
	mtx.a4=mtx.b4=mtx.c4=0;
	// ret->scale({
	// 	sqrtf(mtx.a1*mtx.a1 + mtx.b1*mtx.b1 + mtx.c1*mtx.c1),
	// 	sqrtf(mtx.a2*mtx.a2 + mtx.b2*mtx.b2 + mtx.c2*mtx.c2),
	// 	sqrtf(mtx.a3*mtx.a3 + mtx.b3*mtx.b3 + mtx.c3*mtx.c3)});
	ret->scale({
		sqrtf(mtx.a1*mtx.a1 + mtx.a2*mtx.a2 + mtx.a3*mtx.a3),
		sqrtf(mtx.b1*mtx.b1 + mtx.b2*mtx.b2 + mtx.b3*mtx.b3),
		sqrtf(mtx.c1*mtx.c1 + mtx.c2*mtx.c2 + mtx.c3*mtx.c3)});
	cout<<ret->name<<' '<<ret->position()<<' '<<ret->scale()<<endl;

	for(int i=0;i<node->mNumChildren;i++){
		// auto root=ret;
		// while(ret->parent())
		// 	ret=ret->parent();
		ret->adopt(loadFromFileImpl(model_directory, node->mChildren[i], meshes, materials));
	}
	// mtx.a1/=ret->scale().x;
	// mtx.b1/=ret->scale().x;
	// mtx.c1/=ret->scale().x;
	// mtx.a2/=ret->scale().y;
	// mtx.b2/=ret->scale().y;
	// mtx.c2/=ret->scale().y;
	// mtx.a3/=ret->scale().z;
	// mtx.b3/=ret->scale().z;
	// mtx.c3/=ret->scale().z;
	// ret->setRotate(Quaternion({
	// 	mtx.a1,mtx.a2,mtx.a3,mtx.a4,
	// 	mtx.b1,mtx.b2,mtx.b3,mtx.b4,
	// 	mtx.c1,mtx.c2,mtx.c3,mtx.c4,
	// 	mtx.d1,mtx.d2,mtx.d3,mtx.d4}));

	for(int idx_node=0;idx_node<node->mNumMeshes;idx_node++){
		auto& retmesh = ret->meshes.emplace_back(new Mesh(ret));
		auto mesh = meshes[node->mMeshes[idx_node]];
		assert(mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE);

		// copy shape informations
		retmesh->primitive_type = PrimitiveType::triangles;
		retmesh->vertices.resize(mesh->mNumVertices);
		retmesh->normals.resize(mesh->mNumVertices);
		retmesh->texcoord.resize(mesh->mNumVertices);
		for(int idx_vtx=0;idx_vtx<retmesh->vertices.size();idx_vtx++){
			assert(mesh->HasNormals() and mesh->mTextureCoords[0]);
			retmesh->vertices[idx_vtx]={
				mesh->mVertices[idx_vtx].x,
				mesh->mVertices[idx_vtx].y,
				mesh->mVertices[idx_vtx].z};
			retmesh->normals[idx_vtx]={
				mesh->mNormals[idx_vtx].x,
				mesh->mNormals[idx_vtx].y,
				mesh->mNormals[idx_vtx].z};
			retmesh->texcoord[idx_vtx]={
				mesh->mTextureCoords[0][idx_vtx].x,
				mesh->mTextureCoords[0][idx_vtx].y,
				mesh->mTextureCoords[0][idx_vtx].z};
		}
		retmesh->faces.resize(mesh->mNumFaces);
		for(int idx_face=0;idx_face<retmesh->faces.size();idx_face++){
			retmesh->faces[idx_face].assign(
				mesh->mFaces[idx_face].mIndices,
				mesh->mFaces[idx_face].mIndices+mesh->mFaces[idx_face].mNumIndices);
		}

		// copy material informations
		auto material = materials[mesh->mMaterialIndex];
		int diffuse_count = material->GetTextureCount(aiTextureType_DIFFUSE);
		if(diffuse_count > 0){
			aiString assimp_textureName;
			material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), assimp_textureName);
			retmesh->material = make_shared<Material>(model_directory+"/"+assimp_textureName.data);
		}
	
		// copy bone informations
		for(int bone_idx; bone_idx<mesh->mNumBones; bone_idx++){
		}
	};
	return ret;
}

Entity* Entity::loadFromFile(const std::string& model_path){
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(model_path,
		aiProcess_CalcTangentSpace
		|aiProcess_Triangulate
		|aiProcess_GenSmoothNormals
		|aiProcess_JoinIdenticalVertices
		// |aiProcess_SortByPType
		|aiProcess_FlipUVs
		//aiProcess_SplitLargeMeshes, aiProcess_OptimizeMeshes 
	);
	if(scene==nullptr)
		throw std::string(importer.GetErrorString());
	if(!scene->HasMeshes())
		throw "No meshes in the file.";

	auto model_directory = model_path.substr(0,model_path.find_last_of('/'));
	return loadFromFileImpl(model_directory, scene->mRootNode, scene->mMeshes, scene->mMaterials);
}

Entity::~Entity(){
	for(auto child: children)
		delete child;
}

void Entity::draw(Mat44 parent2world, std::vector<RenderRequest>& render_q)const{
	for(auto i: children)
		i->draw(parent2world*local2parent(), render_q);
	for(auto mesh:meshes){
		render_q.emplace_back(
			parent2world*local2parent(),
			mesh->primitive_type,
			mesh->vertices,
			mesh->normals,
			mesh->texcoord,
			mesh->faces,
			mesh->material->getTextureID());
	}
}

void Entity::update(float delta_time){
	if(onUpdate)
		onUpdate(this, delta_time);
	for(auto i:children)
		i->update(delta_time);
}