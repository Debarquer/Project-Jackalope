#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <windowsx.h>
#include "SimpleMath.h"

class Player
{
private:
	bool movedX, movedZ, isAirborne;

	DirectX::SimpleMath::Vector3 movementSpeed;
	DirectX::SimpleMath::Vector3 currentSpeed;
	DirectX::SimpleMath::Vector3 maxSpeed;

	float rotY = 0.0f;
	float rotX = 0.0f;

public:

	DirectX::SimpleMath::Vector3 camera;
	DirectX::SimpleMath::Vector3 lookAt;

	DirectX::XMMATRIX view;

	void move(DirectX::SimpleMath::Vector3 movement, double dt);
	void update(double dt);

	void jump(double dt);
	void rotate(float x, float y, double dt);

	Player();
	~Player();
};

