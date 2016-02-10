#include "Player.h"

Player::Player()
{
	movementSpeed = DirectX::SimpleMath::Vector3( 300, 500, 300 );
	currentSpeed = DirectX::SimpleMath::Vector3( 0, 0, 0 );
	maxSpeed = DirectX::SimpleMath::Vector3( 100, 1500, 100 );

	camera = XMVectorSet( 0.0f, 50.0f, -2.0f, 0.0f);
	lookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	DefaultFoward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	DefaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	view = DirectX::XMMatrixLookAtLH(camera, lookAt, DirectX::XMVECTOR{ 0.0f, 1.0f, 0.0f });
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
	movement.y = 0;

	currentSpeed += dt*movementSpeed*movement;

	if (movement.x != 0)
		movedX = true;
	if (movement.z != 0)
		movedZ = true;

	if (currentSpeed.x > maxSpeed.x)
		currentSpeed.x = maxSpeed.x;
	if (currentSpeed.z > maxSpeed.z)
		currentSpeed.z = maxSpeed.z;
}

void Player::update(double dt)
{

	camRotationMatrix = XMMatrixRotationRollPitchYaw(rotX, rotY, 0.0f);

	//DirectX::XMMATRIX RotateY = DirectX::XMMatrixRotationY(rotY);
	lookAt = DirectX::XMVector3TransformCoord(DefaultFoward, camRotationMatrix);
	lookAt = DirectX::XMVector3Normalize(lookAt);

	XMMATRIX RotateYTempMatrix;
	RotateYTempMatrix = XMMatrixRotationY(rotX);

	camRight = XMVector3TransformCoord(DefaultRight, RotateYTempMatrix);

	camUp = XMVector3TransformCoord(camUp, RotateYTempMatrix);
	camForward = XMVector3TransformCoord(DefaultFoward, RotateYTempMatrix);

	camera += moveLeftRight*camRight;
	camera += moveBackForward*camForward;

	moveBackForward = 0.0f;
	moveLeftRight = 0.0f;

	lookAt = camera + lookAt;

	view = XMMatrixLookAtLH(camera, lookAt, camUp);
	//camera += dt*currentSpeed;
	//lookAt += dt*currentSpeed;

	if (XMVectorGetY(camera) > 0)
	{
		isAirborne = true;
		currentSpeed.y -= dt*9.81;
	}
	else if(XMVectorGetY(camera) <= 0)
	{
		isAirborne = false;
		currentSpeed.y = 0;
		XMVectorSetY(camera, 0.0f);
		//lookAt.y = 0;
	}
		
	if (currentSpeed.x > 0 && !movedX)
		currentSpeed.x = 0;
	else if (currentSpeed.x < 0 && !movedX)
		currentSpeed.x = 0;
	if (currentSpeed.z > 0 && !movedZ)
		currentSpeed.z = 0;
	else if (currentSpeed.z < 0 && !movedZ)
		currentSpeed.z = 0;

	movedX = false;
	movedZ = false;

	view = XMMatrixLookAtLH(camera, lookAt, camUp);
}

void Player::jump(double dt)
{
	if (isAirborne)
		return;

	currentSpeed.y = movementSpeed.y;
}

void Player::rotate(float x, float y, double dt)
{
	rotY += y*dt;
	//if (rotY > 0.5)
		//rotY = 0.5;
	//if (rotY < -0.25)
		//rotY = -0.25;
	rotX += x*dt;

	camRotationMatrix = XMMatrixRotationRollPitchYaw(rotX, rotY, 0.0f);

	DirectX::XMVECTOR transformedReference = DirectX::XMVector3TransformCoord(camForward, camRotationMatrix);

	lookAt = DirectX::operator+(camera, transformedReference);
	view = DirectX::XMMatrixLookAtLH(camera, lookAt, camUp);
}