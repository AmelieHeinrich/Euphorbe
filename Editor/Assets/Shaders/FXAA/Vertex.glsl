#version 450

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 UV;

layout (location = 0) out vec3 OutPosition;
layout (location = 1) out vec2 OutUV;
layout (location = 3) out vec2 rgbNW;
layout (location = 4) out vec2 rgbNE;
layout (location = 5) out vec2 rgbSW;
layout (location = 6) out vec2 rgbSE;
layout (location = 7) out vec2 rgbM;

void texcoords(vec2 fragCoord, vec2 resolution,
			out vec2 v_rgbNW, out vec2 v_rgbNE,
			out vec2 v_rgbSW, out vec2 v_rgbSE,
			out vec2 v_rgbM) {
	vec2 inverseVP = 1.0 / resolution.xy;
	v_rgbNW = (fragCoord + vec2(-1.0, -1.0)) * inverseVP;
	v_rgbNE = (fragCoord + vec2(1.0, -1.0)) * inverseVP;
	v_rgbSW = (fragCoord + vec2(-1.0, 1.0)) * inverseVP;
	v_rgbSE = (fragCoord + vec2(1.0, 1.0)) * inverseVP;
	v_rgbM = vec2(fragCoord * inverseVP);
}

void main()
{
	gl_Position = vec4(Position, 1.0);

	OutPosition = Position;
	OutUV = UV;
}