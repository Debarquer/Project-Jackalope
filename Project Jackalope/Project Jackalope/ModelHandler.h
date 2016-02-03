#pragma once

#include "Model.h"
#include <vector>
#include <DirectXMath.h>

class ModelHandler
{
private:
	std::vector<Model> mModels;
	std::vector<Model::Vertex> mAllVertices;
public:
	ModelHandler();
	~ModelHandler();

	std::vector<Model>& getModels();
	std::vector<Model::Vertex>& getVertices();

	void addModel(Model& model);
	void addModel(std::vector<Model::Vertex> vertices);
	void addModel(DirectX::XMFLOAT3* positions, DWORD numVertices);
};

