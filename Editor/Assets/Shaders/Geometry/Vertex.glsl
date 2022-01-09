#version 450

layout (push_constant) uniform SceneUniform {
    mat4 projection;
    mat4 view;
    vec3 camera_pos;
    float padding_0;
} scene;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 UV;
layout (location = 2) in vec3 Normals;

layout (location = 0) out PerVertexData {
	vec2 OutUV;
	vec3 OutNormals;
	vec3 WorldPos;
	vec3 CameraPos;
} VertexOut; 

layout (binding = 0, set = 0) uniform ModelData {
    mat4 model;
} model_data;

void main()
{
    gl_Position = scene.projection * scene.view * model_data.model * vec4(Position, 1.0);

    VertexOut.OutUV = UV;
    VertexOut.OutNormals = transpose(inverse(mat3(model_data.model))) * Normals;
    VertexOut.WorldPos = vec3(model_data.model * vec4(Position, 1.0));
    VertexOut.CameraPos = scene.camera_pos;
}