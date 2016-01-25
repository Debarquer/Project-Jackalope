#pragma once

#include "SimpleMath.h"
#include "Model.h"

namespace ModelLoader
{
	std::vector<Vertex> LoadTextFile(std::string filename, bool &failed)
	{
		failed = false;

		std::vector<Vertex> model;

		std::vector<unsigned int> vertexIndices;
		std::vector<DirectX::SimpleMath::Vector3> tempVertices;

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
			else if (strcmp(lineHeader, "f") == 0)
			{
				std::string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3], normalIndex[3];
				int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
				if (matches != 9)
				{
					failed = true;
				}

				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
			}
		}

		for (unsigned int i = 0; i < vertexIndices.size(); i++)
		{
			Vertex vertex;

			unsigned int vertexIndex = vertexIndices[i];
			vertex.x = tempVertices[vertexIndex - 1].x;
			vertex.y = tempVertices[vertexIndex - 1].y;
			vertex.z = tempVertices[vertexIndex - 1].z;

			vertex.r = 1;
			vertex.g = 0;
			vertex.b = 0;
			vertex.w = 1;

			model.push_back(vertex);
		}

		fclose(file);
		return model;
	}
};