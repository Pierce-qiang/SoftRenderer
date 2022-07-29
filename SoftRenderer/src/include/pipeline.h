#pragma once
#include "./Shader.h"
#include "../platform/win32.h"
#include "./params.h"
#include "./SkyBox.h"

void draw_triangles(unsigned char* framebuffer, float* zbuffer, IShader* shader,int nface);

void draw_skybox(unsigned char* framebuffer, float* zbuffer, SkyBox* skybox, Camera* camera);

