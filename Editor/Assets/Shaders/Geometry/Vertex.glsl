#version 450

layout (push_constant) uniform SceneUniform {
    mat4 mvp;
} scene;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 UV;
layout (location = 2) in vec3 Normals;

layout (location = 0) out vec3 OutPos;
layout (location = 1) out vec2 OutUV;
layout (location = 2) out vec3 OutNormals;
layout (location = 3) out vec3 WorldPos;

void main()
{
    gl_Position = scene.mvp * vec4(Position, 1.0);

    OutPos = Position;
    OutUV = UV;
    OutNormals = Normals;
    WorldPos = vec4(scene.mvp * vec4(Position, 1.0)).xyz;
}