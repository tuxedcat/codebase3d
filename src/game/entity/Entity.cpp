#include "game/entity/Entity.h"
#include "game/component/Mesh.h"
#include "render/Renderer.h"
#include <iostream>
using namespace std;

Entity* Entity::loadFromFileImpl(
	aiNode* node,
	const vector<shared_ptr<Mesh>>& meshes,
	map<aiNode*,Entity*>& mapping){
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
	// cout<<ret->name<<' '<<ret->position()<<' '<<ret->scale()<<endl;

	for(int i=0;i<node->mNumChildren;i++){
		// auto root=ret;
		// while(ret->parent())
		// 	ret=ret->parent();
		ret->adopt(loadFromFileImpl(node->mChildren[i], meshes, mapping));
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

	for(int idx_mesh=0;idx_mesh<node->mNumMeshes;idx_mesh++){
		ret->meshes.emplace_back(meshes[node->mMeshes[idx_mesh]]);
	};
	return mapping[node]=ret;
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
		|aiProcess_PopulateArmatureData
	);
	if(scene==nullptr)
		throw std::string(importer.GetErrorString());
	if(!scene->HasMeshes())
		throw "No meshes in the file.";

	vector<shared_ptr<Material>> materials;
	for(int idx_mat=0;idx_mat<scene->mNumMaterials;idx_mat++){
		auto material_src = scene->mMaterials[idx_mat];
		int diffuse_count = material_src->GetTextureCount(aiTextureType_DIFFUSE);
		if(diffuse_count > 0){
			aiString assimp_textureName;
			material_src->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), assimp_textureName);
			auto model_directory = model_path.substr(0,model_path.find_last_of('/'));
			materials.emplace_back(make_shared<Material>(model_directory+"/"+assimp_textureName.data));
		}else{
			materials.emplace_back(make_shared<Material>());
		}
	}

	vector<shared_ptr<Mesh>> meshes;
	for(int idx_mesh=0;idx_mesh<scene->mNumMeshes;idx_mesh++){
		auto& mesh = meshes.emplace_back(make_shared<Mesh>());
		auto& mesh_src = scene->mMeshes[idx_mesh];
		assert(mesh_src->mPrimitiveTypes & aiPrimitiveType_TRIANGLE);

		// copy shape informations
		mesh->primitive_type = PrimitiveType::triangles;
		mesh->vertices.resize(mesh_src->mNumVertices);
		mesh->normals.resize(mesh_src->mNumVertices);
		mesh->texcoord.resize(mesh_src->mNumVertices);
		mesh->bones_per_vertex.resize(mesh_src->mNumVertices);
		for(int idx_vtx=0;idx_vtx<mesh->vertices.size();idx_vtx++){
			assert(mesh_src->HasNormals() and mesh_src->mTextureCoords[0]);
			mesh->vertices[idx_vtx]={
				mesh_src->mVertices[idx_vtx].x,
				mesh_src->mVertices[idx_vtx].y,
				mesh_src->mVertices[idx_vtx].z};
			mesh->normals[idx_vtx]={
				mesh_src->mNormals[idx_vtx].x,
				mesh_src->mNormals[idx_vtx].y,
				mesh_src->mNormals[idx_vtx].z};
			mesh->texcoord[idx_vtx]={
				mesh_src->mTextureCoords[0][idx_vtx].x,
				mesh_src->mTextureCoords[0][idx_vtx].y,
				mesh_src->mTextureCoords[0][idx_vtx].z};
		}
		mesh->faces.resize(mesh_src->mNumFaces);
		for(int idx_face=0;idx_face<mesh->faces.size();idx_face++){
			mesh->faces[idx_face].assign(
				mesh_src->mFaces[idx_face].mIndices,
				mesh_src->mFaces[idx_face].mIndices+mesh_src->mFaces[idx_face].mNumIndices);
		}

		// copy material informations
		mesh->material = materials[mesh_src->mMaterialIndex];
	}

	map<aiNode*,Entity*> mapping;
	auto ret = loadFromFileImpl(scene->mRootNode, meshes, mapping);
	for(int idx_mesh=0;idx_mesh<scene->mNumMeshes;idx_mesh++){
		// copy bone informations
		auto& mesh_src = scene->mMeshes[idx_mesh];
		for(int bone_idx=0; bone_idx<mesh_src->mNumBones; bone_idx++){
			const auto& bone_src = mesh_src->mBones[bone_idx];
			for(int idx_weight=0;idx_weight<bone_src->mNumWeights;idx_weight++){
				const auto& weight_src = bone_src->mWeights;
				bone_src->mOffsetMatrix;
				
				// mesh->bones_per_vertex[weight_src->mVertexId].emplace_back(make_shared<Bone>(..~));
				// bone_src->mArmature
			}
		}
	}
	// scene->mAnimations[0]->
	return ret;
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