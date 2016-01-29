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
