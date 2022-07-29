#pragma once
#include "./Texture.h"
#include "./maths.h"
#include <vector>

class CubeMap
{
public:
	CubeMap(const char* file);
	~CubeMap();
	vec3 value(vec3 dir)const;

private:
	std::vector<Texture*> textures; // +x  -x  +y -y  +z -z
};