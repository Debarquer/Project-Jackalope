#pragma once

#include <d3d11.h>
#include <vector>
#include <string>
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
	Model(Model& other);

	std::string material;
	std::vector<Vertex> mVertices;

	Model();
	~Model();

	void LoadModel(Model& other);
};

