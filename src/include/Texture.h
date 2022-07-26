#pragma once
#include "./maths.h"
class Texture
{
public:
	const static int bytes_per_pixel = 4;

	Texture(const char* filename);
	~Texture();
	Color3 value(const vec2& uv)const;
	

private:
	unsigned char* data;
	int width, height;
	int bytes_per_scanline;
};