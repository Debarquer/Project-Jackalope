#include "Model.h"



Model::Model()
{
}

Model::Model(Model& other)
{
	this->material = other.material;
	this->mVertices = other.mVertices;
}

Model::~Model()
{
}

void Model::LoadModel(Model& other)
{
	this->material = other.material;
	this->mVertices = other.mVertices;
}