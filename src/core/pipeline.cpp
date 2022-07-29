#include "../include/pipeline.h"
//#include <mutex>
//
//std::mutex frameMutex;
//std::mutex zbufferMutex;

static void set_color(unsigned char* framebuffer, int x, int y, Color3 color, bool inversey = true)
{
	// the origin for pixel is bottom-left, but the framebuffer index counts from top-left
	int index;
	if (inversey)
		index = ((WINDOW_HEIGHT - y - 1) * WINDOW_WIDTH + x) * 4;
	else
		index = (y * WINDOW_WIDTH + x) * 4;
	{
		//std::lock_guard<std::mutex> lcg(frameMutex);
		for (int i = 0; i < 3; i++)
			framebuffer[index + i] = float_clamp(static_cast<int>(color[i] * 255.999), 0, 255);

	}
}


static int get_index(int x, int y,int width, int height)
{
	return (height - y - 1) * width + x;
}

static bool is_back_facing(vec3* screen_pos) {
	float x1 = screen_pos[1][0] - screen_pos[0][0];
	float y1 = screen_pos[1][1] - screen_pos[0][1];
	float x2 = screen_pos[2][0] - screen_pos[0][0];
	float y2 = screen_pos[2][1] - screen_pos[0][1];
	float signed_area = x1 * y2 - y1 * x2;
	return signed_area <= 0;
}
static vec4 Barycentric(float x, float y, const vec3* v) {
	float alpha, beta, gamma;
	alpha = (x * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * y + v[1].x() * v[2].y() - v[2].x() * v[1].y()) / (v[0].x() * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * v[0].y() + v[1].x() * v[2].y() - v[2].x() * v[1].y());
	beta = (x * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * y + v[2].x() * v[0].y() - v[0].x() * v[2].y()) / (v[1].x() * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * v[1].y() + v[2].x() * v[0].y() - v[0].x() * v[2].y());
	gamma = 1 - alpha - beta;
#ifdef TRUEINTER
	return { alpha,beta,gamma, 1.0 };
#else
	float z = 1.0 / (alpha / v[0][2] + beta / v[1][2] + gamma / v[2][2]);
	alpha *= z / v[0][2];
	beta *= z / v[1][2];
	gamma *= z / v[2][2];

	return { alpha,beta,gamma, z };
#endif 
}

typedef enum
{
	W_PLANE,
	X_RIGHT,
	X_LEFT,
	Y_TOP,
	Y_BOTTOM,
#ifdef INVZ
	Z_NEAR,
	Z_FAR
#else
	Z_FAR,
	Z_NEAR
#endif
} clip_plane;

static bool is_inside_plane(clip_plane c_plane, vec4 vertex)
{
#ifdef INVZ
	switch (c_plane)
	{
	case W_PLANE:
		return vertex.w() <= -Nearplane;
	case X_RIGHT:
		return vertex.x() >= vertex.w();
	case X_LEFT:
		return vertex.x() <= -vertex.w();
	case Y_TOP:
		return vertex.y() >= vertex.w();
	case Y_BOTTOM:
		return vertex.y() <= -vertex.w();
	case Z_NEAR:
		return vertex.z() >= vertex.w();
	case Z_FAR:
		return vertex.z() <= -vertex.w();
	default:
		return 0;
	}

#else
	switch (c_plane)
	{
	case W_PLANE:
		return vertex.w() >= Nearplane; //avoid divide zero  W = EPSILON
	case X_RIGHT:
		return vertex.x() <= vertex.w();
	case X_LEFT:
		return vertex.x() >= -vertex.w();
	case Y_TOP:
		return vertex.y() <= vertex.w();
	case Y_BOTTOM:
		return vertex.y() >= -vertex.w();
	case Z_FAR:
		return vertex.z() <= vertex.w();
	case Z_NEAR:
		return vertex.z() >= -vertex.w();
	default:
		return false;
	}
#endif 
}
// for the deduction of intersection ratio
// refer to: https://fabiensanglard.net/polygon_codec/clippingdocument/Clipping.pdf
static float get_intersect_ratio(vec4 prev, vec4 curv, clip_plane c_plane)
{
#ifdef INVZ
	switch (c_plane)
	{
	case W_PLANE:
		return (prev.w() + Nearplane) / (prev.w() - curv.w());
	case X_RIGHT:
		return (prev.w() - prev.x()) / ((prev.w() - prev.x()) - (curv.w() - curv.x()));
	case X_LEFT:
		return (prev.w() + prev.x()) / ((prev.w() + prev.x()) - (curv.w() + curv.x()));
	case Y_TOP:
		return (prev.w() - prev.y()) / ((prev.w() - prev.y()) - (curv.w() - curv.y()));
	case Y_BOTTOM:
		return (prev.w() + prev.y()) / ((prev.w() + prev.y()) - (curv.w() + curv.y()));
	case Z_NEAR:
		return (prev.w() - prev.z()) / ((prev.w() - prev.z()) - (curv.w() - curv.z()));
	case Z_FAR:
		return (prev.w() + prev.z()) / ((prev.w() + prev.z()) - (curv.w() + curv.z()));
	default:
		return 0;
	}
#else
	switch (c_plane)
	{
	case W_PLANE:
		return (prev.w() - Nearplane) / (prev.w() - curv.w());
	case X_RIGHT:
		return (prev.w() - prev.x()) / ((prev.w() - prev.x()) - (curv.w() - curv.x()));
	case X_LEFT:
		return (prev.w() + prev.x()) / ((prev.w() + prev.x()) - (curv.w() + curv.x()));
	case Y_TOP:
		return (prev.w() - prev.y()) / ((prev.w() - prev.y()) - (curv.w() - curv.y()));
	case Y_BOTTOM:
		return (prev.w() + prev.y()) / ((prev.w() + prev.y()) - (curv.w() + curv.y()));
	case Z_FAR:
		return (prev.w() - prev.z()) / ((prev.w() - prev.z()) - (curv.w() - curv.z()));
	case Z_NEAR:
		return (prev.w() + prev.z()) / ((prev.w() + prev.z()) - (curv.w() + curv.z()));
	default:
		return 0;
	}
#endif // INVZ


}

static int clip_with_plane(clip_plane c_plane, int num_vert, payload_t& payload)
{
	int out_vert_num = 0;
	int previous_index, current_index;
	int is_odd = (c_plane + 1) % 2; // w:1 posx:0,negx:1 ...  negz:1

	// set the right in and out datas
	vec4* in_clipcoord = is_odd ? payload.in_clipcoord : payload.out_clipcoord;
	vec3* in_worldcoord = is_odd ? payload.in_worldcoord : payload.out_worldcoord;
	vec3* in_normal = is_odd ? payload.in_normal : payload.out_normal;
	vec2* in_uv = is_odd ? payload.in_uv : payload.out_uv;
	vec4* out_clipcoord = is_odd ? payload.out_clipcoord : payload.in_clipcoord;
	vec3* out_worldcoord = is_odd ? payload.out_worldcoord : payload.in_worldcoord;
	vec3* out_normal = is_odd ? payload.out_normal : payload.in_normal;
	vec2* out_uv = is_odd ? payload.out_uv : payload.in_uv;

	// tranverse all the edges from first vertex
	for (int i = 0; i < num_vert; i++)
	{
		current_index = i;
		previous_index = (i - 1 + num_vert) % num_vert;
		vec4 cur_vertex = in_clipcoord[current_index];
		vec4 pre_vertex = in_clipcoord[previous_index];

		int is_cur_inside = is_inside_plane(c_plane, cur_vertex);
		int is_pre_inside = is_inside_plane(c_plane, pre_vertex);
		if (is_cur_inside != is_pre_inside)
		{
			float ratio = get_intersect_ratio(pre_vertex, cur_vertex, c_plane);

			out_clipcoord[out_vert_num] = vec4_lerp(pre_vertex, cur_vertex, ratio);
			out_worldcoord[out_vert_num] = vec3_lerp(in_worldcoord[previous_index], in_worldcoord[current_index], ratio);
			out_normal[out_vert_num] = vec3_lerp(in_normal[previous_index], in_normal[current_index], ratio);
			out_uv[out_vert_num] = vec2_lerp(in_uv[previous_index], in_uv[current_index], ratio);

			out_vert_num++;
		}

		if (is_cur_inside)
		{
			out_clipcoord[out_vert_num] = cur_vertex;
			out_worldcoord[out_vert_num] = in_worldcoord[current_index];
			out_normal[out_vert_num] = in_normal[current_index];
			out_uv[out_vert_num] = in_uv[current_index];

			out_vert_num++;
		}
	}

	return out_vert_num;
}

static int homo_clipping(payload_t& payload)
{
	int num_vertex = 3;
	num_vertex = clip_with_plane(W_PLANE, num_vertex, payload);
	num_vertex = clip_with_plane(X_RIGHT, num_vertex, payload);
	num_vertex = clip_with_plane(X_LEFT, num_vertex, payload);
	num_vertex = clip_with_plane(Y_TOP, num_vertex, payload);
	num_vertex = clip_with_plane(Y_BOTTOM, num_vertex, payload);
#ifdef INVZ
	num_vertex = clip_with_plane(Z_NEAR, num_vertex, payload);
	num_vertex = clip_with_plane(Z_FAR, num_vertex, payload);
#else

	num_vertex = clip_with_plane(Z_FAR, num_vertex, payload);
	num_vertex = clip_with_plane(Z_NEAR, num_vertex, payload);
#endif // INVZ

	return num_vertex;
}





static void rasterizeSingleThread(vec4* clipcoord_attri, unsigned char* framebuffer, float* zbuffer, IShader* shader) {
	vec3 ndc_pos[3], screen_pos[3];
	Color3 color;
	int width = window->width, height = window->height;

	// simple homogeneous division
	for (int i = 0; i < 3; i++)
	{
		ndc_pos[i][0] = clipcoord_attri[i][0] / clipcoord_attri[i].w();
		ndc_pos[i][1] = clipcoord_attri[i][1] / clipcoord_attri[i].w();
		ndc_pos[i][2] = clipcoord_attri[i][2] / clipcoord_attri[i].w();
	}

#ifdef CLIPPING

#else
	// simple clipping to test whether cliping method work
	for (int i = 0; i < 3; i++)
	{	// consider float error, otherwise, it will flicker
		if (ndc_pos[i][0] < -1 - EPSILON || ndc_pos[i][0]>1 + EPSILON) return;
		if (ndc_pos[i][1] < -1 - EPSILON || ndc_pos[i][1]>1 + EPSILON) return;
		if (ndc_pos[i][2] < -1 - EPSILON || ndc_pos[i][2]>1 + EPSILON) return;
	}
#endif // CLIPPING


	// viewport transformation
	for (int i = 0; i < 3; i++)
	{
		screen_pos[i][0] = 0.5 * (ndc_pos[i][0] + 1.0) * (width - 1);
		screen_pos[i][1] = 0.5 * (ndc_pos[i][1] + 1.0) * (height - 1);
		//screen_pos[i][2] = ndc_pos[i][2]*0.5+0.5; // 0~1
#ifdef INVZ
#ifdef ViewZ
		screen_pos[i][2] = -clipcoord_attri[i][3];
#else
		screen_pos[i][2] = ndc_pos[i][2] * 0.5 - 0.5;
		screen_pos[i][2] *= -1;
#endif // ViewZ

#else

	#ifdef ViewZ
			screen_pos[i][2] = clipcoord_attri[i][3];
	#else
			screen_pos[i][2] = ndc_pos[i][2] * 0.5 + 0.5;
	#endif // ViewZ

#endif // INVZ
	}
	
	if (is_back_facing(ndc_pos))
		return;
	// bounding box
	float xmin = 10000, xmax = -10000, ymin = 10000, ymax = -10000;
	for (int i = 0; i < 3; i++)
	{
		xmin = float_min(xmin, screen_pos[i][0]);
		xmax = float_max(xmax, screen_pos[i][0]);
		ymin = float_min(ymin, screen_pos[i][1]);
		ymax = float_max(ymax, screen_pos[i][1]);
	}

	int xs = max((int)xmin, 0), xe = min((int)xmax, width - 1);
	int ys = max((int)ymin, 0), ye = min((int)ymax, height - 1);
	for (int x = xs; x <= xe; ++x) {
		for (int y = ys; y <= ye; ++y) {
			auto bari = Barycentric(float(x + 0.5), float(y + 0.5), screen_pos);
			float alpha = bari[0], beta = bari[1], gamma = bari[2];
			// add EPSILON can avoid some aliasing
			if (alpha < -EPSILON || beta < -EPSILON || gamma < -EPSILON) continue;

#ifdef TRUEINTER
			float normalizer = 1.0 / (alpha / clipcoord_attri[0].w() + beta / clipcoord_attri[1].w() + gamma / clipcoord_attri[2].w());

			float depth = (alpha * screen_pos[0].z() / clipcoord_attri[0].w() + beta * screen_pos[1].z() / clipcoord_attri[1].w() +
				gamma * screen_pos[2].z() / clipcoord_attri[2].w()) * normalizer;

#else
			float depth = bari[3];
#endif // 

			

			int ind = get_index(x, y, width, height);

			{
				//std::lock_guard<std::mutex> lck(zbufferMutex);
				if (zbuffer[ind] < depth) continue;
				zbuffer[ind] = depth;

			}

			auto color = shader->fragment_shader(alpha, beta, gamma);
			set_color(framebuffer, x, y, color);
		}
	}

}
static void transform_attri(payload_t& payload, int index0, int index1, int index2) {
	payload.clipcoord_attri[0] = payload.out_clipcoord[index0];
	payload.clipcoord_attri[1] = payload.out_clipcoord[index1];
	payload.clipcoord_attri[2] = payload.out_clipcoord[index2];
	payload.worldcoord_attri[0] = payload.out_worldcoord[index0];
	payload.worldcoord_attri[1] = payload.out_worldcoord[index1];
	payload.worldcoord_attri[2] = payload.out_worldcoord[index2];
	payload.normal_attri[0] = payload.out_normal[index0];
	payload.normal_attri[1] = payload.out_normal[index1];
	payload.normal_attri[2] = payload.out_normal[index2];
	payload.uv_attri[0] = payload.out_uv[index0];
	payload.uv_attri[1] = payload.out_uv[index1];
	payload.uv_attri[2] = payload.out_uv[index2];
}



void draw_triangles(unsigned char* framebuffer, float* zbuffer, IShader* shader, int nface)
{
	for (int i = 0; i < 3; ++i) {
		shader->vertex_shader(nface, i);
	}

#ifdef CLIPPING
	 // CLIPPING

	// homo_clipping
	int num_vertex = homo_clipping(shader->payload);


	// triangle assembly and reaterize
	for (int i = 0; i < num_vertex - 2; i++) {
		int index0 = 0;
		int index1 = i + 1;
		int index2 = i + 2;
		// transform data to real vertex attri
		transform_attri(shader->payload, index0, index1, index2);

		rasterizeSingleThread(shader->payload.clipcoord_attri, framebuffer, zbuffer, shader);
	}
#else
	rasterizeSingleThread(shader->payload.clipcoord_attri, framebuffer, zbuffer, shader);
#endif
}

void draw_skybox(unsigned char* framebuffer, float* zbuffer, SkyBox* skybox, Camera* camera)
{
	int width = window->width;
	int height = window->height;
	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {
			int ind = get_index(x, y, width, height);
#ifdef ViewZ
			if (zbuffer[ind] != Farplane) continue;
#else
			if (zbuffer[ind] != 1.0) continue;
#endif // ViewZ
			vec2 uv = { (x+0.5f) / (width), (y+0.5f) / (height) };

			vec3 dir = camera->generateDir(uv);

			Color3 color = skybox->sample(dir);

			set_color(framebuffer, x, y, color );
		}
	}
}
