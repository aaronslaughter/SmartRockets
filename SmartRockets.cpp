#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "Rocket.h"

const int NUM_ROCKETS = 40;

int main()
{
	srand(time(NULL));

	sf::RenderWindow window(sf::VideoMode(600, 900), "Smart Rockets");

	sf::Clock refresh;
	sf::Clock enginesOff;

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

		if (refresh.getElapsedTime().asSeconds() > 0.016) // 60FPS
		{
			refresh.restart();
			for (int i = 0; i < NUM_ROCKETS; i++)
			{
				rockets[i]->executeNextInst();
			}	
			
		}		

		window.clear();

		for (int i = 0; i < NUM_ROCKETS; i++)
		{
			rockets[i]->draw(&window);
		}

		window.display();
	}

	for (int i = 0; i < NUM_ROCKETS; i++)
		delete rockets[i];

	return 0;
}