#pragma once
#include "./payload.h"
#include <memory>
#include <iostream>


//��Ⱦ���ӿ�
class IShader
{
public:
	//��Զ���
	virtual void vertex_shader(int nface, int nvertex)= 0;
	//������أ���payload�Ѿ��ж�Ӧ������������
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