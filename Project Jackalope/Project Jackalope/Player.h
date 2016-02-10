#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <windowsx.h>
#include "SimpleMath.h"

using namespace DirectX;

class Player
{
private:
	bool movedX, movedZ, isAirborne;

	SimpleMath::Vector3 movementSpeed;
	SimpleMath::Vector3 currentSpeed;
	SimpleMath::Vector3 maxSpeed;
	XMVECTOR DefaultFoward;
	XMVECTOR DefaultRight;
	XMVECTOR camForward;
	XMVECTOR camRight;
	XMVECTOR camUp;
	float rotY;
	float rotX;

public:

	XMVECTOR camera;
	XMVECTOR lookAt;
	float moveLeftRight = 0.0f;
	float moveBackForward = 0.0f;
	XMMATRIX view;
	XMMATRIX camRotationMatrix;
	
	void move(DirectX::SimpleMath::Vector3 movement, double dt);
	void update(double dt);

	void jump(double dt);
	void rotate(float x, float y, double dt);

	Player();
	~Player();
};

