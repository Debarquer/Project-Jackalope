#pragma once
#include <Windows.h>
#include "ModelLoader.h"

using namespace DirectX;

int NumFaces = 0;
int NumVerices = 0;

std::vector<Model::Vertex> CreateGrid(ModelLoader::HeightMapInfo &hminfo)
{
	int cols = hminfo.terrainWidth;
	int rows = hminfo.terrainHeight;
	//Create the grid
	NumVerices = rows * cols;
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
	int texVundex = 0;
	for (DWORD i = 0; i < rows - 1; i++)
	{
		for (DWORD j = 0; j < cols - 1; j++)
		{
			//Bottom left
			indices[k] = i*cols + j;
			vertex.u = texUindex + 0.0f;
			vertex.v = texVundex + 1.0f;
			v.push_back(vertex);
			//Bottom right
			indices[k + 1] = i*cols + j + 1;
			vertex.u = texUindex + 1.0f;
			vertex.v = texVundex + 1.0f;
			v.push_back(vertex);
			//Top left
			indices[k + 2] = (i + 1)*cols + j;
			vertex.u = texUindex + 0.0f;
			vertex.u = texVundex + 0.0f;
			v.push_back(vertex);
			//Top left
			indices[k + 3] = (i + 1)*cols + j;
			vertex.u = texUindex + 0.0f;
			vertex.v = texVundex + 0.0f;
			v.push_back(vertex);
			//Bottom right 
			indices[k + 4] = i*cols + j + 1;
			vertex.u = texUindex + 1.0f;
			vertex.v = texVundex + 1.0f;
			v.push_back(vertex);
			//Top right
			indices[k + 5] = (i + 1)*cols + j + 1;
			vertex.u = texUindex + 1.0f;
			vertex.v = texVundex + 0.0f;
			v.push_back(vertex);
			k += 6; //next quad
			texUindex++;
		}
		texVundex++;
	}
	return v;
}

std::vector<Model::Vertex> triangulateHeightMapData(std::vector<Model::Vertex> vertices, ModelLoader::HeightMapInfo &hminfo)
{
	std::vector<Model::Vertex> tmp;
	tmp.resize(hminfo.terrainHeight*hminfo.terrainWidth*6);

	for (int i = 0, n = 0; i < hminfo.terrainHeight-1; i++)
	{
		for (int y = 0; y < hminfo.terrainWidth; y++, n+=6)
		{
			tmp[n].pX = vertices[y + i*hminfo.terrainWidth].pX;
			tmp[n].pZ = vertices[y + i*hminfo.terrainWidth].pZ;
			tmp[n].pY = vertices[y + i*hminfo.terrainWidth].pY;

			tmp[n + 1].pX = vertices[y + hminfo.terrainWidth + 1 + i*hminfo.terrainWidth].pX;
			tmp[n + 1].pZ = vertices[y + hminfo.terrainWidth + 1 + i*hminfo.terrainWidth].pZ;
			tmp[n + 1].pY = vertices[y + hminfo.terrainWidth + 1 + i*hminfo.terrainWidth].pY;

			tmp[n + 2].pX = vertices[y + hminfo.terrainWidth + i*hminfo.terrainWidth].pX;
			tmp[n + 2].pZ = vertices[y + hminfo.terrainWidth + i*hminfo.terrainWidth].pZ;
			tmp[n + 2].pY = vertices[y + hminfo.terrainWidth + i*hminfo.terrainWidth].pY;

			tmp[n + 3].pX = vertices[y + i*hminfo.terrainWidth].pX;
			tmp[n + 3].pZ = vertices[y + i*hminfo.terrainWidth].pZ;
			tmp[n + 3].pY = vertices[y + i*hminfo.terrainWidth].pY;

			tmp[n + 4].pX = vertices[y + 1 + i*hminfo.terrainWidth].pX;
			tmp[n + 4].pZ = vertices[y + 1 + i*hminfo.terrainWidth].pZ;
			tmp[n + 4].pY = vertices[y + 1 + i*hminfo.terrainWidth].pY;

			tmp[n + 5].pX = vertices[y + hminfo.terrainWidth + 1 + i*hminfo.terrainWidth].pX;
			tmp[n + 5].pZ = vertices[y + hminfo.terrainWidth + 1 + i*hminfo.terrainWidth].pZ;
			tmp[n + 5].pY = vertices[y + hminfo.terrainWidth + 1 + i*hminfo.terrainWidth].pY;
		}
	}

	return tmp;
}