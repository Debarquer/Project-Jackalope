#pragma once

#include <d3d11.h>
#include <vector>
#include <string>
#include "SimpleMath.h"
#include "WICTextureLoader.h"


//using namespace DirectX::SimpleMath;

class Model
{
private:

public:
	//Model(Model& other);

	/*
	float4 Position : POSITION;
	float2 TexCoord : TEXCOORDS0;
	float3 Normal : NORMAL;
	float4 Tangent : TANGENT;
	*/
	struct Vertex
	{
		float 
			pX, pY, pZ, pW,
			u, v,
			nX, nY, nZ,
			tX, tY, tZ, tW;
	};

	std::string materialFile;
	std::string material;
	std::vector<Vertex> mVertices;
	ID3D11ShaderResourceView* pSRV;

	Model();
	~Model();
	void LoadModel(Model& other);
	static DirectX::XMFLOAT3 cross(DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2);
	static Model LoadTextFile(std::string filename, bool &failed, ID3D11Device* dev, ID3D11DeviceContext* devcon);
};

