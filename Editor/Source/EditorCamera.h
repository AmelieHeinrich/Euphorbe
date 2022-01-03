#ifndef EUPHORBE_EDITOR_CAMERA_H
#define EUPHORBE_EDITOR_CAMERA_H

#include <Euphorbe/Core/Common.h>
#include <Euphorbe/Platform/Input.h>
#include <cglm/cglm.h>
#include <cglm/call.h>

#define CAMERA_DEFAULT_YAW -90.0f
#define CAMERA_DEFAULT_PITCH 0.0f
#define CAMERA_DEFAULT_SPEED 1.0f
#define CAMERA_DEFAULT_MOUSE_SENSITIVITY 5.0f
#define CAMERA_DEFAULT_ZOOM 90.0f

typedef struct EditorCamera EditorCamera;
struct EditorCamera
{
	f32 yaw;
	f32 pitch;
	f32 zoom;

	vec3 position;
	vec3 front;
	vec3 up;
	vec3 right;
	vec3 worldup;

	vec2 mouse_pos;
	b32 first_mouse;

	mat4 view;
	mat4 proj;
	mat4 camera_matrix; // view * proj

	f32 viewport_width;
	f32 viewport_height;

	f32 acceleration;
	f32 friction;
	vec3 velocity;
	f32 max_velocity;
};

void EditorCameraInit(EditorCamera* camera);
void EditorCameraUpdate(EditorCamera* camera, f32 dt);
void EditorCameraProcessInput(EditorCamera* camera, f32 dt);
void EditorCameraProcessMouse(EditorCamera* camera, f32 dt);
void EditorCameraOnMouseScroll(EditorCamera* camera, f32 scroll);
void EditorCameraResize(EditorCamera* camera, i32 width, i32 height);

#endif