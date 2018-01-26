#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "Rocket.h"

const int NUM_ROCKETS = 100;

float distanceBetweenVecs(sf::Vector2f vector1, sf::Vector2f vector2);
void fitnessSort(Rocket* rockets[]);
void createNextGen(Rocket* rockets[]);

int main()
{
	srand(time(NULL));

	sf::RenderWindow window(sf::VideoMode(600, 900), "Smart Rockets");
	sf::Vector2f goal(300.0f, 25.0f);
	sf::Clock refresh;
	sf::Clock enginesOff;
	int elapsedFrames = 0;
	int generation = 1;
	float distance;

	Rocket* rockets[NUM_ROCKETS];

	for (int i = 0; i < NUM_ROCKETS; i++)
	{
		rockets[i] = new Rocket(sf::Vector2f(10.0f, 30.0f), sf::Vector2f(300.0f, 850.0f), rand());
	}

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		if (refresh.getElapsedTime().asSeconds() > 0.016) 
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
			}

			elapsedFrames++;
		}

		if (elapsedFrames > 1200)
		{
			elapsedFrames = 0;
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

		for (int i = 0; i < NUM_ROCKETS; i++)
			rockets[i]->draw(&window);

		window.display();
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

	for (int i = NUM_ROCKETS - 1; i > NUM_ROCKETS / 2; i--)
	{
		rockets[i]->copyInstructions((*rockets[copySourceIndex]));
		copySourceIndex++;
	}
}
