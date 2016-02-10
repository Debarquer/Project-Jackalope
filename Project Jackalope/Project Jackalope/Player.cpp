#include "Player.h"

Player::Player()
{
	movementSpeed = DirectX::SimpleMath::Vector3( 300, 500, 300 );
	currentSpeed = DirectX::SimpleMath::Vector3( 0, 0, 0 );
	maxSpeed = DirectX::SimpleMath::Vector3( 100, 1500, 100 );

	camera = DirectX::XMVectorSet( 0.0f, 250.0f, -2.0f, 0.0f);
	lookAt = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	DefaultFoward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	DefaultRight = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	camForward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	camRight = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	camUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	view = DirectX::XMMatrixLookToLH(camera, lookAt, camUp);
	
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

void Player::move(XMVECTOR movement, double dt)
{
	DirectX::XMVECTOR s = movement;
	DirectX::XMVECTOR l = lookAt;
	DirectX::XMVECTOR p = camera;

	camera = DirectX::XMVectorMultiplyAdd(s, l, p);
	//lookAt = DirectX::XMVectorMultiplyAdd(s, l, l);

	view = DirectX::XMMatrixLookToLH(camera, lookAt, camUp);
}

void Player::strafe(XMVECTOR movement, double dt)
{
	DirectX::XMVECTOR s = movement;
	DirectX::XMVECTOR r = camRight;
	DirectX::XMVECTOR p = camera;

	camera = DirectX::XMVectorMultiplyAdd(s, r, p);
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