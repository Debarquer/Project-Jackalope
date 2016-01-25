#include "Model.h"



Model::Model()
{
}


Model::~Model()
{
}

void Model::LoadModel(std::vector<Vertex> &vertices)
{
	mVertices = vertices;
}