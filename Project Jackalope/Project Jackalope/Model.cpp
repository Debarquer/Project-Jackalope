#include "Model.h"

Model::Model()
{
	pSRV = nullptr;
}

//Model::Model(Model& other)
//{
//	this->material = other.material;
//	this->mVertices = other.mVertices;
//}

Model::~Model()
{
}

//**DEPRECATED?**
void Model::LoadModel(Model& other)
{
	this->material = other.material;
	this->mVertices = other.mVertices;
}

DirectX::XMFLOAT3 Model::cross(DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2)
{
	DirectX::SimpleMath::Vector3 output;

	output.x = v1.y * v2.z - v1.z * v2.y;
	output.y = v1.z * v2.x - v1.x * v2.z;
	output.z = v1.x * v2.y - v1.y * v2.x;

	return output;
}

Model Model::LoadTextFile(std::string filename, bool &failed, ID3D11Device* dev, ID3D11DeviceContext* devcon)
{
	bool hadMaterialFile, hadMaterial;
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
			hadMaterialFile = true;
		}
		else if (strcmp(lineHeader, "usemtl") == 0)
		{
			char tmpFileName[128];
			fscanf(file, "%s", &tmpFileName);
			model.material = tmpFileName;
			hadMaterial = true;
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

	if (!hadMaterial || !hadMaterialFile)
	{
		hadMaterial = "NULL";
		hadMaterialFile = "NULL";
	}
	else
	{
		std::string fileName = "";
		filename = model.materialFile.substr(0, model.materialFile.size() - 4);
		filename += ".png";

		wchar_t* wide_string = new wchar_t[filename.length() + 1];
		std::copy(filename.begin(), filename.end(), wide_string);
		wide_string[filename.length()] = 0;

		HRESULT hr = DirectX::CreateWICTextureFromFile(dev, L"grass.png", nullptr, &model.pSRV);
	}

	fclose(file);
	return model;
}