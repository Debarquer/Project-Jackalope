#include "Player.h"

Player::Player()
{
	movementSpeed = DirectX::SimpleMath::Vector3( 3, 5, 3 );
	currentSpeed = DirectX::SimpleMath::Vector3( 0, 0, 0 );
	maxSpeed = DirectX::SimpleMath::Vector3( 1, 1500, 1 );

	camera = DirectX::SimpleMath::Vector3( 0, 0, -2 );
	lookAt = DirectX::SimpleMath::Vector3( 0, 0, 0 );

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
	//movementSpeed.y = 1 / dt;

	currentSpeed += dt*movementSpeed*movement;

	if (movement.x != 0)
		movedX = true;
	if (movement.z != 0)
		movedZ = true;

	if (currentSpeed.x > maxSpeed.x)
		currentSpeed.x = maxSpeed.x;
	//if (currentSpeed.y > maxSpeed.y)
		//currentSpeed.y = maxSpeed.y;
	if (currentSpeed.z > maxSpeed.z)
		currentSpeed.z = maxSpeed.z;
}

void Player::update(double dt)
{
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
		lookAt.y = 0;
	}
		
	if (currentSpeed.x > 0 && !movedX)
		currentSpeed.x -= dt * 8;
	else if(currentSpeed.x < 0 && !movedX)
		currentSpeed.x += dt * 8;
	if (currentSpeed.z > 0 && !movedZ)
		currentSpeed.z -= dt * 8;
	else if (currentSpeed.z < 0 && !movedZ)
		currentSpeed.z += dt * 8;

	movedX = false;
	movedZ = false;
}

void Player::jump(double dt)
{
	if (isAirborne)
		return;

	currentSpeed.y = movementSpeed.y;
}
