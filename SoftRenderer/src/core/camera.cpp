#include "../include/camera.h"
#include "../platform/win32.h"

Camera::Camera(Point3 e, Point3 tar, vec3 up, float asp, float vfovRad, float n, float f):eye(e),target(tar),Up(up),aspect(asp),vfov(vfovRad),nearplane(n),farplane(f){}

Camera::~Camera()
{
}

mat4 Camera::get_Perspective()const 
{
	return mat4_perspective(vfov, aspect, nearplane, farplane);
}
mat4 Camera::get_ViewMatrix()const 
{
	return mat4_lookat(eye, target, Up);
}

mat4 Camera::get_VPMatrix()const
{
	return get_Perspective()*get_ViewMatrix();
}
void Camera::update_camera_pos()
{
	vec3 from_target = eye - target;			// vector point from target to camera's position
	float radius = from_target.norm();

	float phi = (float)atan2(from_target[0], from_target[2]); // azimuth angle(方位角), angle between from_target and z-axis，[-pi, pi]
	float theta = (float)acos(from_target[1] / radius);		  // zenith angle(天顶角), angle between from_target and y-axis, [0, pi]
	float x_delta = window->mouse_info.orbit_delta[0] / window->width;
	float y_delta = window->mouse_info.orbit_delta[1] / window->height;

	// for mouse wheel
	radius *= (float)pow(0.95, window->mouse_info.wheel_delta);

	float factor = 1.5 * PI;
	// for mouse left button
	phi += x_delta * factor;
	theta += y_delta * factor;
	if (theta > PI) theta = PI - EPSILON * 100;
	if (theta < 0)  theta = EPSILON * 100;

	eye[0] = target[0] + radius * sin(phi) * sin(theta);
	eye[1] = target[1] + radius * cos(theta);
	eye[2] = target[2] + radius * sin(theta) * cos(phi);

	// for mouse right button
	factor = radius * (float)tan(60.0 / 360 * PI) * 2.2;
	x_delta = window->mouse_info.fv_delta[0] / window->width;
	y_delta = window->mouse_info.fv_delta[1] / window->height;
	vec3 left = x_delta * factor * x;
	vec3 up = y_delta * factor * y;

	eye += (left - up);
	target += (left - up);
}

void Camera::handle_mouse_events()
{
	if (window->buttons[0]) //左键按下
	{
		vec2 cur_pos = get_mouse_pos();
		window->mouse_info.orbit_delta = window->mouse_info.orbit_pos - cur_pos;
		window->mouse_info.orbit_pos = cur_pos;
	}

	if (window->buttons[1]) //右键按下
	{
		vec2 cur_pos = get_mouse_pos();
		window->mouse_info.fv_delta = window->mouse_info.fv_pos - cur_pos;
		window->mouse_info.fv_pos = cur_pos;
	}
	update_camera_pos();

}

void Camera::handle_key_events()
{
	float distance = (target - eye).norm();

	if (window->keys['W'])
	{
		eye += -10.0 / window->width * z * distance;
	}
	if (window->keys['S'])
	{
		eye += 0.05f * z;
	}
	if (window->keys[VK_UP] || window->keys['Q'])
	{
		eye += 0.05f * y;
		target += 0.05f * y;
	}
	if (window->keys[VK_DOWN] || window->keys['E'])
	{
		eye += -0.05f * y;
		target += -0.05f * y;
	}
	if (window->keys[VK_LEFT] || window->keys['A'])
	{
		eye += -0.05f * x;
		target += -0.05f * x;
	}
	if (window->keys[VK_RIGHT] || window->keys['D'])
	{
		eye += 0.05f * x;
		target += 0.05f * x;
	}
	if (window->keys[VK_ESCAPE])
	{
		window->is_close = 1;
	}
}

void Camera::handle_event()
{
	//mouse and keyboard events
	handle_mouse_events();

	//cac cameara axis
	z = unit_vector(eye - target);
	x = unit_vector(cross(Up, z));
	y = unit_vector(cross(z, x));

	handle_key_events();
}
