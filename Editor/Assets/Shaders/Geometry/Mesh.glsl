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
	vec4 cone;
	uint vertices[64];   
    uint8_t indices[378];   
    uint8_t vertex_count;
	uint8_t triangle_count;
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

bool ConeCull(vec4 cone, vec3 view)
{
	return dot(cone.xyz, view) > cone.w;
}

void main()
{
	uint mi = gl_WorkGroupID.x;
	uint ti = gl_LocalInvocationID.x;

	if (ConeCull(meshlets[mi].cone, vec3(0, 0, 1)))
	{
		if (ti == 0)
			gl_PrimitiveCountNV = 0;
		return;
	}
	
	uint mhash = hash(mi);
	vec3 mcolor = vec3(float(mhash & 255), float((mhash >> 8) & 255), float((mhash >> 16) & 255)) / 255.0;

	uint vertexCount = uint(meshlets[mi].vertex_count);
	uint indexCount = uint(meshlets[mi].triangle_count) * 3;
	uint triangleCount = uint(meshlets[mi].triangle_count);

	for (uint i = ti; i < vertexCount; i += 32)
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
	
	for (uint i = ti; i < indexCount; i += 32)
		gl_PrimitiveIndicesNV[i] = uint(meshlets[mi].indices[i]);

	if (ti == 0)
		gl_PrimitiveCountNV = triangleCount;
}