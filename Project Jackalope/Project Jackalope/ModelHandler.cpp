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
}
