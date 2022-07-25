#include <iostream>
#include <memory>
#include "./src/platform/win32.h"
#include "./src/include/mesh.h"
#include "./src/include/camera.h"
#include "./src/include/params.h"
#include "./src/include/pipeline.h"

using namespace std;


void clear_zbuffer(int width, int height, float* zbuffer);
void clear_framebuffer(int width, int height, unsigned char* framebuffer);


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

	std::vector<vec3> myverts = {
		{0.5f,  0.5f, 0.0f},  // top right
		{ 0.5f, -0.5f, 0.0f},  // bottom right
		{-0.5f, -0.5f, 0.0f},  // bottom left
		{-0.5f,  0.5f, 0.0f}   // top left 
	};
	std::vector<vec2> myuv = {
		{1,1},
		{1,0},
		{0,0},
		{0,1}
	};

	std::vector<vec3> mynormal = {
		{1,0,0}
	};

	std::vector<std::vector<int>> myface = {
		{0,0,0, 3,3,0, 1,1,0},	// first triangle
		{1,1,0, 3,3,0, 2,2,0}  // second triangle
	};

	shared_ptr<Model> model = make_shared<Model>(myverts, myuv, mynormal, myface);
	model->diffusemap = make_shared<Texture>("E:/c++/SoftRenderer/SoftRenderer/src/resource/awesomeface.png");
	shared_ptr<IShader> shader = make_shared<PhongShader>();
	shader->payload.camera = camera;
	shader->payload.model = model;

	while (!window->is_close)
	{
		//clear buffer
		clear_zbuffer(width, height, zbuffer);
		clear_framebuffer(width, height, framebuffer);

		//handle events
		camera->handle_event();

		//update matrix
		shader->payload.camera_perp_matrix = shader->payload.camera->get_Perspective();
		shader->payload.mvp_matrix = shader->payload.camera->get_VPMatrix();
		
		
		//draw
		for(int i = 0; i < model->nfaces(); i++)
		{
			draw_triangles(framebuffer, zbuffer, shader.get(), i);
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

