#include "../include/model.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <io.h>
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
	if (diffusemap == nullptr) {
		printf("diffusemap lose!");
		return { 1,0,1 };
	}
	return diffusemap->value(uv);
}

vec3 Model::normal(vec2 uv) const
{
	if (normalmap == nullptr) {
		printf("normalmap lose!");
		return { 0,1,0 };
	}
	return normalmap->value(uv);
}

float Model::roughness(vec2 uv) const
{
	if (roughnessmap == nullptr) {
		printf("roughnessmap lose!");
		return { 1 };
	}
	return (roughnessmap->value(uv)).norm();
}

float Model::metalness(vec2 uv) const
{
	if ( metalnessmap == nullptr) {
		printf("metalnessmap lose!");
		return {1};
	}
	return (metalnessmap->value(uv)).norm();
}

vec3 Model::emission(vec2 uv) const
{
	if (emision_map == nullptr) {
		return { 0,0,0 };
	}
	return emision_map->value(uv);
}

float Model::occlusion(vec2 uv) const
{
	if (occlusion_map == nullptr) {
		return { 1 };
	}
	return (occlusion_map->value(uv)).norm();
}

float Model::specular(vec2 uv) const
{
	if (specularmap == nullptr) {
		printf("specularmap lose!");
		return { 1 };
	}
	return (specularmap->value(uv)).norm();
}













Model::Model(const char* filename, int is_skybox)
	: is_skybox(is_skybox)
{
	std::ifstream in;
	in.open(filename, std::ifstream::in);
	if (in.fail())
	{
		printf("load model failed\n");
		return;
	}

	std::string line;
	while (!in.eof()) {
		std::getline(in, line);
		std::istringstream iss(line.c_str());
		char trash;
		if (!line.compare(0, 2, "v "))
		{
			iss >> trash; //delete 'v'
			vec3 v;
			for (int i = 0; i < 3; i++)
				iss >> v[i];

			verts.push_back(v);
		}
		else if (!line.compare(0, 3, "vn "))
		{
			iss >> trash >> trash; // delete 'vn'
			vec3 n;
			for (int i = 0; i < 3; i++)
				iss >> n[i];

			normals.push_back(n);
		}
		else if (!line.compare(0, 3, "vt "))
		{
			iss >> trash >> trash;
			vec2 uv;
			for (int i = 0; i < 2; i++)
				iss >> uv[i];

			uvs.push_back(uv);
		}
		else if (!line.compare(0, 2, "f "))
		{
			std::vector<int> f;
			int tmp[3];
			iss >> trash;
			//	delete				'/'					'/'
			while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2]) 
			{
				for (int i = 0; i < 3; i++)
					tmp[i]--; // in wavefront obj all indices start at 1, not zero

				f.push_back(tmp[0]); f.push_back(tmp[1]); f.push_back(tmp[2]);
			}
			faces.push_back(f);
		}
	}
	std::cerr << "# v# " << verts.size() << " f# " << faces.size() << " vt# " << uvs.size() << " vn# " << normals.size() << std::endl;

	/*create_map(filename);

	environment_map = NULL;
	if (is_skybox)
	{
		environment_map = new cubemap_t();
		load_cubemap(filename);
	}*/
}
//void Model::create_map(const char* filename)
//{
//	diffusemap = NULL;
//	normalmap = NULL;
//	specularmap = NULL;
//	roughnessmap = NULL;
//	metalnessmap = NULL;
//	occlusion_map = NULL;
//	emision_map = NULL;
//
//	std::string texfile(filename);
//	size_t dot = texfile.find_last_of(".");
//
//	texfile = texfile.substr(0, dot) + std::string("_diffuse.tga");
//	if (_access(texfile.data(), 0) != -1) // file exist?
//	{
//		diffusemap = new Texture();
//		load_texture(filename, "_diffuse.tga", diffusemap);
//	}
//
//	texfile = texfile.substr(0, dot) + std::string("_normal.tga");
//	if (_access(texfile.data(), 0) != -1)
//	{
//		normalmap = new TGAImage();
//		load_texture(filename, "_normal.tga", normalmap);
//	}
//
//	texfile = texfile.substr(0, dot) + std::string("_spec.tga");
//	if (_access(texfile.data(), 0) != -1)
//	{
//		specularmap = new TGAImage();
//		load_texture(filename, "_spec.tga", specularmap);
//	}
//
//	texfile = texfile.substr(0, dot) + std::string("_roughness.tga");
//	if (_access(texfile.data(), 0) != -1)
//	{
//		roughnessmap = new TGAImage();
//		load_texture(filename, "_roughness.tga", roughnessmap);
//	}
//
//	texfile = texfile.substr(0, dot) + std::string("_metalness.tga");
//	if (_access(texfile.data(), 0) != -1)
//	{
//		metalnessmap = new TGAImage();
//		load_texture(filename, "_metalness.tga", metalnessmap);
//	}
//
//	texfile = texfile.substr(0, dot) + std::string("_emission.tga");
//	if (_access(texfile.data(), 0) != -1)
//	{
//		emision_map = new TGAImage();
//		load_texture(filename, "_emission.tga", emision_map);
//	}
//
//	texfile = texfile.substr(0, dot) + std::string("_occlusion.tga");
//	if (_access(texfile.data(), 0) != -1)
//	{
//		occlusion_map = new TGAImage();
//		load_texture(filename, "_occlusion.tga", metalnessmap);
//	}
//}
//



