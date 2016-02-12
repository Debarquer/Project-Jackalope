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
	cols = 0;
	rows = 0;
}

HeightMap::~HeightMap()
{
}

void HeightMap::CreateGrid(HeightMapInfo & hminfo, std::vector<Model::Vertex>& v, std::vector<DWORD>& indices)
{
		cols = hminfo.terrainWidth;
		rows = hminfo.terrainHeight;
		//Create the grid
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
					//Top left
					indices.push_back(i*cols + j);
					//Bottom right
					indices.push_back((i + 1)*cols + j + 1);
					//Bottom left
					indices.push_back((i + 1)*cols + j);
					//Top left
					indices.push_back(i*cols + j);
					//Top right 
					indices.push_back(i*cols + j + 1);
					//Bottom right
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

void HeightMap::calculateNormals()
{
	for (int i = 0; i < indices.size(); i+=3)
	{
		int a = indices[i];
		int b = indices[i + 1];
		int c = indices[i + 2];

		XMVECTOR xnormal = XMVector3Cross(
			XMVectorSet(v[c].pX - v[a].pX, v[c].pY - v[a].pY, v[c].pZ - v[a].pZ, 1.0),
			XMVectorSet(v[b].pX - v[a].pX, v[b].pY - v[a].pY, v[b].pZ - v[a].pZ, 1.0));

		xnormal = XMVector3Normalize(xnormal);
		XMFLOAT3 normal;
		XMStoreFloat3(&normal, xnormal);

		v[a].nX = v[b].nX = v[c].nX = normal.x;
		float lengtha = sqrt(v[a].pX*v[a].pX + v[a].pY*v[a].pY + v[a].pZ*v[a].pZ);
		v[a].nY = v[b].nY = v[c].nY = normal.y;
		float lengthb = sqrt(v[b].pX*v[b].pX + v[b].pY*v[b].pY + v[b].pZ*v[b].pZ);
		v[a].nZ = v[b].nZ = v[c].nZ = normal.z;
		float lengthc = sqrt(v[c].pX*v[c].pX + v[c].pY*v[c].pY + v[c].pZ*v[c].pZ);

		v[a].nX /= lengtha;
		v[a].nY /= lengtha;
		v[a].nZ /= lengtha;

		v[b].nX /= lengthb;
		v[b].nY /= lengthb;
		v[b].nZ /= lengthb;

		v[c].nX /= lengthc;
		v[c].nY /= lengthc;
		v[c].nZ /= lengthc;
	}
	
}