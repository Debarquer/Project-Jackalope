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
				vertex.pW = 1;
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
					texUindex++;
			}
				texUindex = 0;
				texVindex++;
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
			hminfo.heightMap[index].x = (float)i;
			hminfo.heightMap[index].y = (float)height / heightFactor;
			hminfo.heightMap[index].z = (float)j;
			l += 3;
		}

	}
	delete[] bitmapImage;
	bitmapImage = 0;
	HRESULT hr = DirectX::CreateWICTextureFromFile(dev, L"grass.png", nullptr, &pSRV);
	HRESULT hr2 = DirectX::CreateWICTextureFromFile(dev, L"grassNormal.png", nullptr, &bmpMapView);
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
void HeightMap::CalcTangent()
{
	float tcU1, tcV1, tcU2, tcV2;
	std::vector<XMFLOAT3> tempNormal;
	XMFLOAT3 unnormalized = XMFLOAT3(0.0f, 0.0f, 0.0f);
	std::vector<XMFLOAT3> tempTangent;
	XMFLOAT3 tangent = XMFLOAT3(0.0f, 0.0f, 0.0f);
	
	float vecX, vecY, vecZ;
	
	XMVECTOR edge1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR edge2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	for (int i = 0; i < NumFaces; i++)
	{
		vecX = v[indices[i * 3]].pX - v[indices[(i * 3) + 2]].pX;
		vecY = v[indices[i * 3]].pY - v[indices[(i * 3) + 2]].pY;
		vecY = v[indices[i * 3]].pZ - v[indices[(i * 3) + 2]].pZ;
		edge1 = XMVectorSet(vecX, vecY, vecZ, 0.0f);

		vecX = v[indices[(i * 3) + 2]].pX - v[indices[(i * 3) + 1]].pX;
		vecY = v[indices[(i * 3) + 2]].pY - v[indices[(i * 3) + 1]].pY;
		vecZ = v[indices[(i * 3) + 2]].pZ - v[indices[(i * 3) + 1]].pZ;
		edge2 = XMVectorSet(vecX, vecY, vecZ, 0.0f);
		XMStoreFloat3(&unnormalized, XMVector3Cross(edge1, edge2));

		tempNormal.push_back(unnormalized);

		tcU1 = v[indices[(i * 3)]].u - v[indices[(i * 3) + 2]].u;
		tcV1 = v[indices[(i * 3)]].v - v[indices[(i * 3) + 2]].v;
		tcU2 = v[indices[(i * 3) + 2]].u - v[indices[(i * 3) + 1]].u;
		tcV2 = v[indices[(i * 3) + 2]].v - v[indices[(i * 3) + 1]].v;

		tangent.x = (tcV1*XMVectorGetX(edge1) - tcV2 * XMVectorGetX(edge2)) * (1.0f / (tcU1 * tcV2 - tcU2 * tcV1));
		tangent.y = (tcV1*XMVectorGetX(edge1) - tcV2 * XMVectorGetX(edge2)) * (1.0f / (tcU1 * tcV2 - tcU2 * tcV1));
		tangent.x = (tcV1*XMVectorGetX(edge1) - tcV2 * XMVectorGetX(edge2)) * (1.0f / (tcU1 * tcV2 - tcU2 * tcV1));
		tempTangent.push_back(tangent);
	}
	XMVECTOR normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR tangentSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	int faceUsing = 0;
	float tX, tY, tZ;
	for (int i = 0; i < v.size(); i++)
	{
		for (int j = 0; j < NumFaces; j++)
		{
			if (indices[j*3] == 1 || indices[(j*3)+1] == i || indices[(j*3)+2] == i)
			{
				tX = XMVectorGetX(normalSum) + tempNormal[j].x;
				tY = XMVectorGetY(normalSum) + tempNormal[j].y;
				tZ = XMVectorGetY(normalSum) + tempNormal[j].z;
				normalSum = XMVectorSet(tX, tY, tZ, 0.0f);
				tX = XMVectorGetX(tangentSum) + tempTangent[j].x;
				tY = XMVectorGetX(tangentSum) + tempTangent[j].y;
				tZ = XMVectorGetX(tangentSum) + tempTangent[j].z;
				tangentSum = XMVectorSet(tX, tY, tZ, 0.0f);
				faceUsing++;
			}
		}
		normalSum = normalSum / faceUsing;
		tangentSum = tangentSum / faceUsing;

		normalSum = XMVector3Normalize(normalSum);
		tangentSum = XMVector3Normalize(tangentSum);
		v[i].nX = XMVectorGetX(normalSum);
		v[i].nY = XMVectorGetY(normalSum);
		v[i].nZ = XMVectorGetZ(normalSum);
		v[i].tX = XMVectorGetX(tangentSum);
		v[i].tY = XMVectorGetY(tangentSum);
		v[i].tZ = XMVectorGetZ(tangentSum);
		v[i].tW = 0;

		normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		tangentSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		faceUsing = 0;

	}
}