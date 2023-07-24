#include "game/entity/Entity.h"
#include "game/component/Mesh.h"
#include "render/Renderer.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
using namespace std;

static Entity* loadFromFileImpl(
	aiNode* node,
	const vector<shared_ptr<Mesh>>& meshes,
	map<string,Entity*>& mapping){
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
		ret->addMesh(meshes[node->mMeshes[idx_mesh]]);
	};
	assert(!mapping.contains(node->mName.C_Str()));
	return mapping[node->mName.C_Str()]=ret;
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
		const auto& material_src = scene->mMaterials[idx_mat];
		int diffuse_count = material_src->GetTextureCount(aiTextureType_DIFFUSE);
		if(diffuse_count > 0){
			aiString assimp_textureName;
			material_src->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), assimp_textureName);
			const auto& model_directory = model_path.substr(0,model_path.find_last_of('/'));
			materials.emplace_back(make_shared<Material>(model_directory+"/"+assimp_textureName.data));
		}else{
			materials.emplace_back(make_shared<Material>());
		}
	}

	vector<shared_ptr<Mesh>> meshes;
	for(int idx_mesh=0;idx_mesh<scene->mNumMeshes;idx_mesh++){
		const auto& mesh_src = scene->mMeshes[idx_mesh];
		auto& mesh = meshes.emplace_back(make_shared<Mesh>());
		assert(mesh_src->mPrimitiveTypes & aiPrimitiveType_TRIANGLE);

		// copy shape informations
		mesh->primitive_type = PrimitiveType::triangles;
		mesh->vertices.resize(mesh_src->mNumVertices);
		mesh->normals.resize(mesh_src->mNumVertices);
		mesh->texcoord.resize(mesh_src->mNumVertices);
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

		mesh->bones.resize(mesh_src->mNumBones);
		mesh->bone_influences.resize(mesh_src->mNumVertices);
		for(int bone_idx=0; bone_idx<mesh_src->mNumBones; bone_idx++){
			const auto& bone_src = mesh_src->mBones[bone_idx];
			auto& bone = mesh->bones[bone_idx];
			const auto& offset_src = bone_src->mOffsetMatrix;
			bone.offset = {
				offset_src.a1, offset_src.a2, offset_src.a3, offset_src.a4,
				offset_src.b1, offset_src.b2, offset_src.b3, offset_src.b4,
				offset_src.c1, offset_src.c2, offset_src.c3, offset_src.c4,
				offset_src.d1, offset_src.d2, offset_src.d3, offset_src.d4,
			};
			for(int idx_weight=0;idx_weight<bone_src->mNumWeights;idx_weight++){
				int target_vertex = bone_src->mWeights[idx_weight].mVertexId;
				float weight = bone_src->mWeights[idx_weight].mWeight;

				int influence_idx=0;
				while(influence_idx<MAX_BONE_INFLUENCE and mesh->bone_influences[target_vertex][influence_idx].bone)
					influence_idx++;
				assert(influence_idx<MAX_BONE_INFLUENCE);

				mesh->bone_influences[target_vertex][influence_idx].bone=&bone;
				mesh->bone_influences[target_vertex][influence_idx].weight=weight;
			}
			cout<<"BoneName: "<<bone_src->mName.C_Str()<<endl;
			// cout<<"NodeName: "<<bone_src->mNode->mName.C_Str()<<endl; == bone name
		}
	}

	map<string,Entity*> mapping;
	auto ret = loadFromFileImpl(scene->mRootNode, meshes, mapping);
	if(scene->mAnimations){
		const auto& anim_src=scene->mAnimations[0];
		ret->anims.emplace_back(
			string(anim_src->mName.C_Str()),
			float(anim_src->mDuration),
			float(anim_src->mTicksPerSecond)
		);
		for(int i=0;i<anim_src->mNumChannels;i++){
			const auto& channel_src=anim_src->mChannels[i];
			ret->anims.back().channels.emplace_back(
				mapping[channel_src->mNodeName.C_Str()]
			);
			for(int j=0;j<channel_src->mNumPositionKeys;j++){
				ret->anims.back().channels.back().positions.emplace_back(
					channel_src->mPositionKeys[j].mTime,
					Vec3{
						channel_src->mPositionKeys[j].mValue.x,
						channel_src->mPositionKeys[j].mValue.y,
						channel_src->mPositionKeys[j].mValue.z,
					}
				);
			}
			for(int j=0;j<channel_src->mNumRotationKeys;j++){
				ret->anims.back().channels.back().rotations.emplace_back(
					channel_src->mRotationKeys[j].mTime,
					Quaternion{
						channel_src->mRotationKeys[j].mValue.w,
						channel_src->mRotationKeys[j].mValue.x,
						channel_src->mRotationKeys[j].mValue.y,
						channel_src->mRotationKeys[j].mValue.z,
					}
				);
			}
			for(int j=0;j<channel_src->mNumScalingKeys;j++){
				ret->anims.back().channels.back().scales.emplace_back(
					channel_src->mScalingKeys[j].mTime,
					Vec3{
						channel_src->mScalingKeys[j].mValue.x,
						channel_src->mScalingKeys[j].mValue.y,
						channel_src->mScalingKeys[j].mValue.z,
					}
				);
			}
		}
	}
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
		vector<Vec3> vertices_cur(mesh->vertices.size()), normals_cur(mesh->vertices.size());
		for(int i=0;i<mesh->vertices.size();i++){
			vertices_cur.emplace_back(mesh->vertices[i]);
			normals_cur.emplace_back(mesh->normals[i]);
		}
		render_q.emplace_back(
			parent2world*local2parent(),
			mesh->primitive_type,
			mesh->vertices,
			mesh->normals,
			mesh->texcoord,
			mesh->faces,
			mesh->material->getTextureID());
	}
	render_q.emplace_back(
		parent2world*local2parent(),
		PrimitiveType::lines,
		std::vector<Vec3>{{0,0,0},{0.1,0,0}, {0.1,0,0},{0.1,0.1,0}, {0.1,0.1,0},{0,0.1,0}, {0,0.1,0},{0,0,0}},
		std::vector<Vec3>{},
		std::vector<Vec3>{},
		std::vector<std::vector<uint>>{},
		unsigned(-1));
}

void Entity::update(float delta_time){
	if(onUpdate)
		onUpdate(this, delta_time);
	for(auto i:children)
		i->update(delta_time);
}