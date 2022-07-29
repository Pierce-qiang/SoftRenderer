#pragma once
#include "./maths.h"

class Camera
{
public:
	Camera(Point3 e,Point3 tar, vec3 up,float asp,float vfovAngle,float n,float f);
	~Camera();
	mat4 get_Perspective()const;
	mat4 get_ViewMatrix()const;
	mat4 get_VPMatrix()const;
	void handle_event();
	vec3 generateDir(vec2 uv)const;

public:
	Point3 eye;
	Point3 target;
	vec3 Up;
	vec3 x, y, z;
	float aspect,vfov,nearplane,farplane;
private:
	void update_camera_pos();
	void handle_mouse_events();
	void handle_key_events();
};