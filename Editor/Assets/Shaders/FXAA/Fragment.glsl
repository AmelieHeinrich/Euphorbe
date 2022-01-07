#version 450

layout (location = 0) in vec3 OutPosition;
layout (location = 1) in vec2 OutUV;

layout (location = 0) out vec4 OutColor;

layout (binding = 0, set = 0) uniform sampler screen_sampler;
layout (binding = 1, set = 0) uniform texture2D color_image;

layout (push_constant) uniform FXAASettings {
	// FXAA enabled
	bool enabled;
	bool show_edges;
	vec2 screen_size;

	// FXAA settings
	float luma_threshold;
	float mul_reduce;
	float min_reduce;
	float max_span;
} settings;

vec3 apply_fxaa(vec4 uv, texture2D tex, sampler samp, vec2 rcpFrame)
{
	vec3 rgbNW = textureLod(sampler2D(tex, samp), uv.zw, 0.0).xyz;
    vec3 rgbNE = textureLod(sampler2D(tex, samp), uv.zw + vec2(1,0) * rcpFrame.xy, 0.0).xyz;
    vec3 rgbSW = textureLod(sampler2D(tex, samp), uv.zw + vec2(0,1) * rcpFrame.xy, 0.0).xyz;
    vec3 rgbSE = textureLod(sampler2D(tex, samp), uv.zw + vec2(1,1) * rcpFrame.xy, 0.0).xyz;
    vec3 rgbM  = textureLod(sampler2D(tex, samp), uv.xy, 0.0).xyz;

    vec3 luma = vec3(0.299, 0.587, 0.114);
    float lumaNW = dot(rgbNW, luma);
    float lumaNE = dot(rgbNE, luma);
    float lumaSW = dot(rgbSW, luma);
    float lumaSE = dot(rgbSE, luma);
    float lumaM  = dot(rgbM,  luma);

    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    float dirReduce = max(
        (lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * settings.mul_reduce),
        settings.min_reduce);
    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    
    dir = min(vec2( settings.max_span,  settings.max_span),
          max(vec2(-settings.max_span, -settings.max_span),
          dir * rcpDirMin)) * rcpFrame.xy;

    vec3 rgbA = (1.0/2.0) * (
        textureLod(sampler2D(tex, samp), uv.xy + dir * (1.0/3.0 - 0.5), 0.0).xyz +
        textureLod(sampler2D(tex, samp), uv.xy + dir * (2.0/3.0 - 0.5), 0.0).xyz);
    vec3 rgbB = rgbA * (1.0/2.0) + (1.0/4.0) * (
        textureLod(sampler2D(tex, samp), uv.xy + dir * (0.0/3.0 - 0.5), 0.0).xyz +
        textureLod(sampler2D(tex, samp), uv.xy + dir * (3.0/3.0 - 0.5), 0.0).xyz);
    
    float lumaB = dot(rgbB, luma);

    if((lumaB < lumaMin) || (lumaB > lumaMax)) return rgbA;
    
    return rgbB;
}

void main()
{
    vec2 rcpFrame = 1.0 / settings.screen_size;
  	vec2 uv2 = OutUV;
        
    vec3 col;

    if (settings.enabled)
    {
        vec4 uv = vec4( uv2, uv2 - (rcpFrame * (0.5 + settings.luma_threshold)));
	    col = apply_fxaa(uv, color_image, screen_sampler, 1.0 / settings.screen_size.xy);
    }
    else
    {
        col = texture(sampler2D(color_image, screen_sampler), OutUV).xyz;
    }
    
    OutColor = vec4(col, 1.0);
}