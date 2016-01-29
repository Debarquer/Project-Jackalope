#pragma once

#include "Model.h"
#include <vector>
#include <DirectXMath.h>

class ModelHandler
{
private:
	std::vector<Model> mModels;
	std::vector<Vertex> mAllVertices;
public:
	ModelHandler();
	~ModelHandler();

	std::vector<Model>& getModels();
	std::vector<Vertex>& getVertices();

	void addModel(Model& model);
	void addModel(DirectX::XMFLOAT3* positions);
};

