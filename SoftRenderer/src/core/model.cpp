#include "../include/model.h"
#include <cassert>

int Model::nverts() const
{
	return verts.size();
}

int Model::nfaces() const
{
	return faces.size();
}

vec3 Model::vert(int iface, int nthvert) const
{
	return verts[faces[iface][nthvert * 3]];
}

vec2 Model::uv(int iface, int nthvert) const
{
	return uvs[faces[iface][nthvert * 3+1]];
}

vec3 Model::normal(int iface, int nthvert) const
{
	return normals[faces[iface][nthvert*3+2]];
}
vec3 Model::vert(int i) const
{
	return verts[i];
}

std::vector<int> Model::face(int idx) const
{
	return faces[idx];
}

Color3 Model::diffuse(const vec2& uv) const
{
	assert((diffusemap) != nullptr);
	return diffusemap->value(uv);
}


