#version 450

#extension GL_NV_mesh_shader : require

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(triangles, max_vertices = 64, max_primitives = 126) out;

layout (push_constants) uniform SceneData {
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

layout (binding = 0, set = 0) uniform ObjectData {
	mat4 transform;
} ModelTransform;

layout (binding = 1, set = 0) buffer Vertices {
	MeshVertex vertices[];
} VertexBuffer;

struct Meshlet
{
	uint vertices[64];
	uint indices[378]; 
	uint vertex_count;
	uint index_count;
};	

layout (binding = 2, set = 0) buffer Meshlets {
	Meshlet meshlets[];
} MeshletBuffer;

layout (location = 0) out PerVertexData {
	vec2 OutUV;
	vec3 OutNormals;
	vec3 WorldPos;
	vec3 CameraPos;
} VertexOut[];

void main()
{
	uint mi = gl_WorkGroupID.x;
	uint thread_id = gl_LocalInvocationID.x;

	uint vertex_count = MeshletBuffer.meshlets[mi].vertex_count;
	for (uint i = 0; i < vertex_count; ++i)
	{
		uint vi = MeshletBuffer.meshlets[mi].vertices[i];

		vec4 Pw = ModelTransform.transform * VertexBuffer.vertices[vi].Position;
		vec4 P = scene.projection * scene.view * Pw;

		// GL->VK conventions...
		//P.y = -P.y;
		//P.z = (P.z + P.w) / 2.0;

		gl_MeshVerticesNV[i].gl_Position = P;

		VertexOut[i].OutUV = VertexBuffer.vertices[vi].UV;
		VertexOut[i].OutNormals = transpose(inverse(mat3(ModelTransform.transform))) * VertexBuffer.vertices[vi].Normals;
		VertexOut[i].WorldPos = vec3(ModelTransform.transform * vec4(VertexBuffer.vertices[vi].Position, 1.0));
		VertexOut[i].CameraPos = scene.camera_position;
	}

	uint index_count = MeshletBuffer.meshlets[mi].index_count;
	gl_PrimitiveCountNV = uint(index_count) / 3;

	for (uint i = 0; i < index_count; ++i)
	{
		gl_PrimitiveIndicesNV[i] = uint(MeshletBuffer.meshlets[mi].indices[i]);
	}
}