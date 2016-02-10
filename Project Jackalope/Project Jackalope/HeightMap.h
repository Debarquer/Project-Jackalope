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
	int NumVertices;
	int cols;
	int rows;

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
	void CreateGrid(HeightMapInfo &hminfo, std::vector <Model::Vertex> v, std::vector <DWORD> indices);
	bool HeightMapLoad(char* filename, HeightMapInfo &hminfo);
};