#include "Player.h"

Player::Player()
{
	movementSpeed = DirectX::SimpleMath::Vector3( 3, 5, 3 );
	currentSpeed = DirectX::SimpleMath::Vector3( 0, 0, 0 );
	maxSpeed = DirectX::SimpleMath::Vector3( 1, 1500, 1 );

	camera = DirectX::SimpleMath::Vector3( 0, 0, -2 );
	lookAt = DirectX::SimpleMath::Vector3( 0, 0, 0 );

	view = DirectX::XMMatrixLookAtLH(camera, lookAt, DirectX::XMVECTOR{ 0, 1, 0 });

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
	DirectX::XMMATRIX RotateY = DirectX::XMMatrixRotationX(rotY);
	DirectX::XMVECTOR camForward = DirectX::XMVector3TransformCoord(DirectX::XMVECTOR{ 0, 0 , 1 }, RotateY);
	camera += DirectX::XMVectorScale(DirectX::operator*(currentSpeed, camForward), dt);

	camera += dt*currentSpeed;
	lookAt += dt*currentSpeed;

	if (camera.y > 0)
	{
		isAirborne = true;
		currentSpeed.y -= dt*9.81;
	}
	else if(camera.y <= 0)
	{
		isAirborne = false;
		currentSpeed.y = 0;
		camera.y = 0;
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

	view = DirectX::XMMatrixLookAtLH(camera, lookAt, DirectX::XMVECTOR{ 0, 1, 0 });
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
	if (rotY > 0.5)
		rotY = 0.5;
	if (rotY < -0.25)
		rotY = -0.25;
	rotX += x*dt;

	DirectX::XMMATRIX rotationMatrixY = DirectX::XMMatrixRotationX(rotY);
	DirectX::XMMATRIX rotationMatrixX = DirectX::XMMatrixRotationY(rotX);

	DirectX::XMMATRIX rotationMatrix = rotationMatrixY*rotationMatrixX;

	DirectX::XMVECTOR transformedReference = DirectX::XMVector3Transform(DirectX::XMVECTOR{ 0, 0, 1 }, rotationMatrix);

	lookAt = DirectX::operator+(camera, transformedReference);
	view = DirectX::XMMatrixLookAtLH(camera, lookAt, DirectX::XMVECTOR{ 0, 1, 0 });
}