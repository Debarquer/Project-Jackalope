#include "Model.h"



Model::Model()
{
}


Model::~Model()
{
}

void Model::LoadModel(std::vector<Vertex> &vertices)
{
	mVertices = new Vertex[vertices.size()];
	for (int i = 0; i < vertices.size(); i++)
	{
		mVertices[i].x = vertices[i].x;
		mVertices[i].y = vertices[i].y;
		mVertices[i].z = vertices[i].z;
		mVertices[i].r = vertices[i].r;
		mVertices[i].g = vertices[i].g;
		mVertices[i].b = vertices[i].b;
		mVertices[i].w = vertices[i].w;
	}
}