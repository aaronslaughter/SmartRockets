#include "Rocket.h"

Rocket::Rocket(sf::Vector2f size, sf::Vector2f startPos, int ranSeed, sf::Texture* rocketTexture, sf::Texture* thrusterTexture) :
	rocketBody(size),
	startPos(startPos),
	mainThrustFire(sf::Vector2f(size.x / 1.5f, size.y / 1.5f)),
	leftThrustFire(sf::Vector2f(size.x / 3.0f, size.y / 3.0f)),
	rightThrustFire(sf::Vector2f(size.x / 3.0f, size.y / 3.0f))
{
	srand(ranSeed);

	crashed = false;
	fitness = 1000.0;
	velocity.x = 0;
	velocity.y = 0;
	angularVelocity = 0;
	heading = rocketBody.getRotation();

	rocketBody.setOrigin(sf::Vector2f(rocketBody.getSize().x / 2.0f, rocketBody.getSize().y / 2.0f));
	rocketBody.setPosition(startPos);
	rocketBody.setTexture(rocketTexture);

	mainThrustFire.setTexture(thrusterTexture);
	leftThrustFire.setTexture(thrusterTexture);
	rightThrustFire.setTexture(thrusterTexture);

	mainThrustFire.setOrigin(sf::Vector2f(mainThrustFire.getSize().x / 2.0f, mainThrustFire.getSize().y / 2.0f));
	leftThrustFire.setOrigin(sf::Vector2f(leftThrustFire.getSize().x / 2.0f, leftThrustFire.getSize().y / 2.0f));
	rightThrustFire.setOrigin(sf::Vector2f(rightThrustFire.getSize().x / 2.0f, rightThrustFire.getSize().y / 2.0f));

	setThrusterPositions();

	currentInstruction = 0;

	for (int i = 0; i < NUM_INSTRUCTIONS; i++)
	{
		if (rand() % 2 == 0)
			mainThrustInst[i] = true;
		else
			mainThrustInst[i] = false;

		if (rand() % 2 == 0)
			leftThrustInst[i] = true;
		else
			leftThrustInst[i] = false;

		if (rand() % 2 == 0)
			rightThrustInst[i] = true;
		else
			rightThrustInst[i] = false;
	}

}

void Rocket::thrustMain()
{
	// SFML treats 0 degrees as the top of the unit circle, so heading has to be offset
	velocity.x -= MAIN_THRUSTER * cos((heading + 90) * M_PI / 180);
	velocity.y -= MAIN_THRUSTER * sin((heading + 90) * M_PI / 180);
}

void Rocket::thrustLeft()
{
	angularVelocity += ANGULAR_THRUST;
}

void Rocket::thrustRight()
{
	angularVelocity -= ANGULAR_THRUST;
}

void Rocket::applyGravity()
{
	velocity.y += GRAVITY;
}


void Rocket::updatePosition()
{
	if (rocketBody.getPosition().x < 0 || rocketBody.getPosition().x > 600)
		crashed = true;

	if (rocketBody.getPosition().y < 0 || rocketBody.getPosition().y > 900)
		crashed = true;

	if (!crashed)
	{
		applyGravity();

		heading += angularVelocity;
		rocketBody.setRotation(heading);

		sf::Vector2f newPosition(rocketBody.getPosition().x, rocketBody.getPosition().y);
		newPosition.x += velocity.x;
		newPosition.y += velocity.y;
		rocketBody.setPosition(newPosition);

		setThrusterPositions();
	}
}

void Rocket::draw(sf::RenderWindow* window)
{
	if (currentInstruction < NUM_INSTRUCTIONS && !crashed)
	{
		if (mainThrustInst[currentInstruction])
			window->draw(mainThrustFire);

		if (leftThrustInst[currentInstruction])
			window->draw(leftThrustFire);

		if (rightThrustInst[currentInstruction])
			window->draw(rightThrustFire);
	}

	window->draw(rocketBody);
}

void Rocket::setThrusterPositions()
{
	float x, y;
	x = (rocketBody.getSize().y / 1.4) * cos((heading + 90) * M_PI / 180.0);
	y = (rocketBody.getSize().y / 1.4) * sin((heading + 90) * M_PI / 180.0);

	mainThrustFire.setPosition(rocketBody.getPosition());
	mainThrustFire.move(sf::Vector2f(x, y));
	mainThrustFire.setRotation(heading);

	x = (rocketBody.getSize().y / 3.0) * cos((heading - 135) * M_PI / 180.0);
	y = (rocketBody.getSize().y / 3.0) * sin((heading - 135) * M_PI / 180.0);

	leftThrustFire.setPosition(rocketBody.getPosition());
	leftThrustFire.move(sf::Vector2f(x, y));
	leftThrustFire.setRotation(heading + 90);

	x = (rocketBody.getSize().y / 3.0) * cos((heading - 45) * M_PI / 180.0);
	y = (rocketBody.getSize().y / 3.0) * sin((heading - 45) * M_PI / 180.0);

	rightThrustFire.setPosition(rocketBody.getPosition());
	rightThrustFire.move(sf::Vector2f(x, y));
	rightThrustFire.setRotation(heading - 90);
}

void Rocket::executeNextInst()
{
	if (currentInstruction < NUM_INSTRUCTIONS && !crashed)
	{
		if (mainThrustInst[currentInstruction])
			thrustMain();

		if (leftThrustInst[currentInstruction])
			thrustLeft();

		if (rightThrustInst[currentInstruction])
			thrustRight();
	}

	updatePosition();
	currentInstruction++;
}

sf::Vector2f Rocket::getPosition()
{
	sf::Vector2f pos = rocketBody.getPosition();
	return pos;
}

void Rocket::reset()
{
	rocketBody.setPosition(startPos);
	rocketBody.setRotation(0);
	heading = rocketBody.getRotation();
	velocity.x = 0;
	velocity.y = 0;
	angularVelocity = 0;
	currentInstruction = 0;
	fitness = 1000;
	crashed = false;
}

void Rocket::copyInstructions(const Rocket& sourceRocket)
{
	for (int i = 0; i < NUM_INSTRUCTIONS; i++)
	{
		this->mainThrustInst[i] = sourceRocket.mainThrustInst[i];
		this->leftThrustInst[i] = sourceRocket.leftThrustInst[i];
		this->rightThrustInst[i] = sourceRocket.rightThrustInst[i];
	}
}

void Rocket::mutate()
{
	int instructionIndex, instructionType;

	for (int i = 0; i < MUTATION_RATE; i++)
	{
		instructionIndex = rand() % NUM_INSTRUCTIONS;
		instructionType = rand() % 3;

		switch (instructionType)
		{
		case 1:	mainThrustInst[instructionIndex] ? mainThrustInst[instructionIndex] = false 
			: mainThrustInst[instructionIndex] = true;
			break;
		case 2: leftThrustInst[instructionIndex] ? leftThrustInst[instructionIndex] = false 
			: leftThrustInst[instructionIndex] = true;
			break;
		case 3:	rightThrustInst[instructionIndex] ? rightThrustInst[instructionIndex] = false 
			: rightThrustInst[instructionIndex] = true;
			break;
		default:
			break;
		}
	}
}

//////////////////
// Returns a ConvexShape with the global coordinates of the rocketBody
// with the rotation factored in. RectangleShape.getPoint() does not
// adjust for rotation. Useful for SAT collision detection.
// Assumes rocketBody is a rectangle and the origin is set
// to the center.
//////////////////
sf::ConvexShape Rocket::getRotatedShape()
{
	sf::ConvexShape rotatedShape(4);
	sf::Vector2f topVector, rightVector, bottomVector, leftVector;
	float halfHeight = rocketBody.getSize().y / 2.0;
	float halfWidth = rocketBody.getSize().x / 2.0;
	float rotation = rocketBody.getRotation() + 90;
	
	topVector.x = halfHeight * cos(rotation * M_PI / 180.0);
	topVector.y = halfHeight * sin(rotation * M_PI / 180.0);

	bottomVector.x = topVector.x * -1;
	bottomVector.y = topVector.y * -1;

	rightVector.y = halfWidth * cos((360 - rotation) * M_PI / 180.0);
	rightVector.x = halfWidth * sin((360 - rotation) * M_PI / 180.0);

	leftVector.x = rightVector.x * -1;
	leftVector.y = rightVector.y * -1;

	rotatedShape.setPoint(0, rocketBody.getPosition() + topVector + rightVector);
	rotatedShape.setPoint(1, rocketBody.getPosition() + bottomVector + rightVector);
	rotatedShape.setPoint(2, rocketBody.getPosition() + bottomVector + leftVector);
	rotatedShape.setPoint(3, rocketBody.getPosition() + topVector + leftVector);

	return rotatedShape;
}
