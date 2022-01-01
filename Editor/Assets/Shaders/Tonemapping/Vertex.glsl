#version 450

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 UV;

layout (location = 0) out vec3 OutPosition;
layout (location = 1) out vec2 OutUV;

void main()
{
	gl_Position = vec4(Position, 1.0);

	OutPosition = Position;
	OutUV = UV;
}