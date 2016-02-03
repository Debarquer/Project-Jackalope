#pragma once
#include <Windows.h>
#include "ModelLoader.h"

using namespace DirectX;

int NumFaces = 0;
int NumVerices = 0;

void CreateGrid(ModelLoader::HeightMapInfo &hminfo)
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
	std::vector<Model> mModels;
	Model model;
	model.mVertices = v;
	model.material = "NULL";
	model.materialFile = "NULL";
	mModels.push_back(model);
	hminfo.indices = indices;
}

void CreateModelFromHeightMap(ModelLoader::HeightMapInfo& hmInfo)
{
	const int size = hmInfo.indices.size();
	ModelLoader::HeightMapInfo temp;
	temp.heightMap = new DirectX::XMFLOAT3[size];
	hmInfo.heightMap = new DirectX::XMFLOAT3[size];
	temp = hmInfo;

	for (int i = 0; i < hmInfo.indices.size(); i++)
	{
		hmInfo.heightMap[i] = temp.heightMap[temp.indices[i]];
	}
}