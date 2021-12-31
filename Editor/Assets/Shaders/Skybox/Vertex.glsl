#version 450

layout (push_constant) uniform SceneUniform {
    mat4 mvp;
} scene;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 UV;
layout (location = 2) in vec3 Normals;

layout (location = 0) out vec2 OutUV;
layout (location = 1) out vec3 OutPosition;
layout (location = 2) out vec3 WorldPos;

void main()
{
    gl_Position = scene.mvp * vec4(Position, 1.0);
    OutUV = UV;
    OutPosition = Position;
}