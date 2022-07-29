#pragma once
#include "./maths.h"
#include "./camera.h"
#include "./model.h"


//attach on shader
struct payload_t
{
	//matrix
	mat4 model_matrix;				//M
	mat4 model_matrix_inv_trans;		// M^(-T)
	mat4 camera_view_matrix;		//V
	mat4 camera_perp_matrix;		//P
	mat4 mvp_matrix;				//MVP
	//for shadow map
	mat4 light_view_matrix;
	mat4 light_perp_matrix;


	std::shared_ptr<Camera> camera;
	std::shared_ptr<Model> model;

	//vertex attribute for vsout -> clip -> fs_in triangle
	vec3 normal_attri[3];
	vec2 uv_attri[3];
	vec3 worldcoord_attri[3];
	vec4 clipcoord_attri[3]; // after MVP transform

	//for homogeneous clipping
	vec3 in_normal[MAX_VERTEX];
	vec2 in_uv[MAX_VERTEX];
	vec3 in_worldcoord[MAX_VERTEX];
	vec4 in_clipcoord[MAX_VERTEX];
	vec3 out_normal[MAX_VERTEX];
	vec2 out_uv[MAX_VERTEX];
	vec3 out_worldcoord[MAX_VERTEX];
	vec4 out_clipcoord[MAX_VERTEX];

};
