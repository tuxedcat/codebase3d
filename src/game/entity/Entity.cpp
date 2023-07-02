#include "game/entity/Entity.h"
#include "game/component/Mesh.h"
#include "render/Renderer.h"
#include <iostream>
using namespace std;

Entity* Entity::loadFromFileImpl(aiNode* node, aiMesh** meshes, aiMaterial** materials){
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
	// if(ret->name=="Armature")
	// 	ret->scale({1,1,1});
	cout<<ret->name<<' '<<ret->position()<<' '<<ret->scale()<<endl;

	for(int i=0;i<node->mNumChildren;i++)
		ret->adopt(loadFromFileImpl(node->mChildren[i],meshes,materials));
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

	ret->mesh = new Mesh(ret);
	for(int i=0;i<node->mNumMeshes;i++){
		auto mesh = meshes[node->mMeshes[i]];
		if(mesh->mPrimitiveTypes & aiPrimitiveType_POINT)
			ret->mesh->primitive_type = PrimitiveType::points;
		else if(mesh->mPrimitiveTypes & aiPrimitiveType_LINE)
			ret->mesh->primitive_type = PrimitiveType::lines;
		else if(mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE)
			ret->mesh->primitive_type = PrimitiveType::triangles;
		else
			throw "Unimplemented";

		ret->mesh->vertices.resize(mesh->mNumVertices);
		ret->mesh->normals.resize(mesh->mNumVertices);
		ret->mesh->texcoord.resize(mesh->mNumVertices);
		for(int i=0;i<ret->mesh->vertices.size();i++){
			ret->mesh->vertices[i]={
				mesh->mVertices[i].x,
				mesh->mVertices[i].y,
				mesh->mVertices[i].z};
			ret->mesh->normals[i]={
				mesh->mNormals[i].x,
				mesh->mNormals[i].y,
				mesh->mNormals[i].z};
			ret->mesh->texcoord[i]={
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y,
				mesh->mTextureCoords[0][i].z};
		}
		
		ret->mesh->faces.resize(mesh->mNumFaces);
		for(int i=0;i<ret->mesh->faces.size();i++){
			ret->mesh->faces[i].assign(
				mesh->mFaces[i].mIndices,
				mesh->mFaces[i].mIndices+mesh->mFaces[i].mNumIndices);
		}
		auto material = materials[mesh->mMaterialIndex];
		int diffuse_count = material->GetTextureCount(aiTextureType_DIFFUSE);
		if(diffuse_count > 0){
			aiString assimp_textureName;
			material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), assimp_textureName);
			ret->mesh->material = new Material(assimp_textureName.data);
		}
	};
	return ret;
}

Entity* Entity::loadFromFile(const std::string& model_path){
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(model_path,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);
	if(scene==nullptr)
		throw std::string(importer.GetErrorString());
	if(!scene->HasMeshes())
		throw "No meshes in the file.";

	return loadFromFileImpl(scene->mRootNode, scene->mMeshes, scene->mMaterials);
}

Entity::~Entity(){
	for(auto child: children)
		delete child;
	if(mesh)
		delete mesh;
	mesh=nullptr;
}

void Entity::draw(Mat44 parent2world, std::vector<RenderRequest>& render_q)const{
	for(auto i: children)
		i->draw(parent2world*local2parent(), render_q);
	if(mesh and mesh->material)
		render_q.emplace_back(
			parent2world*local2parent(),
			mesh->primitive_type,
			mesh->vertices,
			mesh->normals,
			mesh->texcoord,
			mesh->faces,
			mesh->material->getTextureID());
}

void Entity::update(float delta_time){
	if(onUpdate)
		onUpdate(this, delta_time);
	for(auto i:children)
		i->update(delta_time);
}