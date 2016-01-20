#pragma once

#include <DirectXMath.h>
#include "SimpleMath.h"

class Player
{
private:
	bool movedX, movedZ, isAirborne;

	DirectX::SimpleMath::Vector3 movementSpeed;
	DirectX::SimpleMath::Vector3 currentSpeed;
	DirectX::SimpleMath::Vector3 maxSpeed;
public:

	DirectX::SimpleMath::Vector3 camera;
	DirectX::SimpleMath::Vector3 lookAt;

	void move(DirectX::SimpleMath::Vector3 movement, double dt);
	void update(double dt);

	Player();
	~Player();
};

