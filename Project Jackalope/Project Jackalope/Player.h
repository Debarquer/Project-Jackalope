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
	float rotationAngle;

public:

	XMVECTOR camera;
	XMVECTOR lookAt;
	float moveLeftRight = 0.0f;
	float moveBackForward = 0.0f;
	XMMATRIX view;
	XMMATRIX camRotationMatrix;
	
	void move(XMVECTOR movement, double dt);
	void strafe(XMVECTOR movement, double dt);
	void pitch(float direction, double dt);
	void yaw(float direction, double dt);
	void update();
	void jump(double dt);

	Player();
	~Player();
};

