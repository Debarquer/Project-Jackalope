#pragma once

#include "SimpleMath.h"
#include "Model.h"
#include <DirectXMath.h>

using namespace DirectX;

namespace ModelLoader
{
	struct HeightMapInfo {
		std::vector<DWORD> indices;
		int terrainWidth;
		int terrainHeight;
		DWORD numVertices = 0;
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
		return true;
	}
}