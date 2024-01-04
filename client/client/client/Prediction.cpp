#include "Prediction.h"

Prediction::Prediction()
{
}


Prediction::~Prediction()
{
}

//Save 3 most recent networked positions into a queue
void Prediction::NetworkPositionData(const Data& receivedMessage)
{
	if (networkMovement.size() >= quadratic_message_number) networkMovement.pop_back();
	networkMovement.push_front(receivedMessage);
}

//Save 3 most recent positions into a queue
void Prediction::LocalPositionData(const Data& localMessage)
{
	if (localMovement.size() >= quadratic_message_number) localMovement.pop_back();
	localMovement.push_front(localMessage);
}

sf::Vector2f Prediction::quadraticInterpolation(sf::Vector2f& localMovement0, sf::Vector2f& localMovement1, sf::Vector2f& localMovement2, sf::Vector2f& networkMovement0, sf::Vector2f& networkMovement1, sf::Vector2f& networkMovement2, float& time0, float& time1, float& time2, float& time)
{
	float tm = time;

	
	float averageXVelocity1, averageYVelocity1,
		averageXVelocity2, averageYVelocity2,
		averageX, averageY,
		newX, newY;


	// Local player positions

	averageXVelocity1 = (localMovement0.x - localMovement1.x) / (time0 - time1);
	averageYVelocity1 = (localMovement0.y - localMovement1.y) / (time0 - time1);

	averageXVelocity2 = (localMovement1.x - localMovement2.x) / (time1 - time2);
	averageYVelocity2 = (localMovement1.y - localMovement2.y) / (time1 - time2);

	averageX = (averageXVelocity2 - averageXVelocity1) / (time2 - time0);
	averageY = (averageYVelocity2 - averageYVelocity1) / (time2 - time0);

	newX = localMovement2.x + (averageXVelocity2 * (time - time2)) + ((0.5 * averageX) * powf((time - time2), 2));
	newY = localMovement2.y + (averageYVelocity2 * (time - time2)) + ((0.5 * averageY) * powf((time - time2), 2));

	sf::Vector2f local_player_pos(newX, newY);

	// Network position

	averageXVelocity1 = (networkMovement0.x - networkMovement1.x) / (time0 - time1);
	averageYVelocity1 = (networkMovement0.y - networkMovement1.y) / (time0 - time1);

	averageXVelocity2 = (networkMovement1.x - networkMovement2.x) / (time1 - time2);
	averageYVelocity2 = (networkMovement1.y - networkMovement2.y) / (time1 - time2);

	averageX = (averageXVelocity2 - averageXVelocity1) / (time2 - time0);
	averageY = (averageYVelocity2 - averageYVelocity1) / (time2 - time0);

	newX = networkMovement2.x + (averageXVelocity2 * (time - time2)) + ((0.5 * averageX) * powf((time - time2), 2));
	newY = networkMovement2.y + (averageYVelocity2 * (time - time2)) + ((0.5 * averageY) * powf((time - time2), 2));

	sf::Vector2f network_player_pos(newX, newY);

	sf::Vector2f finalPosition = lerp(local_player_pos, network_player_pos, 0.1);

	return finalPosition;
}