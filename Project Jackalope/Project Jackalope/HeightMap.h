#pragma once
#include <DirectXMath.h>
#include <Windows.h>
#include "Model.h"

using namespace DirectX;

int NumFaces = 0;
int NumVerices = 0;

struct HeightMapInfo { 
	int terrainWidth;
	int terrainHeight;
	XMFLOAT3 *heightMap;
};

bool HeightMapLoad(char* filename, HeightMapInfo &hminfo)
{
	FILE *fileptr;

	BITMAPFILEHEADER bitmapFileHeader; // Structure which stores info about file
	BITMAPINFOHEADER bitmapInfoHeader; // Structure which stores info about image

	int imageSize = 0, index = 0;
	unsigned char height = 0;

	fileptr = fopen(filename, "rb");
	if (fileptr == NULL)
	{
		return 0;
	}
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, fileptr);

	fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, fileptr);

	hminfo.terrainWidth = bitmapInfoHeader.biWidth;
	hminfo.terrainHeight = bitmapInfoHeader.biHeight;
	// Size of the image in bytes. The 3 represents RGB for each pixel
	imageSize = hminfo.terrainWidth * hminfo.terrainHeight * 3;

	unsigned char* bitmapImage = new unsigned char[imageSize];

	fseek(fileptr, bitmapFileHeader.bfOffBits, SEEK_SET);

	fread(bitmapImage, 1, imageSize, fileptr);
	if (fileptr != NULL)
	{
		fclose(fileptr);
	}
	
	hminfo.heightMap = new XMFLOAT3[hminfo.terrainWidth * hminfo.terrainHeight];

	int k = 0;

	float heightFactor = 10.0f;
	for (int j = 0; j < hminfo.terrainHeight; j++)
	{
		for (int i = 0; i < hminfo.terrainWidth; i++)
		{

			height = bitmapImage[k];
			index = (hminfo.terrainHeight * j) + i;
			hminfo.heightMap[index].x = (float)i;
			hminfo.heightMap[index].y = (float)height / heightFactor;
			hminfo.heightMap[index].z = (float)j;
			k += 3;
		}

	}
	delete[] bitmapImage;
	bitmapImage = 0;

	int cols = hminfo.terrainWidth;
	int rows = hminfo.terrainHeight;
	//Create the grid
	NumVerices = rows * cols;
	NumFaces = (rows - 1) * (cols - 1) * 2;
	std::vector<Vertex> v(NumVerices);

	for (DWORD i = 0; i < rows; ++i)
	{
		for (DWORD j = 0; j < cols; ++j)
		{
			v[i*cols + j].pX = hminfo.heightMap[i*cols + j].x;
			v[i*cols + j].pY = hminfo.heightMap[i*cols + j].y;
			v[i*cols + j].pZ = hminfo.heightMap[i*cols + j].z;
			v[i*cols + j].nX = 0.0f;
			v[i*cols + j].nY = 1.0f;
			v[i*cols + j].nZ = 0.0f;
		}
	}
	// Creating index list
	std::vector<DWORD> indices(NumFaces * 3);
	k = 0;
	int texUindex = 0;
	int texVundex = 0;
	for (DWORD i = 0; i < rows-1; i++)
	{
		for (DWORD j = 0; j < cols-1; j++)
		{
			//Bottom left
			indices[k] = i*cols + j;
			v[i*cols + j].u = texUindex + 0.0f;
			v[i*cols + j].v = texVundex + 1.0f;
			//Bottom right
			indices[k + 1] = i*cols + j + 1;
			v[i*cols + j + 1].u = texUindex + 1.0f;
			v[i*cols + j + 1].v = texVundex + 1.0f;
			//Top left
			indices[k + 2] = (i + 1)*cols + j;
			v[(i + 1) * cols + j].u = texUindex + 0.0f;
			v[(i + 1) * cols + j].u = texVundex + 0.0f;
			//Top left
			indices[k + 3] = (i + 1)*cols + j;
			v[(i + 1) * cols + j].u = texUindex + 0.0f;
			v[(i + 1) * cols + j].v = texVundex + 0.0f;
			//Bottom right 
			indices[k + 4] = i*cols + j + 1;
			v[i*cols + j + 1].u = texUindex + 1.0f;
			v[i*cols + j + 1].v = texVundex + 1.0f;
			//Top right
			indices[k + 5] = (i + 1)*cols + j + 1;
			v[(i + 1)*cols + j + 1].u = texUindex + 1.0f;
			v[(i + 1)*cols + j + 1].v = texVundex + 0.0f;
			k += 6; //next quad
			texUindex++;
		}
		texUindex = 0;
		texVundex++;
	}
	return true;
}