#include "../include/CubeMap.h"

CubeMap::CubeMap(const char* file)
{
	std::string fileroot(file),filepath;
	filepath = fileroot + "right.jpg";
	textures.push_back(new Texture(filepath.c_str()));
	filepath = fileroot + "left.jpg";
	textures.push_back(new Texture(filepath.c_str()));
	filepath = fileroot + "top.jpg";
	textures.push_back(new Texture(filepath.c_str()));
	filepath = fileroot + "bottom.jpg";
	textures.push_back(new Texture(filepath.c_str()));
	filepath = fileroot + "back.jpg";
	textures.push_back(new Texture(filepath.c_str()));
	filepath = fileroot + "front.jpg";
	textures.push_back(new Texture(filepath.c_str()));
}

CubeMap::~CubeMap()
{
	for (auto i : textures) {
		if (i != nullptr) delete i;
	}
}

vec3 CubeMap::value(vec3 dir) const
{
	int maxind = 0;
	float maxval = abs(dir[0]);
	if (abs(dir[1]) > maxval) {
		maxind = 1; maxval = abs(dir[1]);
	}
	if (abs(dir[2]) > maxval) {
		maxind = 2; maxval = abs(dir[2]);
	}
	int ind = maxind * 2 + (dir[maxind] < 0 ? 1 : 0);

	dir = dir / maxval;
	dir = dir * 0.5f + vec3(0.5,0.5,0.5);

	vec2 uv;
	switch (ind)
	{
	case 0:
		uv = { dir[2], dir[1] };
		break;
	case 1:
		uv = { 1.0f-dir[2], dir[1] };
		break;
	case 2:
		uv = { dir[0], dir[2] };
		break;
	case 3:
		uv = { dir[0], 1.0f-dir[2] };
		break;
	case 4:
		uv = { 1-dir[0], dir[1] };
		break;
	case 5:
		uv = { dir[0], dir[1] };
		break;
	default:
		break;
	}
	return textures[ind]->value(uv);
}
