#include "../include/Shader.h"
#include <cassert>

void PhongShader::vertex_shader(int nfaces, int nvertex)
{
	vec4 temp_vert = to_vec4(payload.model->vert(nfaces, nvertex), 1.0f);
	vec4 temp_normal = to_vec4(payload.model->normal(nfaces, nvertex), 1.0f);

	payload.uv_attri[nvertex] = payload.model->uv(nfaces, nvertex);
	payload.in_uv[nvertex] = payload.uv_attri[nvertex];
	payload.clipcoord_attri[nvertex] = payload.mvp_matrix * temp_vert;
	payload.in_clipcoord[nvertex] = payload.clipcoord_attri[nvertex];

	temp_vert = payload.model_matrix * temp_vert;
	temp_normal = payload.model_matrix_inv_trans * temp_normal;
	assert(abs(temp_normal[3]) < EPSILON);
	// only model matrix can change normal vector in world space ( Normal Matrix: tranverse(inverse(model)) )
	for (int i = 0; i < 3; i++)
	{
		payload.worldcoord_attri[nvertex][i] = temp_vert[i];
		payload.in_worldcoord[nvertex][i] = temp_vert[i];
		payload.normal_attri[nvertex][i] = temp_normal[i];
		payload.in_normal[nvertex][i] = temp_normal[i];
	}
}

vec3 PhongShader::fragment_shader(float alpha, float beta, float gamma)
{
	vec4* clip_coords = payload.clipcoord_attri;
	vec3* world_coords = payload.worldcoord_attri;
	vec3* normals = payload.normal_attri;
	vec2* uvs = payload.uv_attri;

	vec2 uv = alpha * uvs[0] + beta * uvs[1] + gamma * uvs[2];
	vec3 normal  = alpha * normals[0] + beta * normals[1] + gamma * normals[2];
	vec3 world_coord = alpha * world_coords[0] + beta * world_coords[1] + gamma * world_coords[2];

	return payload.model->diffuse(uv);
}