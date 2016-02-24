#pragma once
#include <Windows.h>
#include "Model.h"

using namespace DirectX;

class HeightMap
{
private:
	std::vector <Model::Vertex> v;
	std::vector<DWORD> indices;

public:
	int NumFaces;
	int cols;
	int rows;
	int texUindex, texVindex;
	ID3D11ShaderResourceView* pSRV;
	ID3D11ShaderResourceView* bmpMapView;

	struct HeightMapInfo {
		int terrainWidth;
		int terrainHeight;
		DWORD numVertices = 0;
		XMFLOAT3 *heightMap;
	};

	std::vector<Model::Vertex>& getV();
	std::vector<DWORD>& getIndices();
	HeightMap();
	~HeightMap();
	void CreateGrid(HeightMapInfo &hminfo, std::vector <Model::Vertex>& v, std::vector <DWORD>& indices);
	bool HeightMapLoad(char* filename, HeightMapInfo &hminfo, ID3D11Device* dev);
	void calculateNormals();
};
