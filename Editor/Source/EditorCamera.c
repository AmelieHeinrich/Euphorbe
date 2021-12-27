#include "EditorCamera.h"

#include <Euphorbe/Core/Log.h>

void EditorCameraUpdateVectors(EditorCamera* camera)
{
	vec3 front;
	glm_vec3_zero(front);
	front[0] = cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
	front[1] = sin(glm_rad(camera->pitch));
	front[2] = sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
	glm_normalize_to(front, camera->front);

	vec3 right_crossed;
	vec3 up_crossed;
	glm_vec3_zero(right_crossed);
	glm_vec3_zero(up_crossed);
	glm_cross(camera->front, camera->worldup, right_crossed);
	glm_cross(camera->right, camera->front, up_crossed);

	glm_normalize_to(right_crossed, camera->right);
	glm_normalize_to(up_crossed, camera->up);
}

void EditorCameraInit(EditorCamera* camera)
{
	glm_vec3_zero(camera->position);
	glm_vec3_zero(camera->up);
	glm_vec3_zero(camera->front);
	glm_vec3_zero(camera->right);
	glm_vec3_zero(camera->worldup);

	camera->up[1] = 1.0f;
	camera->front[2] = -1.0f;
	camera->worldup[1] = 1.0f;
	camera->yaw = CAMERA_DEFAULT_YAW;
	camera->pitch = CAMERA_DEFAULT_PITCH;
	camera->zoom = CAMERA_DEFAULT_ZOOM;
	camera->friction = 10.0f;
	camera->acceleration = 20.0f;
	camera->max_velocity = 15.0f;
	camera->viewport_width = 1280;
	camera->viewport_height = 720;

	EditorCameraUpdateVectors(camera);
}

void EditorCameraUpdate(EditorCamera* camera, f32 dt)
{
	f32 mouse_x = E_GetMousePosX();
	f32 mouse_y = E_GetMousePosY();

	camera->mouse_pos[0] = mouse_x;
	camera->mouse_pos[1] = mouse_y;

	glm_mat4_identity(camera->view);
	glm_mat4_identity(camera->proj);
	glm_mat4_identity(camera->camera_matrix);

	vec3 center;
	glm_vec3_zero(center);
	glm_vec3_add(camera->position, camera->front, center);
	glm_lookat(camera->position, center, camera->worldup, camera->view);

	glm_perspective(glm_rad(camera->zoom), camera->viewport_width / camera->viewport_height, 0.0001f, 100000.0f, camera->proj);
	camera->proj[1][1] *= -1;

	glm_mat4_mul(camera->proj, camera->view, camera->camera_matrix);
}

void EditorCameraProcessInput(EditorCamera* camera, f32 dt)
{
	// keyboard
	f32 speed_multiplier = camera->acceleration * dt;
	if (E_IsKeyPressed(EUPHORBE_KEY_Z) || E_IsKeyPressed(EUPHORBE_KEY_W))
		glm_vec3_add(camera->velocity, (vec3) { camera->front[0] * speed_multiplier, camera->front[1] * speed_multiplier, camera->front[2] * speed_multiplier }, camera->velocity);
	if (E_IsKeyPressed(EUPHORBE_KEY_S))
		glm_vec3_sub(camera->velocity, (vec3) { camera->front[0] * speed_multiplier, camera->front[1] * speed_multiplier, camera->front[2] * speed_multiplier }, camera->velocity);
	if (E_IsKeyPressed(EUPHORBE_KEY_Q) || E_IsKeyPressed(EUPHORBE_KEY_A))
		glm_vec3_sub(camera->velocity, (vec3) { camera->right[0] * speed_multiplier, camera->right[1] * speed_multiplier, camera->right[2] * speed_multiplier }, camera->velocity);
	if (E_IsKeyPressed(EUPHORBE_KEY_D))
		glm_vec3_add(camera->velocity, (vec3) { camera->right[0] * speed_multiplier, camera->right[1] * speed_multiplier, camera->right[2] * speed_multiplier }, camera->velocity);

	f32 friction_multiplier = 1.0f / (1.0f + (camera->friction * dt));
	glm_vec3_mul(camera->velocity, (vec3) { friction_multiplier, friction_multiplier, friction_multiplier }, camera->velocity);

	f32 vec_length = (camera->velocity[0] * camera->velocity[0]) + (camera->velocity[1] * camera->velocity[1]) + (camera->velocity[2] * camera->velocity[2]);
	if (vec_length > camera->max_velocity)
	{
		vec3 normalised_velocity;
		glm_vec3_normalize_to(camera->velocity, normalised_velocity);
		glm_vec3_mul(normalised_velocity, (vec3) { camera->max_velocity, camera->max_velocity, camera->max_velocity }, camera->velocity);
	}

	glm_vec3_add(camera->position, (vec3) { camera->velocity[0] * dt, camera->velocity[1] * dt, camera->velocity[2] * dt }, camera->position);

	// mouse

	f32 mouse_x = E_GetMousePosX();
	f32 mouse_y = E_GetMousePosY();

	f32 dx = (mouse_x - camera->mouse_pos[0]) * (CAMERA_DEFAULT_MOUSE_SENSITIVITY * dt);
	f32 dy = (mouse_y - camera->mouse_pos[1]) * (CAMERA_DEFAULT_MOUSE_SENSITIVITY * dt);

	if (E_IsMouseButtonPressed(EUPHORBE_MOUSE_LEFT))
	{
		camera->yaw += dx; // Inverse camera controls
		camera->pitch -= dy; // Inverse camera controls
	}

	EditorCameraUpdateVectors(camera);
}

void EditorCameraOnMouseScroll(EditorCamera* camera, f32 scroll)
{
	camera->zoom -= scroll;
	if (camera->zoom < 1.0f)
		camera->zoom = 1.0f;
	if (camera->zoom > 90.0f)
		camera->zoom = 90.0f;

	EditorCameraUpdateVectors(camera);
}

void EditorCameraResize(EditorCamera* camera, i32 width, i32 height)
{
	camera->viewport_width = (f32)width;
	camera->viewport_height = (f32)height;

	EditorCameraUpdateVectors(camera);
}
