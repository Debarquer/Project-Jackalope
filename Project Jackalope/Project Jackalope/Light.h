#pragma once
#include "Model.h"
class Light :
	public Model
{
private:
public:
	float r, g, b;
	DirectX::XMFLOAT3 position;

	Light();
	~Light();
};

