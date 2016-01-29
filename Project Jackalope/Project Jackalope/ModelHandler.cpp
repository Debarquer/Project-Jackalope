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

std::vector<Vertex>& ModelHandler::getVertices()
{
	return mAllVertices;
}

void ModelHandler::addModel(Model& model)
{
	mModels.push_back(model);

	for (int i = 0; i < model.mVertices.size(); i++)
		mAllVertices.push_back(model.mVertices[i]);
}

void ModelHandler::addModel(DirectX::XMFLOAT3* positions, int numVertices)
{
	std::vector<Vertex> vertices;
	for (int i = 0; i < numVertices; i++)
	{
		Vertex vertex;
		vertex.pX = positions->x;
		vertex.pY = positions->y;
		vertex.pZ = positions->z;

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

