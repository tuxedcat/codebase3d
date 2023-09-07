#include "game/entity/Entity.h"
#include "game/component/Mesh.h"
#include "render/Renderer.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "glm/gtx/matrix_decompose.hpp"
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

	glm::mat4 transformation; // your transformation matrix.
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;

	// Workaround
	ret->position({mtx.a4,mtx.b4,mtx.c4});
	mtx.a4=mtx.b4=mtx.c4=0;

	transformation[0][0]=mtx.a1; transformation[0][1]=mtx.a2; transformation[0][2]=mtx.a3; transformation[0][3]=mtx.a4;
	transformation[1][0]=mtx.b1; transformation[1][1]=mtx.b2; transformation[1][2]=mtx.b3; transformation[1][3]=mtx.b4;
	transformation[2][0]=mtx.c1; transformation[2][1]=mtx.c2; transformation[2][2]=mtx.c3; transformation[2][3]=mtx.c4;
	transformation[3][0]=mtx.d1; transformation[3][1]=mtx.d2; transformation[3][2]=mtx.d3; transformation[3][3]=mtx.d4;
	glm::decompose(transformation, scale, rotation, translation, skew, perspective);

	ret->scale({
		scale.x,
		scale.y,
		scale.z});
	ret->rotate({rotation.w,rotation.x,rotation.y,rotation.z});

	for(int i=0;i<node->mNumChildren;i++){
		ret->adopt(loadFromFileImpl(node->mChildren[i], meshes, mapping));
	}

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

	map<string,Bone*> name2bone;
	map<string,Entity*> name2bonenode;
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

		// copy bone informations
		mesh->bone_influences.resize(mesh_src->mNumVertices);
		mesh->bones.resize(mesh_src->mNumBones);//Allocate memory before emplace_back to prevent dangling pointer.
		for(int bone_idx=0; bone_idx<mesh_src->mNumBones; bone_idx++){
			const auto& bone_src = mesh_src->mBones[bone_idx];
			const auto& offset_src = bone_src->mOffsetMatrix;
			auto& bone = mesh->bones[bone_idx];
			bone.offset={
				offset_src.a1, offset_src.a2, offset_src.a3, offset_src.a4,
				offset_src.b1, offset_src.b2, offset_src.b3, offset_src.b4,
				offset_src.c1, offset_src.c2, offset_src.c3, offset_src.c4,
				offset_src.d1, offset_src.d2, offset_src.d3, offset_src.d4,
			};
			bone.name=bone_src->mName.C_Str();
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
			name2bone[bone.name]=&bone;
		}
	}

	auto ret = loadFromFileImpl(scene->mRootNode, meshes, name2bonenode);
	if(scene->mAnimations){
		const auto& anim_src=scene->mAnimations[0];
		for(int i=0;i<anim_src->mNumChannels;i++){
			const auto& channel_src=anim_src->mChannels[i];
			auto& bone = name2bonenode[channel_src->mNodeName.C_Str()];
			name2bone[bone->name]->node=bone;
			bone->anims.emplace_back(
				string(anim_src->mName.C_Str()),
				float(anim_src->mDuration),
				float(anim_src->mTicksPerSecond)
			);
			for(int j=0;j<channel_src->mNumPositionKeys;j++){
				bone->anims.back().positions.emplace_back(
					channel_src->mPositionKeys[j].mTime,
					Vec3{
						channel_src->mPositionKeys[j].mValue.x,
						channel_src->mPositionKeys[j].mValue.y,
						channel_src->mPositionKeys[j].mValue.z,
					}
				);
			}
			for(int j=0;j<channel_src->mNumRotationKeys;j++){
				bone->anims.back().rotations.emplace_back(
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
				bone->anims.back().scales.emplace_back(
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

void Entity::draw(const Mat44& world2camera, Mat44 parent2world, std::vector<RenderRequest>& render_q)const{
	for(auto i: children){
		i->draw(world2camera, parent2world*local2parent(), render_q);
	}

	for(auto mesh:meshes){
		vector<Vec3> vertices_cur(mesh->vertices.size()), normals_cur(mesh->vertices.size());
		for(int i=0;i<mesh->vertices.size();i++){
			Vec3 vtx_skinned;
			for(int j=0;j<MAX_BONE_INFLUENCE;j++){
				if(mesh->bone_influences[i][j].weight!=0)
					vtx_skinned += mesh->bone_influences[i][j].bone->node->local2world()*mesh->bone_influences[i][j].bone->offset * mesh->vertices[i] * mesh->bone_influences[i][j].weight;
			}
			vertices_cur[i]=vtx_skinned;
			//TODO: normal
			normals_cur[i]=mesh->normals[i];
		}
		render_q.emplace_back(
			world2camera*local2parent(),
			mesh->primitive_type,
			vertices_cur,
			normals_cur,
			mesh->texcoord,
			mesh->faces,
			mesh->material->getTextureID());
	}
	// //bone tree
	// render_q.emplace_back(
	// 	world2camera*parent2world,
	// 	PrimitiveType::lines,
	// 	std::vector<Vec3>{{0,0,0},position()},
	// 	std::vector<Vec3>{},
	// 	std::vector<Vec3>{},
	// 	std::vector<std::vector<uint>>{},
	// 	unsigned(-1));
}

void Entity::update(float delta_time){
	static float elapsed=0.0f;
	elapsed+=delta_time;
	if(anims.size()){
		
		if(auto it = upper_bound(anims.back().positions.begin(),anims.back().positions.end(),elapsed,[](float time, const auto& keyframe){
				return time < keyframe.first;
			});it!=anims.back().positions.begin() and it!=anims.back().positions.end()){
			auto [t0,v0] = *prev(it);
			auto [t1,v1] = *it;
			position(v0.lerp(v1, (elapsed-t0)/(t1-t0)));
		}
		
		if(auto it = upper_bound(anims.back().rotations.begin(),anims.back().rotations.end(),elapsed,[](float time, const auto& keyframe){
				return time < keyframe.first;
			});it!=anims.back().rotations.begin() and it!=anims.back().rotations.end()){
			auto [t0,q0] = *prev(it);
			auto [t1,q1] = *it;
			rotate(q0.slerp(q1, (elapsed-t0)/(t1-t0)).normalized());
		}
		
		if(auto it = upper_bound(anims.back().scales.begin(),anims.back().scales.end(),elapsed,[](float time, const auto& keyframe){
				return time < keyframe.first;
			});it!=anims.back().scales.begin() and it!=anims.back().scales.end()){
			auto [t0,v0] = *prev(it);
			auto [t1,v1] = *it;
			scale(v0.lerp(v1, (elapsed-t0)/(t1-t0)));
		}
	}


	if(onUpdate)
		onUpdate(this, delta_time);
	for(auto i:children)
		i->update(delta_time);
}