#pragma once

#include <d3d11.h>
#include <vector>
//#include "SimpleMath.h"

//using namespace DirectX::SimpleMath;

struct Vertex
{
	float pX, pY, pZ, r, g, b, w;
	float nX, nY, nZ, u, v;
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

