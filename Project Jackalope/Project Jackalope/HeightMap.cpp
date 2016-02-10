#include "HeightMap.h"

std::vector<Model::Vertex>& HeightMap::getV()
{
	return v;
}

std::vector<DWORD>& HeightMap::getIndices()
{
	return indices;
}

HeightMap::HeightMap()
{
	NumFaces = 0;
	NumVertices = 0;
	cols = 0;
	rows = 0;
}

HeightMap::~HeightMap()
{
}

void HeightMap::CreateGrid(HeightMapInfo & hminfo, std::vector<Model::Vertex> v, std::vector<DWORD> indices)
{
		cols = hminfo.terrainWidth;
		rows = hminfo.terrainHeight;
		//Create the grid
		//NumVertices = rows * cols;
		NumFaces = (rows - 1) * (cols - 1) * 2;
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
			for (DWORD i = 0; i < rows - 1; i++)
			{
				for (DWORD j = 0; j < cols - 1; j++)
				{
					//Bottom left
					indices.push_back(i*cols + j);
					//Bottom right
					indices.push_back(i*cols + j + 1);
					//Top left
					indices.push_back((i + 1)*cols + j);
					//Top left
					indices.push_back((i + 1)*cols + j);
					//Bottom right 
					indices.push_back(i*cols + j + 1);
					//Top right
					indices.push_back((i + 1)*cols + j + 1);
			}
		}
}

bool HeightMap::HeightMapLoad(char * filename, HeightMapInfo & hminfo)
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

	int l = 0;

	float heightFactor = 10.0f;
	for (int j = 0; j < hminfo.terrainHeight; j++)
	{
		for (int i = 0; i < hminfo.terrainWidth; i++)
		{

			height = bitmapImage[l];
			index = (hminfo.terrainHeight * j) + i;
			hminfo.heightMap[index].x = (float)i;
			hminfo.heightMap[index].y = (float)height / heightFactor;
			hminfo.heightMap[index].z = (float)j;
			l += 3;
		}

	}
	delete[] bitmapImage;
	bitmapImage = 0;
	return true;

}
