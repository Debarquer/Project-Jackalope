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
	texUindex = 0;
	texVindex = 0;
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
					v[i*cols + j].u = texUindex + 0.0f;
					v[i*cols + j].v = texVindex + 0.0f;
					//Bottom right
					indices.push_back((i + 1)*cols + j + 1);
					v[(i + 1)*cols + j + 1].u = texUindex + 1.0f;
					v[(i + 1)*cols + j + 1].v = texVindex + 1.0f;
					//Bottom left
					indices.push_back((i + 1)*cols + j);
					v[(i + 1)*cols + j].u = texUindex + 0.0f;
					v[(i + 1)*cols + j].v = texVindex + 1.0f;
					//Top left
					indices.push_back(i*cols + j);
					v[i*cols + j].u = texUindex + 0.0f;
					v[i*cols + j].v = texVindex + 0.0f;
					//Top right 
					indices.push_back(i*cols + j + 1);
					v[i*cols + j + 1].u = texUindex + 1.0f;
					v[i*cols + j + 1].v = texVindex + 0.0f;
					//Bottom right
					indices.push_back((i + 1)*cols + j + 1);
					v[(i + 1)*cols + j + 1].u = texUindex + 1.0f;
					v[(i + 1)*cols + j + 1].v = texVindex + 1.0f;
					texUindex ++;
			}
				texUindex = 0;
				texVindex ++;
		}
}

bool HeightMap::HeightMapLoad(char * filename, HeightMapInfo & hminfo, ID3D11Device* dev)
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
			hminfo.heightMap[index].x = (float) i;
			hminfo.heightMap[index].y = (float)height/heightFactor;
			hminfo.heightMap[index].z = (float) j;
			l += 3; 
		}

	}
	delete[] bitmapImage;
	bitmapImage = 0;
	HRESULT hr = DirectX::CreateWICTextureFromFile(dev, L"grass.jpg", nullptr, &pSRV);
	//HRESULT hr2 = DirectX::CreateWICTextureFromFile(dev, L"grassNormal.png", nullptr, &bmpMapView);
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
		v[a].nY = v[b].nY = v[c].nY = normal.y;
		v[a].nZ = v[b].nZ = v[c].nZ = normal.z;

		float deltaUVX1, deltaUVX2, deltaUVY1, deltaUVY2;

		deltaUVX1 = v[c].u - v[a].u;
		deltaUVY1 = v[c].v - v[a].v;
		deltaUVX2 = v[b].u - v[a].u;
		deltaUVY2 = v[b].v - v[a].v;
		
		float f = 1.0f / (deltaUVX1 * deltaUVY2 - deltaUVY1 * deltaUVX2);
		
		XMFLOAT3 xtangent;
		xtangent.x = f * (deltaUVY2 * (v[c].pX - v[a].pX) - deltaUVY1 * (v[b].pX - v[a].pX));
		xtangent.y = f * (deltaUVY2 * (v[c].pY - v[a].pY) - deltaUVY1 * (v[b].pY - v[a].pY));
		xtangent.z = f * (deltaUVY2 * (v[c].pZ - v[a].pZ) - deltaUVY1 * (v[b].pZ - v[a].pZ));
		XMVECTOR tangent = XMLoadFloat3(&xtangent);
		tangent = XMVector3Normalize(tangent);

		v[a].tX = v[b].tX = v[c].tX = XMVectorGetX(tangent);
		v[b].tY = v[b].tY = v[c].tY = XMVectorGetY(tangent);
		v[a].tZ = v[b].tZ = v[c].tZ = XMVectorGetZ(tangent);
		
		/*XMFLOAT3 xbitangent;
		xbitangent.x = f * (-deltaUVX2 * (v[c].pX - v[a].pX) + deltaUVX1 * (v[b].pX - v[a].pX));
		xbitangent.y = f * (-deltaUVX2 * (v[c].pY - v[a].pY) + deltaUVX1 * (v[b].pY - v[a].pY));
		xbitangent.z = f * (-deltaUVX2 * (v[c].pZ - v[a].pZ) + deltaUVX1 * (v[b].pZ - v[a].pZ));
		XMVECTOR bitangent;
		bitangent = XMLoadFloat3(&xbitangent);
		bitangent = XMVector3Normalize(bitangent);*/
	}
}