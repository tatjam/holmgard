#pragma once
#include <glad/glad.h>
#include "AssetManager.h"
#include "Shader.h"
#include "Material.h"
#include <renderer/camera/CameraUniforms.h>
#include <renderer/lighting/ShadowCamera.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <physics/glm/BulletGlmCompat.h>
#include <tiny_gltf/tiny_gltf.h>
#include <unordered_set>
#include "Asset.h"

class Model;

class Mesh
{
	friend class Model;
	friend struct Node;

private:

	GLuint vao, ebo;
	std::vector<GLuint> vbos;

	// It's only loaded while we are uploaded
	AssetHandle<Material> material;
	std::vector<ModelTexture> textures;

	bool drawable;

public:

	Model* in_model;

	MaterialOverride mat_override;

	glm::vec3 min_bound;
	glm::vec3 max_bound;
	
	std::vector<glm::vec3> get_verts();
	std::unordered_set<std::string> has_attributes;
	int mesh_idx;
	int prim_idx;

	// Stuff that starts with 'col_' are colliders
	// Any children of a non-drawable is not drawable!
	// Non-drawable stuff gets the aditional vertex positions loaded
	bool is_drawable() const;

	void upload();
	void unload();


	// Binds core uniforms and material uniforms
	void bind_uniforms(const CameraUniforms& uniforms, glm::dmat4 model, GLint drawable_id) const;

	// Only issues the draw command, does absolutely nothing else
	void draw_command() const;

	// We take ownership of the handle
	Mesh(AssetHandle<Material>&& mat, Model* rmodel) : material(std::move(mat))
	{
		in_model = rmodel;
		vao = 0;
		ebo = 0;
	}

};

struct Node
{
	std::string name;
	std::unordered_map<std::string, Node*> child_names;

	std::vector<Mesh> meshes;
	glm::dmat4 sub_transform;

	std::vector<Node*> children;

	std::unordered_map<std::string, std::string> properties;

	glm::vec3 min_bound;
	glm::vec3 max_bound;

	std::vector<const Mesh*> get_all_meshes_recursive(bool include_ours = true) const;

	std::vector<Node*> get_children_recursive() const;

	Node* get_child(const std::string& name) const;

	// n must be a children / ourselves
	glm::dmat4 get_tform(const Node* n) const;
	// to/from must be a children / ourselves
	std::vector<const Node*> get_path(const Node* to, const Node* from) const;
	// n must be a children / ourselves
	std::pair<glm::dvec3, glm::dvec3> get_bounds(const Node* n) const;

	void draw_all_meshes(const CameraUniforms& uniforms, GLint drawable_id, glm::dmat4 model) const;
	void draw_all_meshes_shadow(const ShadowCamera& sh_cam, glm::dmat4 model) const;
	void draw_all_meshes_override(const CameraUniforms& uniforms, const Material* mat, const MaterialOverride* mat_override,
		GLint drawable_id, glm::dmat4 model) const;

	// Draws all meshes, and call sthe same on all children,
	// accumulating sub transforms
	// The ignore_our_subtform flag is useful specially for stuff like parts
	// where the piece transform is ignored during game rendering
	void draw(const CameraUniforms& uniforms, glm::dmat4 model, GLint drawable_id, 
		bool ignore_our_subtform, bool increase_did = false) const;

	void draw_shadow(const ShadowCamera& sh_cam, glm::dmat4 model, bool ignore_our_subtform = false) const;

	// Draws everything using given material, is mat_override is null, the default material override will be
	// used, if it's non-null, the given one will be used
	// If mat is null then default materials are used, but the material override is applied
	void draw_override(const CameraUniforms& uniforms, const Material* mat, glm::dmat4 model, GLint drawable_id,
		const MaterialOverride* mat_over, bool ignore_our_subtform, bool increase_did = false) const;
};

// An empty mesh is a node, it contains only a transform
class Model : public Asset
{
private:

	friend class Mesh;
	friend struct GPUModelPointer;

	int gpu_users;

	bool uploaded;



	void upload();
	void unload();

	void load_node(const tinygltf::Model& model, int node, Node* parent, Model* rmodel, bool parent_draw);
	void load_mesh(const tinygltf::Model& model, const tinygltf::Primitive& primitive, Model* rmodel, Node* node,
				bool drawable, int mesh_idx, int prim_idx);

	void generic_load();
public:
	Node* root;

	// Here lives all the data which needs to be uploaded to the GPU
	tinygltf::Model gltf;

	static constexpr const char* COLLIDER_PREFIX = "col_";
	static constexpr const char* MARK_PREFIX = "m_";

	std::unordered_map<std::string, Node*> node_by_name;

	void get_gpu();
	void free_gpu();



	// For creating from code
	Model(ASSET_INFO);
	// Moves the model
	Model(tinygltf::Model&& model, ASSET_INFO);
	// Copies the model, slightly less efficient, used for generative geometry
	Model(const tinygltf::Model& model, ASSET_INFO);
	~Model();
};

Model* load_model(const std::string& path, const std::string& name, const std::string& pkg, const cpptoml::table& cfg);

// As models CAN BE pretty heavy on the GPU memory there is 
// code to allow you to upload and unload them using the
// GPUModelPointer, which is basically a RAII over the 
// GPU allocated model. When there are no GPU users, the model
// is unloaded from the GPU.
// Models, once loaded, are always kept in RAM for fast uploading
// They are a non-copyable resource, similar to AssetHandle
struct GPUModelPointer
{
private:

	friend class Model;

	AssetHandle<Model> model;


public:

	// You could store the Node pointer, but make sure
	// it doesn't outlive the GPUModelPointer.
	// It's better to simply call this every time, it's
	// pretty fast!
	Node* get_node(const std::string& name);

	// Same as above
	Node* get_root_node();

	GPUModelPointer() : model()
	{

	}

	// We must take the ownership
	GPUModelPointer(GPUModelPointer&& b)
	{
		this->model = std::move(b.model);
		b.model = AssetHandle<Model>();
	}

	GPUModelPointer& operator=(GPUModelPointer&& b)
	{
		this->model = std::move(b.model);
		b.model = AssetHandle<Model>();
		return *this;
	}

	GPUModelPointer duplicate() const
	{
		return GPUModelPointer(model.duplicate());
	}

	bool is_null()
	{
		return model.is_null();
	}

	// We take ownership of the asset handle (std::move)
	GPUModelPointer(AssetHandle<Model>&& model);
	~GPUModelPointer();

	// We can safely handle model.data directly, allowing const for convenience
	const Model* operator->() const
	{
		return model.get();
	}

	const Model& operator*() const
	{
		return *model.get();
	}

};

// Useful if you only want a node, same as before, non copyable
struct GPUModelNodePointer
{
	GPUModelPointer model_ptr;
	Node* sub_node;

	GPUModelNodePointer() : model_ptr()
	{
		sub_node = nullptr;
	}

	GPUModelNodePointer(GPUModelNodePointer&& b) : model_ptr(std::move(b.model_ptr))
	{
		b.model_ptr = GPUModelPointer(AssetHandle<Model>());
		sub_node = b.sub_node;
	}

	GPUModelNodePointer& operator=(GPUModelNodePointer&& b)
	{
		this->model_ptr = std::move(b.model_ptr);
		b.model_ptr = GPUModelPointer(AssetHandle<Model>());
		this->sub_node = b.sub_node;

		return *this;
	}

	GPUModelNodePointer duplicate() const
	{
		return GPUModelNodePointer(std::move(model_ptr.duplicate()), sub_node->name);
	}

	bool is_null()
	{
		return model_ptr.is_null();
	}

	GPUModelNodePointer(GPUModelPointer&& n_model_ptr, const std::string& sub_name) : model_ptr(std::move(n_model_ptr))
	{
		this->sub_node = model_ptr.get_node(sub_name);
	}

	// Automatic destructor is good


	const Node* operator->() const
	{
		return sub_node;
	}

	const Node& operator*() const
	{
		return *sub_node;
	}


};


// Used to easily extract colliders from any model
class ModelColliderExtractor
{
public:

	// Simple checks
	static void single_collider_common(Node* n);

	//        min        max
	static std::pair<glm::vec3, glm::vec3> obtain_bounds(Mesh* m);

	static void load_collider_compound(btCollisionShape** target, Node* n);
	static void load_collider_box(btCollisionShape** target, Node* n);
	static void load_collider_sphere(btCollisionShape** target, Node* n);
	static void load_collider_cylinder(btCollisionShape** target, Node* n);
	static void load_collider_cone(btCollisionShape** target, Node* n);
	static void load_collider_capsule(btCollisionShape** target, Node* n);
	static void load_collider_concave(btCollisionShape** target, Node* n);
	static void load_collider_convex(btCollisionShape** target, Node* n);

	static void load_collider(btCollisionShape** target, Node* n);
};


struct NodeWrapper;

// This is exposed to lua as functions of tinygltf::Model, with a custom constructor
class ModelBuilder
{
public:
	static NodeWrapper create_node(std::shared_ptr<tinygltf::Model> mod, const std::string& name, const NodeWrapper* parent);
	static NodeWrapper create_node(std::shared_ptr<tinygltf::Model> mod, const std::string& name);
};

struct PrimitiveWrapper
{
	size_t get_vertex_count();
	std::shared_ptr<tinygltf::Model> owner;
	int prim_idx;
	int node_idx;
	tinygltf::Primitive* get_prim();


	void set_positions(const std::vector<glm::dvec3>& pos);
	void set_normals(const std::vector<glm::dvec3>& nrm);
	void set_tex0(const std::vector<glm::dvec2>& tex0);
	void set_tex1(const std::vector<glm::dvec2>& tex1);
	void set_tangents(const std::vector<glm::dvec3>& tgt);
	void set_color3(const std::vector<glm::dvec3>& color3);
	void set_color4(const std::vector<glm::dvec4>& color4);

	// Indices must be set after atleast one of the functions before has been called
	// so we can validate number of vertices
	// BEWARE: Indices start at 0!
	void set_indices(const std::vector<int>& idx);

};

struct NodeWrapper
{
	std::shared_ptr<tinygltf::Model> owner;
	int node_idx;
	tinygltf::Node* get_node() const;

	void set_transform(const glm::dmat4& mat) const;
	void set_transform(const glm::dvec3& trans, const glm::dvec3& scale, const glm::dquat& rot) const;
	void set_translation(glm::dvec3 trans) const;
	void set_scale(glm::dvec3 scale) const;
	void set_rotation(glm::dquat rot) const;

	// Note that technically two nodes can point to the same mesh, this is not supported
	// TODO: This may become necessary in the future, but requires some changes to Model too
	PrimitiveWrapper create_primitive();
};
