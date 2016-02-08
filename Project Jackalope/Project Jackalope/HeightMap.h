#pragma once
#include <Windows.h>
#include "Model.h"

using namespace DirectX;

int NumFaces = 0;
int NumVertices = 0;

DirectX::SimpleMath::Vector3 cross(DirectX::SimpleMath::Vector3 v1, DirectX::SimpleMath::Vector3 v2)
{
	DirectX::SimpleMath::Vector3 output;

	output.x = v1.y * v2.z - v1.z * v2.y;
	output.y = v1.z * v2.x - v1.x * v2.z;
	output.z = v1.x * v2.y - v1.y * v2.x;

	return output;
}


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
	int texUindex;
	int texVindex;

	struct HeightMapInfo {
		std::vector<DWORD> indices;
		int terrainWidth;
		int terrainHeight;
		DWORD numVertices = 0;
		XMFLOAT3 *heightMap;
	};

	std::vector<Model::Vertex>& getV();
	std::vector<DWORD>& getIndices();
	HeightMap();
	~HeightMap();
	void CreateGrid(HeightMapInfo &hminfo, std::vector <Model::Vertex> v);
	void Triangulate(std::vector <DWORD> indices);
	bool HeightMapLoad(char* filename, HeightMapInfo &hminfo);
};