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
	ID3D11Texture2D* texture;

	Model();
	~Model();

	void LoadModel(Model& other);
	DirectX::XMFLOAT3 cross(DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2);
	static Model LoadTextFile(std::string filename, bool &failed);
};

