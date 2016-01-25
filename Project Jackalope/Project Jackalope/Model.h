#pragma once

#include <d3d11.h>
#include <vector>
//#include "SimpleMath.h"

//using namespace DirectX::SimpleMath;

struct Vertex
{
	float x, y, z, r, g, b, w;
};

class Model
{
private:

public:
	std::vector<Vertex> mVertices;

	Model();
	~Model();

	void LoadModel(std::vector<Vertex> &vertices);
};

