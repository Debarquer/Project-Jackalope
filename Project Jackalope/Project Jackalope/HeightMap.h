#pragma once
#include <Windows.h>
#include "ModelLoader.h"

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

std::vector<Model::Vertex> CreateGrid(ModelLoader::HeightMapInfo &hminfo)
{
	int cols = hminfo.terrainWidth;
	int rows = hminfo.terrainHeight;
	//Create the grid
	NumVertices = rows * cols;
	NumFaces = (rows - 1) * (cols - 1) * 2;
	std::vector<Model::Vertex> v;
	Model::Vertex vertex;
	for (DWORD i = 0; i < rows; i++)
	{
		for (DWORD j = 0; j < cols; j++)
		{
			vertex.pX = hminfo.heightMap[i*cols + j].x;
			vertex.pY = hminfo.heightMap[i*cols + j].y;
			vertex.pZ = hminfo.heightMap[i*cols + j].z;
			vertex.nX = 0.0f;
			vertex.nY = 1.0f;
			vertex.nZ = 0.0f;
			v.push_back(vertex);
			hminfo.numVertices++;
		}
		
	}
	// Creating index list
	std::vector<DWORD> indices(NumFaces * 3);
	int k = 0;
	int texUindex = 0;
	int texVindex = 0;
	for (DWORD i = 0; i < rows - 1; i++)
	{
		for (DWORD j = 0; j < cols - 1; j++)
		{
			//Bottom left
			indices[k] = i*cols + j;
			vertex.u = texUindex + 0.0f;
			vertex.v = texVindex + 1.0f;
			v.push_back(vertex);
			//Bottom right
			indices[k + 1] = i*cols + j + 1;
			vertex.u = texUindex + 1.0f;
			vertex.v = texVindex + 1.0f;
			v.push_back(vertex);
			//Top left
			indices[k + 2] = (i + 1)*cols + j;
			vertex.u = texUindex + 0.0f;
			vertex.u = texVindex + 0.0f;
			v.push_back(vertex);
			//Top left
			indices[k + 3] = (i + 1)*cols + j;
			vertex.u = texUindex + 0.0f;
			vertex.v = texVindex + 0.0f;
			v.push_back(vertex);
			//Bottom right 
			indices[k + 4] = i*cols + j + 1;
			vertex.u = texUindex + 1.0f;
			vertex.v = texVindex + 1.0f;
			v.push_back(vertex);
			//Top right
			indices[k + 5] = (i + 1)*cols + j + 1;
			vertex.u = texUindex + 1.0f;
			vertex.v = texVindex + 0.0f;
			v.push_back(vertex);
			k += 6; //next quad
			texUindex++;
		}
		texVindex++;
	}
	return v;
}

std::vector<Model::Vertex> triangulateHeightMapData(std::vector<Model::Vertex> vertices, ModelLoader::HeightMapInfo &hminfo)
{
	std::vector<Model::Vertex> tmp;

	for (int i = 0, n = 0; i < hminfo.terrainHeight-1; i++)
	{
		for (int y = 0; y < hminfo.terrainWidth-1; y++, n+=6)
		{
			tmp.resize(tmp.size() + 6);

			tmp[n].pX = vertices[y + i*hminfo.terrainWidth].pX;
			tmp[n].pZ = vertices[y + i*hminfo.terrainWidth].pZ;
			tmp[n].pY = vertices[y + i*hminfo.terrainWidth].pY;

			tmp[n + 1].pX = vertices[y + hminfo.terrainWidth + 1 + i*hminfo.terrainWidth].pX;
			tmp[n + 1].pZ = vertices[y + hminfo.terrainWidth + 1 + i*hminfo.terrainWidth].pZ;
			tmp[n + 1].pY = vertices[y + hminfo.terrainWidth + 1 + i*hminfo.terrainWidth].pY;

			tmp[n + 2].pX = vertices[y + hminfo.terrainWidth + i*hminfo.terrainWidth].pX;
			tmp[n + 2].pZ = vertices[y + hminfo.terrainWidth + i*hminfo.terrainWidth].pZ;
			tmp[n + 2].pY = vertices[y + hminfo.terrainWidth + i*hminfo.terrainWidth].pY;

			//Calculate the normals for the first triangle
			DirectX::SimpleMath::Vector3 tmpNormal = cross(
				DirectX::SimpleMath::Vector3{ tmp[n + 1].pX, tmp[n + 1].pY, tmp[n + 1].pZ } -
				DirectX::SimpleMath::Vector3{ tmp[n].pX, tmp[n].pY, tmp[n].pZ },
				DirectX::SimpleMath::Vector3{ tmp[n + 2].pX, tmp[n + 2].pY, tmp[n + 2].pZ } -
				DirectX::SimpleMath::Vector3{ tmp[n].pX, tmp[n].pY, tmp[n].pZ });

			tmp[n].nX = tmpNormal.x;
			tmp[n].nY = tmpNormal.y;
			tmp[n].nZ = tmpNormal.z;

			tmp[n + 1].nX = tmpNormal.x;
			tmp[n + 1].nY = tmpNormal.y;
			tmp[n + 1].nZ = tmpNormal.z;

			tmp[n + 2].nX = tmpNormal.x;
			tmp[n + 2].nY = tmpNormal.y;
			tmp[n + 2].nZ = tmpNormal.z;

			tmp[n + 3].pX = vertices[y + i*hminfo.terrainWidth].pX;
			tmp[n + 3].pZ = vertices[y + i*hminfo.terrainWidth].pZ;
			tmp[n + 3].pY = vertices[y + i*hminfo.terrainWidth].pY;

			tmp[n + 4].pX = vertices[y + 1 + i*hminfo.terrainWidth].pX;
			tmp[n + 4].pZ = vertices[y + 1 + i*hminfo.terrainWidth].pZ;
			tmp[n + 4].pY = vertices[y + 1 + i*hminfo.terrainWidth].pY;

			tmp[n + 5].pX = vertices[y + hminfo.terrainWidth + 1 + i*hminfo.terrainWidth].pX;
			tmp[n + 5].pZ = vertices[y + hminfo.terrainWidth + 1 + i*hminfo.terrainWidth].pZ;
			tmp[n + 5].pY = vertices[y + hminfo.terrainWidth + 1 + i*hminfo.terrainWidth].pY;

			//Calculate the normals for the second triangle
			tmpNormal = cross(
				DirectX::SimpleMath::Vector3{ tmp[n + 4].pX, tmp[n + 4].pY, tmp[n + 4].pZ } -
				DirectX::SimpleMath::Vector3{ tmp[n + 3].pX, tmp[n + 3].pY, tmp[n + 3].pZ },
				DirectX::SimpleMath::Vector3{ tmp[n + 5].pX, tmp[n + 5].pY, tmp[n + 5].pZ } -
				DirectX::SimpleMath::Vector3{ tmp[n + 3].pX, tmp[n + 3].pY, tmp[n + 3].pZ });

			tmp[n + 3].nX = tmpNormal.x;
			tmp[n + 3].nY = tmpNormal.y;
			tmp[n + 3].nZ = tmpNormal.z;

			tmp[n + 4].nX = tmpNormal.x;
			tmp[n + 4].nY = tmpNormal.y;
			tmp[n + 4].nZ = tmpNormal.z;

			tmp[n + 5].nX = tmpNormal.x;
			tmp[n + 5].nY = tmpNormal.y;
			tmp[n + 5].nZ = tmpNormal.z;
		}
	}

	return tmp;
}