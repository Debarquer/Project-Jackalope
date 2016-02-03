#include "ModelHandler.h"



ModelHandler::ModelHandler()
{
}


ModelHandler::~ModelHandler()
{
}

std::vector<Model>& ModelHandler::getModels()
{
	return mModels;
}

std::vector<Model::Vertex>& ModelHandler::getVertices()
{
	return mAllVertices;
}

void ModelHandler::addModel(Model& model)
{
	mModels.push_back(model);

	for (int i = 0; i < model.mVertices.size(); i++)
		mAllVertices.push_back(model.mVertices[i]);
}

void ModelHandler::addModel(std::vector<Model::Vertex> vertices)
{
	Model model;
	model.mVertices = vertices;
	model.material = "NULL";
	model.materialFile = "NULL";
	
	for (int i = 0; i < vertices.size(); i++)
		mAllVertices.push_back(vertices[i]);

	mModels.push_back(model);
}

void ModelHandler::addModel(DirectX::XMFLOAT3* positions, DWORD numVertices)
{
	std::vector<Model::Vertex> vertices;
	for (DWORD i = 0; i < numVertices; i++)
	{
		Model::Vertex vertex;
		vertex.pX = positions[i].x;
		vertex.pY = positions[i].y;
		vertex.pZ = positions[i].z;

		vertex.u = 0;
		vertex.v = 0;
		
		vertex.nX = 0;
		vertex.nY = 1;
		vertex.nZ = 0;

		vertex.r = 0;
		vertex.g = 1;
		vertex.b = 0;

		vertices.push_back(vertex);
		mAllVertices.push_back(vertex);
	}

	Model model;
	model.mVertices = vertices;
	model.material = "NULL";
	model.materialFile = "NULL";

	mModels.push_back(model);
}

