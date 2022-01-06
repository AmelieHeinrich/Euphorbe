#version 450

/*
 * A compute shader to convert an equirectangular map to a cubemap. Many thanks
 * to http://alinloghin.com/articles/compute_ibl.html, some of the code here was
 * adapted from their excellent article.
*/

layout (local_size_x = 32, local_size_y = 32) in;

layout (binding = 0) uniform samplerCube cube_map;
layout (rgba16f, binding = 1) writeonly uniform imageCube irradiance_map;

vec3 view_pos = vec3(0, 0, 0);

vec3 view_target[6] = {
    vec3(1, 0, 0),
    vec3(-1, 0, 0),
    vec3(0, -1, 0),
    vec3(0, 1, 0),
    vec3(0, 0, 1),
    vec3(0, 0, -1)
};

vec3 view_up[6] = {
    vec3(0, 1, 0),
    vec3(0, 1, 0),
    vec3(0, 0, 1), // +Y
    vec3(0, 0, -1), // -Y
    vec3(0, 1, 0),
    vec3(0, 1, 0)
};

mat4 look_at(vec3 pos, vec3 target, vec3 world_up) {
    vec3 fwd = normalize(target - pos);
    vec3 right = cross(world_up, fwd);
    vec3 up = cross(fwd, right);
    return mat4(vec4(right, 0), vec4(up, 0), vec4(fwd, 0), vec4(pos, 1));
}

// Remaps uv coordinates to a position in range [-1, 1]
vec2 uv_to_quad_pos(vec2 uv) {
    return uv * 2.0f - 1.0f;
}

vec3 to_local_direction(vec2 uv, int face) {
     mat4 view = look_at(view_pos, view_target[face], view_up[face]);
     return (view * vec4(uv_to_quad_pos(uv), 1, 0)).xyz;
}

#define PI 3.14159265359

vec3 calculate_irradiance(vec3 local_direction) {
    vec3 normal = normalize(local_direction);
    vec3 irradiance = vec3(0.0);

    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, normal));
    up = cross(normal, right);

    float sample_delta = 0.025;
    float n_samples = 0.0; 
    for(float phi = 0.0; phi < 2.0 * PI; phi += sample_delta) {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sample_delta) {
            vec3 tangent = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            vec3 sample_vec = tangent.x * right + tangent.y * up + tangent.z * normal;

            vec3 sample_color = texture(cube_map, sample_vec).rgb * cos(theta) * sin(theta);
            irradiance += sample_color;
            n_samples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(n_samples));

    return irradiance;
}

void main() {
    int face_size = imageSize(irradiance_map).x;
    ivec3 real_invocation_id = ivec3(gl_GlobalInvocationID.xyz);
    vec2 uv = (vec2(real_invocation_id.xy)) / float(face_size);
    // Avoid accessing out of bounds
    if (uv.x > 1 || uv.y > 1) return;
    vec3 local_direction = normalize(to_local_direction(uv, int(real_invocation_id.z)));
    local_direction.y = -local_direction.y;

    vec4 color = vec4(calculate_irradiance(local_direction), 1);

    ivec3 cube_texels = real_invocation_id;
    imageStore(irradiance_map, cube_texels, color);
}