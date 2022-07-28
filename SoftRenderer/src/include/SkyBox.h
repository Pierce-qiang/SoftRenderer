#pragma once
#include "./CubeMap.h"

class SkyBox
{
public:
	SkyBox(CubeMap* cube):cubemap(cube){}
	~SkyBox() { if (cubemap != nullptr) delete cubemap; }
	vec3 sample(const vec3& dir)const { return cubemap->value(dir); }
private:
	CubeMap* cubemap;
};