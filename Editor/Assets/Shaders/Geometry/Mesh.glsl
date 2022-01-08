#version 450

#extension GL_NV_mesh_shader : require
#extension GL_EXT_shader_8bit_storage : require
#extension GL_EXT_shader_16bit_storage : require

layout(local_size_x = 32, local_size_y = 1, local_size_z = 1) in;
layout(triangles, max_vertices = 64, max_primitives = 126) out;

layout (push_constant) uniform SceneData {
	mat4 projection;
	mat4 view;
	vec3 camera_position;
	float padding0;
} scene;

struct MeshVertex
{
	vec3 Position;
	vec2 UV;
	vec3 Normals;
};

layout (binding = 0, set = 0) buffer Vertices {
	MeshVertex vertices[];
} VertexBuffer;

struct Meshlet
{
	uint vertices[64];
	uint8_t indices[378]; // Up to 126 triangles
	uint8_t vertex_count;
	uint8_t index_count;
};	

layout (binding = 1, set = 0) buffer Meshlets {
	Meshlet meshlets[];
} MeshletBuffer;

layout (binding = 0, set = 1) uniform ObjectData {
	mat4 transform;
} ModelTransform;

layout (location = 0) out PerVertexData {
	vec2 OutUV;
	vec3 OutNormals;
	vec3 WorldPos;
	vec3 CameraPos;
	vec3 MeshletColor;
} VertexOut[];

uint hash(uint a)
{
	a = (a+0x7ed55d16) + (a<<12);
	a = (a^0xc761c23c) ^ (a>>19);
	a = (a+0x165667b1) + (a<<5);
	a = (a+0xd3a2646c) ^ (a<<9);
	a = (a+0xfd7046c5) + (a<<3);
	a = (a^0xb55a4f09) ^ (a>>16);
	return a;
}

void main()
{
	uint mi = gl_WorkGroupID.x;

	uint mhash = hash(mi);
	vec3 mcolor = vec3(float(mhash & 255), float((mhash >> 8) & 255), float((mhash >> 16) & 255)) / 255.0;

	for (uint i = 0; i < uint(MeshletBuffer.meshlets[mi].vertex_count); ++i)
	{
		uint vi = MeshletBuffer.meshlets[mi].vertices[i];

		vec4 Pw = ModelTransform.transform * vec4(VertexBuffer.vertices[vi].Position, 1.0);
		vec4 P = scene.projection * scene.view * Pw;

		gl_MeshVerticesNV[i].gl_Position = P;

		VertexOut[i].OutUV = VertexBuffer.vertices[vi].UV;
		VertexOut[i].OutNormals = transpose(inverse(mat3(ModelTransform.transform))) * VertexBuffer.vertices[vi].Normals;
		VertexOut[i].WorldPos = vec3(ModelTransform.transform * vec4(VertexBuffer.vertices[vi].Position, 1.0));
		VertexOut[i].CameraPos = scene.camera_position;
		VertexOut[i].MeshletColor = mcolor;
	}

	gl_PrimitiveCountNV = uint(MeshletBuffer.meshlets[mi].index_count) / 3;

	for (uint i = 0; i < uint(MeshletBuffer.meshlets[mi].index_count); ++i)
	{
		gl_PrimitiveIndicesNV[i] = uint(MeshletBuffer.meshlets[mi].indices[i]);
	}
}