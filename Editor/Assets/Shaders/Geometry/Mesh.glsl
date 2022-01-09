#version 450

#extension GL_NV_mesh_shader : require
#extension GL_EXT_shader_8bit_storage : require
#extension GL_EXT_shader_16bit_storage : require

#define DEBUG_RECTANGLE 0

layout(local_size_x = 32, local_size_y = 1, local_size_z = 1) in;
layout(triangles, max_vertices = 64, max_primitives = 126) out;

layout (push_constant) uniform SceneData {
	mat4 projection;
	mat4 view;
	vec3 camera_position;
	float padding0;
} scene;

struct Vertex
{
	float px, py, pz;
	float ux, uy;
	float nx, ny, nz;
};

layout (binding = 0, set = 0) readonly buffer Vertices 
{
	Vertex vertex_data[];
};

struct Meshlet
{
	uint vertices[64];   
    uint8_t indices[378];   
    uint8_t vertex_count;
	uint8_t index_count;
};	

layout (binding = 1, set = 0) readonly buffer Meshlets 
{
	Meshlet meshlets[];
};

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

const vec3 vertices[4] = {
  vec3(-1,-1,0), 
  vec3(-1,1,0), 
  vec3(1,1,0), 
  vec3(1,-1,0)
};
 
const vec3 colors[4] = {
  vec3(1.0,0.0,0.0), 
  vec3(0.0,1.0,0.0), 
  vec3(0.0,0.0,1.0), 
  vec3(1.0,0.0,1.0)
};

void main()
{
#if DEBUG_RECTANGLE
	gl_MeshVerticesNV[0].gl_Position = scene.projection * scene.view * vec4(vertices[0], 1.0); 
	gl_MeshVerticesNV[1].gl_Position = scene.projection * scene.view * vec4(vertices[1], 1.0); 
	gl_MeshVerticesNV[2].gl_Position = scene.projection * scene.view * vec4(vertices[2], 1.0); 
	gl_MeshVerticesNV[3].gl_Position = scene.projection * scene.view * vec4(vertices[3], 1.0);
	
	VertexOut[0].MeshletColor = colors[0];
	VertexOut[1].MeshletColor = colors[1];
	VertexOut[2].MeshletColor = colors[2];
	VertexOut[3].MeshletColor = colors[3];
	
	gl_PrimitiveIndicesNV[0] = 0;
	gl_PrimitiveIndicesNV[1] = 1;
	gl_PrimitiveIndicesNV[2] = 2;
	
	gl_PrimitiveIndicesNV[3] = 2;
	gl_PrimitiveIndicesNV[4] = 3;
	gl_PrimitiveIndicesNV[5] = 0;
	
	gl_PrimitiveCountNV = 2;
#else

	uint mi = gl_WorkGroupID.x;
	
	uint mhash = hash(mi);
	vec3 mcolor = vec3(float(mhash & 255), float((mhash >> 8) & 255), float((mhash >> 16) & 255)) / 255.0;
	
	uint vertex_count = uint(meshlets[mi].vertex_count);
	for (uint i = 0; i < vertex_count; ++i)
	{
		uint vi = meshlets[mi].vertices[i];

		vec3 position = vec3(vertex_data[vi].px, vertex_data[vi].py, vertex_data[vi].pz);
		vec2 uv = vec2(vertex_data[vi].ux, vertex_data[vi].uy);
		vec3 normals = vec3(vertex_data[vi].nx, vertex_data[vi].ny, vertex_data[vi].nz);

		vec4 Pw = scene.projection * scene.view * ModelTransform.transform * vec4(position, 1.0);
	
		VertexOut[i].OutUV = uv;
		VertexOut[i].OutNormals = transpose(inverse(mat3(ModelTransform.transform))) * normals;
		VertexOut[i].WorldPos = vec3(ModelTransform.transform * vec4(position, 1.0));
		VertexOut[i].CameraPos = scene.camera_position;
		VertexOut[i].MeshletColor = mcolor;

		gl_MeshVerticesNV[i].gl_Position = Pw;
	}
	
	uint index_count = uint(meshlets[mi].index_count);
	gl_PrimitiveCountNV = index_count / 3;

	for (uint i = 0; i < index_count; ++i)
	{
		gl_PrimitiveIndicesNV[i] = uint(meshlets[mi].indices[i]);
	}
#endif
}