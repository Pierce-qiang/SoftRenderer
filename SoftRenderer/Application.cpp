#include <iostream>
#include <memory>
#include "./src/platform/win32.h"
#include "./src/include/mesh.h"
#include "./src/include/camera.h"
#include "./src/include/params.h"

using namespace std;


void clear_zbuffer(int width, int height, float* zbuffer);
void clear_framebuffer(int width, int height, unsigned char* framebuffer);
void set_color(unsigned char* framebuffer, int x, int y, Color3 color, bool inversey = true)
{
	// the origin for pixel is bottom-left, but the framebuffer index counts from top-left
	int index;
	if(inversey)
		index = ((WINDOW_HEIGHT - y - 1) * WINDOW_WIDTH + x) * 4;
	else
		index = (y * WINDOW_WIDTH + x) * 4;

	for (int i = 0; i < 3; i++)
		framebuffer[index + i] = float_clamp(static_cast<int>(color[i]*255.999),0,255) ;
}
void set_rectangle(unsigned char* framebuffer, int x1,int y1, int x2,int y2, const Color3& color) {
	for (int i = y1; i <= y2; i++) {
		for (int j = x1; j <= x2; ++j) {
			set_color(framebuffer, i, j, color);
		}
	}
}

vec4 Barycentric(float x,float y, const vec4* v) {
	float alpha, beta, gamma;
	alpha = (x * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * y + v[1].x() * v[2].y() - v[2].x() * v[1].y()) / (v[0].x() * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * v[0].y() + v[1].x() * v[2].y() - v[2].x() * v[1].y());
	beta = (x * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * y + v[2].x() * v[0].y() - v[0].x() * v[2].y()) / (v[1].x() * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * v[1].y() + v[2].x() * v[0].y() - v[0].x() * v[2].y());
	gamma = 1 - alpha - beta;
	float inv_z = alpha / v[0][2] + beta / v[1][2] + gamma / v[2][2];
	alpha /= (v[0][2] * inv_z);
	beta /= (v[1][2] * inv_z);
	gamma /= (v[2][2] * inv_z);

	return { alpha,beta,gamma,inv_z };
}

//void set_meshes(const Mesh& mesh, const Color3& color) {
//	const auto& faces = mesh.faces;
//	const auto& points = mesh.points;
//	for (int i = 0; i < faces.size(); ++i) {
//		for (int i = 0; i < 3; ++i) {
//			set
//		}
//	}
//}


int main(){
	int width = WINDOW_WIDTH, height = WINDOW_HEIGHT;
	float* zbuffer = (float*)malloc(sizeof(float) * width * height);
	unsigned char* framebuffer = (unsigned char*)malloc(sizeof(unsigned char) * 4 * width * height);

	shared_ptr<Camera> camera = make_shared<Camera>(Eye, Target, Up, Aspect, FOV, Nearplane, Farplane);
	
	// initialize window
	window_init(width, height, "softRenderer");

	// render loop
	// -----------
	int num_frames = 0;
	float print_time = platform_get_time();

	vec3 mytriangle[3] = { {-0.5,1,0},{0.5,1,0},{0,2,0} };
	vec3 colors[3] = { {1,0,0},{0,1,0},{0,0,1} };
	while (!window->is_close)
	{
		//clear buffer
		clear_zbuffer(width, height, zbuffer);
		clear_framebuffer(width, height, framebuffer);

		//handle events
		camera->handle_event();

		
		//draw
		vec4 outTriangle[3];
		int x1 = INT_MAX, y1 = INT_MAX, x2 = 0, y2 = 0;
		for (int i = 0; i < 3; ++i) {
			mat4 vp = camera->get_VPMatrix();
			outTriangle[i] = vp * to_vec4(mytriangle[i],1);
			outTriangle[i] /= outTriangle[i][3];
			outTriangle[i][0] = (outTriangle[i][0] + 1) / 2 * width;
			outTriangle[i][1] = (outTriangle[i][1] + 1) / 2 * height;
			x1 = min(x1, (int)outTriangle[i][0]);
			x2 = max(x2,(int)outTriangle[i][0]);
			y1 = min(y1,(int)outTriangle[i][1]);
			y2 = max(y2,(int)outTriangle[i][1]);
		}
		x1 = max(x1, 0), x2 = min(x2, width - 1);
		y1 = max(y1, 0), y2 = min(y2, height - 1);
		//barycentric
		for (int i = y1; i <= y2; ++i) {
			for (int j = x1; j <= x2; ++j) {
				auto bari = Barycentric(j, i, outTriangle);
				//cout << bari << endl;
				if (bari[0] < -EPSILON || bari[1] < -EPSILON || bari[2] < -EPSILON) continue;
				Color3 color = bari[0] * colors[0] + bari[1] * colors[1] + bari[2] * colors[2];
				set_color(framebuffer, j, i, color);
			}
		}
		
		


		//calculate frames and time
		++num_frames;
		float cur_time = platform_get_time();
		if (cur_time - print_time >= 1) {
			int sum_millis = static_cast<int>((cur_time - print_time) * 1000);
			int avg_millis = sum_millis / num_frames;
			printf("fps: %3d, avg: %3d ms\n", num_frames, avg_millis);
			num_frames = 0;
			print_time = cur_time;
		}
		//reset mouse info
		window->mouse_info.wheel_delta = 0;
		window->mouse_info.orbit_delta = vec2(0, 0);
		window->mouse_info.fv_delta = vec2(0, 0);

		//send framebuffer to window
		window_draw(framebuffer);
		msg_dispatch();

	}

	//free mem
	free(zbuffer);
	free(framebuffer);
	window_destroy();


	system("pause");
	return 0;
}


void clear_zbuffer(int width, int height, float* zbuffer) {
	for (int i = 0; i < width * height; ++i) {
		zbuffer[i] = 1.0;
	}
}
void clear_framebuffer(int width, int height, unsigned char* framebuffer) {
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			int ind = (i * width + j) * 4;
			framebuffer[ind] = 56;		//r
			framebuffer[ind + 1] = 56;  //g
			framebuffer[ind + 2] = 80;	//b
		}
	}
}

