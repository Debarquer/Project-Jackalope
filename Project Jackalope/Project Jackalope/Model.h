#pragma once

#include <d3d11.h>
#include <vector>
#include <string>
#include <sstream>
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

	struct Material
	{
		std::string materialName;
		int illum;
		DirectX::XMFLOAT3 Ka;
		DirectX::XMFLOAT3 Kd;
		DirectX::XMFLOAT3 Ks;
		DirectX::XMFLOAT3 Na;
		float Ns;
		std::string texFileName;
	};

	std::string mMaterialFile;
	std::string mMaterialName;
	std::vector<Vertex> mVertices;
	ID3D11ShaderResourceView* mPSRV;
	Material mMaterial;

	Model();
	~Model();
	void LoadModel(Model& other);
	static DirectX::XMFLOAT3 cross(DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2);
	static Model LoadTextFile(std::string filename, bool &failed, ID3D11Device* dev);
	static void LoadMaterialFile(Model& model, ID3D11Device* dev);
};

