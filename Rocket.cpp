#include "Rocket.h"

Rocket::Rocket(sf::Vector2f size, sf::Vector2f startPos, int ranSeed) : 
	rocketBody(size),
	mainThrustFire(sf::Vector2f(size.x / 1.5f, size.y / 1.5f)),
	leftThrustFire(sf::Vector2f(size.x / 3.0f, size.y / 3.0f)),
	rightThrustFire(sf::Vector2f(size.x / 3.0f, size.y / 3.0f))
{
	srand(ranSeed);

	velocity.x = 0;
	velocity.y = 0;
	angularVelocity = 0;
	heading = rocketBody.getRotation();

	rocketBody.setOrigin(sf::Vector2f(rocketBody.getSize().x / 2.0f, rocketBody.getSize().y / 2.0f));
	rocketBody.setPosition(startPos);
	rocketBody.setFillColor(sf::Color::Green);

	mainThrustFire.setFillColor(sf::Color::Red);
	leftThrustFire.setFillColor(sf::Color::Red);
	rightThrustFire.setFillColor(sf::Color::Red);

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

Rocket::Rocket(const Rocket& rocket)
{

}

void Rocket::thrustMain()
{
	// SFML treats 0 degress as the top of the unit circle, so heading has to be offset
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
	applyGravity();

	heading += angularVelocity;
	rocketBody.setRotation(heading);

	sf::Vector2f newPosition(rocketBody.getPosition().x, rocketBody.getPosition().y);
	newPosition.x += velocity.x;
	newPosition.y += velocity.y;
	rocketBody.setPosition(newPosition);
	
	setThrusterPositions();
}

void Rocket::draw(sf::RenderWindow* window)
{
	if (currentInstruction < NUM_INSTRUCTIONS)
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
	x = (rocketBody.getSize().y / 1.5) * cos((heading + 90) * M_PI / 180.0);
	y = (rocketBody.getSize().y / 1.5) * sin((heading + 90) * M_PI / 180.0);

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
	if (currentInstruction < NUM_INSTRUCTIONS)
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