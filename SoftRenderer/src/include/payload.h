#pragma once
#include "./maths.h"
class Camera;
class Model;



struct payload_t
{
	Camera* camera;
	Model* model;

	//vertex attribute for output
	vec3 normal_attri[3];
	vec2 uv_attri[3];
	vec3 worldcoord_attri[3];
	vec4 clipcoord_attri[3];



};
