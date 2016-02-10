#include "Player.h"

Player::Player()
{
	movementSpeed = DirectX::SimpleMath::Vector3( 300, 500, 300 );
	currentSpeed = DirectX::SimpleMath::Vector3( 0, 0, 0 );
	maxSpeed = DirectX::SimpleMath::Vector3( 100, 1500, 100 );

	camera = XMVectorSet( 0.0f, 250.0f, -2.0f, 0.0f);
	lookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	DefaultFoward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	DefaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	view = DirectX::XMMatrixLookAtLH(camera, lookAt, camUp);
	
	rotY = 0.0f;
	rotX = 0.0f;

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;

	movedX = false;
	movedZ = false;
	isAirborne = false;
}

Player::~Player()
{
}

void Player::move(DirectX::SimpleMath::Vector3 movement, double dt)
{
	XMVECTOR s = movement;
	XMVECTOR l = lookAt;
	XMVECTOR p = camera;

	camera = XMVectorMultiplyAdd(s, l, p);
	lookAt = operator+(camera, lookAt);

	view = XMMatrixLookAtLH(camera, lookAt, camUp);
}

void Player::update(double dt)
{

	
}

void Player::jump(double dt)
{
	if (isAirborne)
		return;

	currentSpeed.y = movementSpeed.y;
}

void Player::rotate(float x, float y, double dt)
{

}