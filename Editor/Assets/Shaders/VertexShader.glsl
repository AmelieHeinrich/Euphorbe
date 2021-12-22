#version 450

layout (binding = 0) uniform SceneUniform {
    mat4 mvp;
} scene;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 UV;

layout (location = 0) out vec2 OutUV;

void main()
{
    gl_Position = scene.mvp * vec4(Position, 1.0);
    OutUV = UV;
}