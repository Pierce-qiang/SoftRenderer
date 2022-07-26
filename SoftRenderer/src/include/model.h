#pragma once
#include "./maths.h"
#include <vector>
#include "./Texture.h"

class Model
{
public:
	Model(std::vector<vec3>& v, std::vector<vec2>& uv, std::vector<vec3>& normal, std::vector<std::vector<int> > face) : verts(v), uvs(uv), normals(normal), faces(face){}
	Model(const char* filename, int is_skybox = 0, int is_from_mmd = 0);
	~Model(){}


	int nverts()const;
	int nfaces()const;
	vec3 vert(int iface, int nthvert)const;
	vec2 uv(int iface, int nthvert)const;
	vec3 normal(int iface, int nthvert)const;

	vec3 vert(int i)const;
	//return [vert_id1,uv_id1,normal_id1 , vert_id2,uv_id2,normal_id2 ,...]
	std::vector<int> face(int idx)const;

	// lut
	Color3 diffuse(const vec2& uv)const;
	vec3 normal(vec2 uv)const;
	float roughness(vec2 uv)const;
	float metalness(vec2 uv)const;
	vec3 emission(vec2 uv)const;
	float occlusion(vec2 uv)const;
	float specular(vec2 uv)const;


public:
	std::vector<vec3> verts;
	std::vector<std::vector<int> > faces; // attention, this Vec3i means vertex/uv/normal
	std::vector<vec3> normals;
	std::vector<vec2> uvs;

	bool is_skybox;


	//map
	int is_from_mmd;
	Texture* diffusemap = nullptr;
	Texture* normalmap = nullptr;
	Texture* specularmap;
	Texture* roughnessmap;
	Texture* metalnessmap;
	Texture* occlusion_map;
	Texture* emision_map;

private:
	void create_map(const char* filename);
};