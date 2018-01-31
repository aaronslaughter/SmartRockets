#include <SFML\Graphics.hpp>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

class Rocket
{
public:
	const float GRAVITY  = 0.012;
	const float MAIN_THRUSTER = 0.025;
	const float ANGULAR_THRUST = 0.009;
	const static int NUM_INSTRUCTIONS = 1000;
	const int MUTATION_RATE = NUM_INSTRUCTIONS / 100;

public:
	Rocket(sf::Vector2f size, sf::Vector2f startPos, int ranSeed, sf::Texture* rocketTexture, sf::Texture* thrusterTexture);
	void executeNextInst();
	sf::Vector2f getPosition();
	void reset();
	void draw(sf::RenderWindow* window);
	void copyInstructions(const Rocket& sourceRocket);
	void mutate();

public:
	float fitness;

private:
	void thrustMain();
	void thrustLeft();
	void thrustRight();
	void applyGravity();
	void updatePosition();
	void setThrusterPositions();

private:
	sf::Vector2f startPos;
	sf::RectangleShape rocketBody;
	sf::RectangleShape mainThrustFire;
	sf::RectangleShape leftThrustFire;
	sf::RectangleShape rightThrustFire;
	sf::Vector2f velocity;
	double angularVelocity;
	double heading;
	int currentInstruction;
	bool mainThrustInst[NUM_INSTRUCTIONS];
	bool leftThrustInst[NUM_INSTRUCTIONS];
	bool rightThrustInst[NUM_INSTRUCTIONS];
	bool crashed;
};
