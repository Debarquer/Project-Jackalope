#pragma once
#include <Windows.h>
#include "Model.h"

using namespace DirectX;

class HeightMap
{
private:
	std::vector <Model::Vertex> v;

public:
	int NumFaces;
	int NumVertices;
	int cols;
	int rows;

	struct HeightMapInfo {
		std::vector<DWORD> indices;
		int terrainWidth;
		int terrainHeight;
		DWORD numVertices = 0;
		XMFLOAT3 *heightMap;
	};

	std::vector<Model::Vertex>& getV();
	HeightMap();
	~HeightMap();
	void CreateGrid(HeightMapInfo &hminfo, std::vector <Model::Vertex> v);
	bool HeightMapLoad(char* filename, HeightMapInfo &hminfo);
	//std::vector<Model::Vertex> triangulateHeightMapData(HeightMapInfo &hminfo);
};