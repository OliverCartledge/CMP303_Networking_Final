//https://www.sfml-dev.org/documentation/2.6.0/classsf_1_1SocketSelector.php

#include <iostream>
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <chrono>
#include <box2d/box2d.h>

#include<thread>
#include<vector>

#include "Prediction.h"

int width = 1024;
int height = 768;
using namespace std;

int playerID;

//Networking varibales 
sf::IpAddress ip = sf::IpAddress::getLocalAddress();
//sf::IpAddress ip;
sf::TcpSocket client;
sf::Packet packetSend, packetRecieved;
std::thread recieveThread;
bool recieved = false;

//Timer stuff

float timer = 0;
float sendTime = 0;
float tickTimer = 0;

int predictionType = 1;


sf::RenderWindow* window;


// Begins rendering to the back buffer. Background colour set to light blue.
void beginDraw()
{
	window->clear(sf::Color::Black);
}

// Ends rendering to the back buffer, and swaps buffer to the screen.
void endDraw()
{
	window->display();
}


void render()
{
	beginDraw();

	//map texture 
	sf::Texture mapText;
	sf::Sprite map;

	//loading map
	mapText.loadFromFile("media/controlsScreen.png");
	mapText.setSmooth(true);
	map.setTexture(mapText);
	map.scale(2, 2);
	// define a 120x50 rectangle
	sf::RectangleShape rectangle(sf::Vector2f(120, 50));
	window->draw(rectangle);

	endDraw();
}


int main()
{
	sf::RenderWindow window(sf::VideoMode(960, 540), "CMP303");
	window.setFramerateLimit(60);



	// Initialise objects for delta time
	sf::Clock clock;
	float deltaTime;
	int frame_rate;

	float tickTimer = 0;

	//connecting to server and getting playerId 
	std::string name;
	std::string IP;
	std::cout << "Enter IP: ";
	std::cin >> IP;
	ip = IP;
	std::cout << "\nConnected to " << ip.toString() << "\n";
	while (client.connect(ip, 2000) != sf::Socket::Done) {
		std::cout << "Failed to connect, trying again...\n";
	}
	std::cout << "connected to server\n";
	std::cout << "Enter Player Name: ";
	std::cin >> name;
	packetSend << name;

	client.send(packetSend);
	packetSend.clear();

	packetRecieved.clear();
	client.receive(packetRecieved) == sf::Socket::Done;

	packetRecieved >> playerID;

	std::cout << "\nyou are player: " << playerID << endl;
	playerID = playerID - 1;

	client.setBlocking(false);

	//Initialise the background texture and sprite
	sf::Texture floorTexture;
	sf::Sprite floor;
	floorTexture.loadFromFile("media/controlsScreen.png");
	floorTexture.setRepeated(true);
	floor.setTexture(floorTexture);
	floor.setTextureRect(sf::IntRect(0, 0, 960, 540));

	sf::RectangleShape rectangles[4];
	Prediction prediction[4];

	rectangles[0].setSize(sf::Vector2f(128.0f, 128.0f));
	rectangles[0].setFillColor(sf::Color::Red);
	rectangles[0].setPosition(320, 140);

	rectangles[1].setSize(sf::Vector2f(128.0f, 128.0f));
	rectangles[1].setFillColor(sf::Color::Red);
	rectangles[1].setPosition(640, 140);

	rectangles[2].setSize(sf::Vector2f(128.0f, 128.0f));
	rectangles[2].setFillColor(sf::Color::Red);
	rectangles[2].setPosition(320, 380);

	rectangles[3].setSize(sf::Vector2f(128.0f, 128.0f));
	rectangles[3].setFillColor(sf::Color::Red);
	rectangles[3].setPosition(640, 380);

	while (window.isOpen())
	{
		deltaTime = clock.restart().asSeconds();
		sf::Event event;
		sendTime += deltaTime;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Key::Escape)
					window.close();
				if (event.key.code == sf::Keyboard::Key::R) {
					std::cout << "--------RESET-------- PLAYER" << playerID << endl;
				}
				if (event.key.code == sf::Keyboard::Key::W)
				{
					rectangles[playerID].setPosition(rectangles[playerID].getPosition().x, rectangles[playerID].getPosition().y - 10);
				}
				if (event.key.code == sf::Keyboard::Key::S)
				{
					rectangles[playerID].setPosition(rectangles[playerID].getPosition().x, rectangles[playerID].getPosition().y + 10);
				}
				if (event.key.code == sf::Keyboard::Key::A)
				{
					rectangles[playerID].setPosition(rectangles[playerID].getPosition().x - 10, rectangles[playerID].getPosition().y);
				}
				if (event.key.code == sf::Keyboard::Key::D)
				{
					rectangles[playerID].setPosition(rectangles[playerID].getPosition().x + 10, rectangles[playerID].getPosition().y);
				}
				if (event.key.code == sf::Keyboard::Key::Z)
				{
					predictionType = 1;
				}
			}
		}

		tickTimer += deltaTime;

		//std::cout << tickTimer << endl;

		//added tick timer to limit the frequency of positions being sent to reduce lag between clients
		if (tickTimer > 0.05) {
			packetSend << playerID;

			packetSend << rectangles[playerID].getPosition().x;
			packetSend << rectangles[playerID].getPosition().y;
			packetSend << sendTime;

			client.send(packetSend);
			packetSend.clear();

			tickTimer = 0;
		}

		packetRecieved.clear();

		if (client.receive(packetRecieved) == sf::Socket::Done)
			{
					if (predictionType == 0)
					{
						int id;
					float x, y;
					packetRecieved >> id >> x >> y;
					rectangles[id].setPosition(x, y);
					}
				
					if (predictionType == 1)
					{
						int id, lapCount;
						float x, y, rotation, time;
						packetRecieved >> id >> x >> y >> time;
						sf::Vector2f next{ x, y };
						Data NetMs;
						NetMs.id = id;
						NetMs.playerPosition = next;
						NetMs.time = time;
						Data LocalMs;
						LocalMs.id = id;
						LocalMs.playerPosition = rectangles[id].getPosition();
						LocalMs.time = sendTime;
						prediction[id].NetworkPositionData(NetMs);
						prediction[id].LocalPositionData(LocalMs);
						//If there are 3 positions added into the queue then use those values to predict 
						if (prediction[id].networkMovement.size() == prediction[id].quadratic_message_number && prediction[id].localMovement.size() == prediction[id].quadratic_message_number)
						{
							// Set the 3 most recent local positions
							sf::Vector2f localMovement0(prediction[id].localMovement.at(0).playerPosition);
							sf::Vector2f localMovement1(prediction[id].localMovement.at(1).playerPosition);
							sf::Vector2f localMovement2(prediction[id].localMovement.at(2).playerPosition);

							// Set the 3 most recent networked positions
							sf::Vector2f networkMovement0(prediction[id].networkMovement.at(0).playerPosition);
							sf::Vector2f networkMovement1(prediction[id].networkMovement.at(1).playerPosition);
							sf::Vector2f networkMovement2(prediction[id].networkMovement.at(2).playerPosition);

							// Set the time for calculating the quadratic position 
							float time0 = prediction[id].localMovement.at(0).time;
							float time1 = prediction[id].localMovement.at(1).time;
							float time2 = prediction[id].localMovement.at(2).time;

							sf::Vector2f newPos = prediction[id].quadraticInterpolation(localMovement0, localMovement1, localMovement2, networkMovement0, networkMovement1, networkMovement2, time0, time1, time2, sendTime);
							rectangles[id].setPosition(newPos);

							LocalMs.id = id;
							LocalMs.playerPosition = newPos;
							LocalMs.time = sendTime;
						}
					}
			}

			//Render the scene
			window.clear();
			window.draw(floor);
			for (int i = 0; i < 4; i++)
			{
				window.draw(rectangles[i]);
			}
			window.display();

		}
		return 0;
	}