#pragma once
#include "./Shader.h"
#include "../platform/win32.h"
#include "./params.h"

void draw_triangles(unsigned char* framebuffer, float* zbuffer, IShader* shader,int nface);


