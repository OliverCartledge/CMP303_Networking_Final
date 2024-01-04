#pragma once

#include<SFML/Graphics.hpp>
#include <deque>
	struct Data {
		int id;

		sf::Vector2f playerPosition;

		float time;
	};

	class Prediction
	{
	public:
		Prediction();
		~Prediction();

		// number of messages to keep track of
		const unsigned int quadratic_message_number = 3;

		std::deque<Data> networkMovement;
		std::deque<Data> localMovement;

		void NetworkPositionData(const Data& receivedMessage);
		void LocalPositionData(const Data& localMessage);

		sf::Vector2f quadraticInterpolation(sf::Vector2f& localMovement0,sf::Vector2f& localMovement1,sf::Vector2f& localMovement2,sf::Vector2f& networkMovement0,sf::Vector2f& networkMovement1,sf::Vector2f& networkMovement2,
			float& time0,
			float& time1,
			float& time2,
			float& time);

	protected:

		sf::Vector2f lerp(sf::Vector2f old, sf::Vector2f next, float timer) {
			return (1 - timer) * old + timer * next;
		}

	};
