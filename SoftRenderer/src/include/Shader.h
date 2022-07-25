#pragma once
#include "./payload.h"
#include <memory>
#include <iostream>


//渲染器接口
class IShader
{
public:
	//针对顶点
	virtual void vertex_shader(int nface, int nvertex)= 0;
	//针对像素，而payload已经有对应的三角形数据
	virtual vec3 fragment_shader(float alpha, float beta, float gamma) = 0;
public:
	payload_t payload;
};

class PhongShader :public IShader
{
public:
	void vertex_shader(int nfaces, int nvertex)override;
	vec3 fragment_shader(float alpha, float beta, float gamma)override;

};

class PBRShader :public IShader
{
public:
	void vertex_shader(int nfaces, int nvertex)override;
	vec3 fragment_shader(float alpha, float beta, float gamma)override;
	vec3 direct_fragment_shader(float alpha, float beta, float gamma);
};

class SkyboxShader :public IShader
{
public:
	void vertex_shader(int nfaces, int nvertex)override;
	vec3 fragment_shader(float alpha, float beta, float gamma)override;
};