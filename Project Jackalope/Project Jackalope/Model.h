#pragma once

#include <d3d11.h>
#include <vector>
#include <string>
#include "SimpleMath.h"

//using namespace DirectX::SimpleMath;

class Model
{
private:

public:
	//Model(Model& other);

	struct Vertex
	{
		float pX, pY, pZ, r, g, b, w,
			nX, nY, nZ, u, v;
	};

	std::string materialFile;
	std::string material;
	std::vector<Vertex> mVertices;

	Model();
	~Model();

	void LoadModel(Model& other);
	static Model LoadTextFile(std::string filename, bool &failed);
};

