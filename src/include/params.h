#pragma once
#include "./maths.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

//camera
const vec3 Eye(0.0, 1.0, 5.0);
const vec3 Up(0, 1, 0);
const vec3 Target(0, 1, 0);
const float Aspect = (float)WINDOW_WIDTH / WINDOW_HEIGHT;
const float FOV = 60;
const float Nearplane = 0.1;
const float Farplane = 500;