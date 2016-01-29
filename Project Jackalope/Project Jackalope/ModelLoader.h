#pragma once

#include "SimpleMath.h"
#include "Model.h"
#include <DirectXMath.h>

using namespace DirectX;

namespace ModelLoader
{
	struct HeightMapInfo {
		int terrainWidth;
		int terrainHeight;
		XMFLOAT3 *heightMap;
	};

	Model LoadTextFile(std::string filename, bool &failed)
	{
		failed = false;

		Model model;

		std::vector<unsigned int> vertexIndices, normalIndices, uvIndices;
		std::vector<DirectX::SimpleMath::Vector3> tempVertices, tempNormals, tempUvs;

		FILE* file = fopen(filename.c_str(), "r");
		if (file == NULL)
		{
			failed = true;
		}

		while (true)
		{
			char lineHeader[128];

			int res = fscanf(file, "%s", lineHeader);
			if (res == EOF)
				break;

			if (strcmp(lineHeader, "v") == 0)
			{
				DirectX::SimpleMath::Vector3 vertex;
				fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				tempVertices.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vn") == 0)
			{
				DirectX::SimpleMath::Vector3 normal;
				fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				tempNormals.push_back(normal);
			}
			else if (strcmp(lineHeader, "vt") == 0)
			{
				DirectX::SimpleMath::Vector3 uv;
				fscanf(file, "%f %f %f\n", &uv.x, &uv.y);
				tempUvs.push_back(uv);
			}
			else if (strcmp(lineHeader, "mtllib") == 0)
			{
				char tmpFilePath[128];
				fscanf(file, "%s", &tmpFilePath);
				model.materialFile = tmpFilePath;
			}
			else if (strcmp(lineHeader, "usemtl") == 0)
			{
				char tmpFileName[128];
				fscanf(file, "%s", &tmpFileName);
				model.material = tmpFileName;
			}
			else if (strcmp(lineHeader, "f") == 0)
			{
				std::string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				if (matches != 9)
				{
					failed = true;
				}

				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
			}
		}

		for (unsigned int i = 0; i < vertexIndices.size(); i++)
		{
			Vertex vertex;

			unsigned int vertexIndex = vertexIndices[i];
			vertex.pX = tempVertices[vertexIndex - 1].x;
			vertex.pY = tempVertices[vertexIndex - 1].y;
			vertex.pZ = tempVertices[vertexIndex - 1].z;

			vertex.r = 1;
			vertex.g = 0;
			vertex.b = 0;
			vertex.w = 1;
			
			unsigned int normalIndex = normalIndices[i];
			vertex.nX = tempNormals[normalIndex - 1].x;
			vertex.nY = tempNormals[normalIndex - 1].y;
			vertex.nZ = tempNormals[normalIndex - 1].z;

			unsigned int uvIndex = uvIndices[i];
			vertex.u = tempUvs[uvIndex - 1].x;
			vertex.v = tempUvs[uvIndex - 1].y;

			model.mVertices.push_back(vertex);
		}

		fclose(file);
		return model;
	}

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
};