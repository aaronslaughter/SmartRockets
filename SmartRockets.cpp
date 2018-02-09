#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "Rocket.h"

const int NUM_ROCKETS = 30;


float distanceBetweenVecs(sf::Vector2f vector1, sf::Vector2f vector2);
void fitnessSort(Rocket* rockets[]);
void createNextGen(Rocket* rockets[]);
bool checkIntersection(sf::ConvexShape shape1, sf::ConvexShape shape2);
float projectVectorToAxis(sf::Vector2f sourceVector, sf::Vector2f axis);
float vectorMagnitude(sf::Vector2f vector);

int main()
{
	srand(time(NULL));

	sf::ConvexShape target(4);
	target.setPoint(0, sf::Vector2f(300, 0));
	target.setPoint(1, sf::Vector2f(325, 25));
	target.setPoint(2, sf::Vector2f(300, 50));
	target.setPoint(3, sf::Vector2f(275, 25));
	target.setFillColor(sf::Color::Blue);

	sf::ConvexShape obstacle1(4);
	obstacle1.setPoint(0, sf::Vector2f(300, 500));
	obstacle1.setPoint(1, sf::Vector2f(350, 600));
	obstacle1.setPoint(2, sf::Vector2f(300, 625));
	obstacle1.setPoint(3, sf::Vector2f(250, 600));
	obstacle1.setFillColor(sf::Color::Red);

	sf::ConvexShape obstacle2(3);
	obstacle2.setPoint(0, sf::Vector2f(0, 0));
	obstacle2.setPoint(1, sf::Vector2f(250, 0));
	obstacle2.setPoint(2, sf::Vector2f(0, 500));
	obstacle2.setFillColor(sf::Color::Red);

	sf::ConvexShape obstacle3(3);
	obstacle3.setPoint(0, sf::Vector2f(600, 0));
	obstacle3.setPoint(1, sf::Vector2f(350, 0));
	obstacle3.setPoint(2, sf::Vector2f(600, 500));
	obstacle3.setFillColor(sf::Color::Red);

	sf::Texture rocketTexture;
	sf::Texture thrusterTexture;
	sf::RenderWindow window(sf::VideoMode(600, 900), "Smart Rockets");
	sf::Vector2f goal(300.0f, 25.0f);
	sf::Clock refresh;
	int elapsedFrames = 0;
	int calculatedFrames = 0;
	int generation = 1;
	float distance;

	Rocket* rockets[NUM_ROCKETS];

	rocketTexture.loadFromFile("resources/sprites/rocket.png");
	thrusterTexture.loadFromFile("resources/sprites/thruster.png");

	for (int i = 0; i < NUM_ROCKETS; i++)
	{
		rockets[i] = new Rocket(sf::Vector2f(20.0f, 60.0f), sf::Vector2f(300.0f, 850.0f), rand(), &rocketTexture, &thrusterTexture);
	}

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		if (refresh.getElapsedTime().asSeconds() > 0.004) 
		{
			refresh.restart();

			for (int i = 0; i < NUM_ROCKETS; i++)
			{
				rockets[i]->executeNextInst();
				distance = distanceBetweenVecs(rockets[i]->getPosition(), goal);

				if (distance < rockets[i]->fitness)
				{
					rockets[i]->fitness = distanceBetweenVecs(rockets[i]->getPosition(), goal);
				}
				
				sf::FloatRect quickCollisionBox = rockets[i]->getRotatedShape().getGlobalBounds();
				
				if (quickCollisionBox.intersects(target.getGlobalBounds()))
					if (!rockets[i]->crashed && checkIntersection(rockets[i]->getRotatedShape(), target))
						rockets[i]->crashed = true;

				if (quickCollisionBox.intersects(obstacle1.getGlobalBounds()))
					if (!rockets[i]->crashed && checkIntersection(rockets[i]->getRotatedShape(), obstacle1))
						rockets[i]->crashed = true;

				if (quickCollisionBox.intersects(obstacle2.getGlobalBounds()))
					if (!rockets[i]->crashed && checkIntersection(rockets[i]->getRotatedShape(), obstacle2))
						rockets[i]->crashed = true;

				if (quickCollisionBox.intersects(obstacle3.getGlobalBounds()))
					if (!rockets[i]->crashed && checkIntersection(rockets[i]->getRotatedShape(), obstacle3))
						rockets[i]->crashed = true;

			}

			elapsedFrames++;
		}

		if (elapsedFrames > 1200)
		{
			std::cout << (float)calculatedFrames / (float)elapsedFrames << std::endl;
			elapsedFrames = 0;
			calculatedFrames = 0;
			generation++;
			fitnessSort(rockets);
			createNextGen(rockets);

			for (int i = 0; i < NUM_ROCKETS; i++)
			{
				rockets[i]->mutate();
				rockets[i]->reset();
			}
		}


		window.clear();
		window.draw(target);
		window.draw(obstacle1);
		window.draw(obstacle2);
		window.draw(obstacle3);

		for (int i = 0; i < NUM_ROCKETS; i++)
			rockets[i]->draw(&window);

		window.display();
		calculatedFrames++;
	}

	for (int i = 0; i < NUM_ROCKETS; i++)
		delete rockets[i];

	return 0;
}


float distanceBetweenVecs(sf::Vector2f vector1, sf::Vector2f vector2)
{
	float a = (vector2.x - vector1.x) * (vector2.x - vector1.x);
	float b = (vector2.y - vector1.y) * (vector2.y - vector1.y);
	return (float)sqrt(a + b);
}

void fitnessSort(Rocket* rockets[])
{
	bool flag  = true;
	Rocket* temp;

	for (int i = 0; i < NUM_ROCKETS && flag; i++)
	{
		flag = false;
		for (int j = 0; j < NUM_ROCKETS - 1; j++)
		{
			if (rockets[j + 1]->fitness < rockets[j]->fitness)
			{
				temp = rockets[j];
				rockets[j] = rockets[j + 1];
				rockets[j + 1] = temp;
				flag = true;
			}
		}
	}

}

void createNextGen(Rocket* rockets[])
{
	int copySourceIndex = 0;

	for (int i = NUM_ROCKETS - 1; i > NUM_ROCKETS / 1.2; i--)
	{
		rockets[i]->copyInstructions((*rockets[copySourceIndex]));
		copySourceIndex++;
	}
}

// SAT collision
bool checkIntersection(sf::ConvexShape shape1, sf::ConvexShape shape2)
{
	bool intersects = true;
	bool intersectingNormal = false;
	float shape1RangeMax, shape1RangeMin, shape2RangeMax, shape2RangeMin;
	sf::Vector2f pAxis;

	for (int h = 0; h < 2; h++)
	{
		for (int i = 0; i < shape1.getPointCount() && intersects; i++)
		{
			pAxis.y = shape1.getPoint((i + 1) % shape1.getPointCount()).x - shape1.getPoint(i).x;
			pAxis.x = (shape1.getPoint((i + 1) % shape1.getPointCount()).y - shape1.getPoint(i).y) * -1;

			for (int j = 0; j < shape1.getPointCount(); j++)
			{
				float temp;

				if (j == 0)
					shape1RangeMax = projectVectorToAxis(shape1.getPoint(j), pAxis);
				else if (j == 1)
				{
					temp = projectVectorToAxis(shape1.getPoint(j), pAxis);

					if (temp > shape1RangeMax)
					{
						shape1RangeMin = shape1RangeMax;
						shape1RangeMax = temp;
					}
					else
						shape1RangeMin = temp;
				}
				else
				{
					temp = projectVectorToAxis(shape1.getPoint(j), pAxis);

					if (temp > shape1RangeMax)
						shape1RangeMax = temp;
					else if (temp < shape1RangeMin)
						shape1RangeMin = temp;
				}
			}

			for (int j = 0; j < shape2.getPointCount(); j++)
			{
				float temp;

				if (j == 0)
					shape2RangeMax = projectVectorToAxis(shape2.getPoint(j), pAxis);
				else if (j == 1)
				{
					temp = projectVectorToAxis(shape2.getPoint(j), pAxis);

					if (temp > shape2RangeMax)
					{
						shape2RangeMin = shape2RangeMax;
						shape2RangeMax = temp;
					}
					else
						shape2RangeMin = temp;
				}
				else
				{
					temp = projectVectorToAxis(shape2.getPoint(j), pAxis);

					if (temp > shape2RangeMax)
						shape2RangeMax = temp;
					else if (temp < shape2RangeMin)
						shape2RangeMin = temp;
				}
			}

			if (!(shape1RangeMin <= shape2RangeMax && shape2RangeMin <= shape1RangeMax))
				intersects = false;

		}

		sf::ConvexShape temp = shape1;
		shape1 = shape2;
		shape2 = temp;
	}

	return intersects;
}

float projectVectorToAxis(sf::Vector2f sourceVector, sf::Vector2f axis)
{
	float dotProduct, scalar;

	dotProduct = (sourceVector.x * axis.x) + (sourceVector.y * axis.y);

	scalar = dotProduct / vectorMagnitude(axis);

	axis.x *= scalar;
	axis.y *= scalar;

	return vectorMagnitude(axis);

}

float vectorMagnitude(sf::Vector2f vector)
{
	return sqrt((vector.x * vector.x) + (vector.y * vector.y));
}
