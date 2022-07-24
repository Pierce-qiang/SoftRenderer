#pragma once
#include "./maths.h"
#include <vector>
struct Face {
	int a, b, c;
};

class Mesh
{
public:
	Mesh();
	~Mesh();

public:
	std::vector<Point3> points;
	std::vector<Face> faces;
};