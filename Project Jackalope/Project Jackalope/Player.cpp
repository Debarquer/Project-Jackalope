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
	rotationAngle = 1;

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;

	movedX = false;
	movedZ = false;
	isAirborne = false;
}

Player::~Player()
{
}

void Player::move(float d)
{
	DirectX::XMVECTOR s = XMVectorReplicate(d);
	DirectX::XMVECTOR l = lookAt;
	DirectX::XMVECTOR p = camera;

	//DirectX::XMVECTOR sl = s*l;
	//DirectX::XMVECTOR p2 = p + sl;
	//p = p2;
	camera = DirectX::XMVectorMultiplyAdd(s, l, p);
	//lookAt = DirectX::XMVectorMultiplyAdd(s, l, l);

	view = DirectX::XMMatrixLookToLH(camera, lookAt, camUp);
}
void Player::strafe(float d)
{
	DirectX::XMVECTOR s = XMVectorReplicate(d);
	DirectX::XMVECTOR r = camRight;
	DirectX::XMVECTOR p = camera;

	//DirectX::XMVECTOR sr = s*r;
	//DirectX::XMVECTOR p2 = p + sr;
	//p = p2;

	camera = DirectX::XMVectorMultiplyAdd(s, r, p);

	view = DirectX::XMMatrixLookToLH(camera, lookAt, camUp);
}

void Player::pitch(float direction, double dt)
{
	XMMATRIX R = XMMatrixRotationAxis(camRight, direction * rotationAngle * dt);
	camUp = XMVector3TransformNormal(camUp, R);
	lookAt = XMVector3TransformNormal(lookAt, R);
}

void Player::yaw(float direction, double dt)
{
	XMMATRIX R = XMMatrixRotationY(direction * rotationAngle  * dt);
	camRight = XMVector3TransformNormal(camRight, R);
	lookAt = XMVector3TransformNormal(lookAt, R);
}

void Player::update()
{
	XMVECTOR R = camRight;
	XMVECTOR U = camUp;
	XMVECTOR L = lookAt;
	XMVECTOR P = camera;

	L = XMVector3Normalize(L);
	U = XMVector3Normalize(XMVector3Cross(L, R));
	R = XMVector3Cross(U, L);
	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));
	
	camRight = R;
	camUp = U;
	lookAt = L;

	view = XMMatrixLookToLH(camera, lookAt, camUp);
}

void Player::jump(double dt)
{
	if (isAirborne)
		return;

	currentSpeed.y = movementSpeed.y;
}