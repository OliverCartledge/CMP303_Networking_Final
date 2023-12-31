//https://www.sfml-dev.org/documentation/2.6.0/classsf_1_1SocketSelector.php

#include <iostream>
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <chrono>
#include <box2d/box2d.h>

#include<thread>
#include<vector>

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


sf::RenderWindow* window;


void sendtoServer()
{
	client.send(packetSend);
	packetSend.clear();
}
void recievefromServer()
{
	packetRecieved.clear();
	if (client.receive(packetRecieved) == sf::Socket::Done) {
		recieved = true;
	}
	else
	{
		recieved = false;
	}
}

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
	sendtoServer();
	recievefromServer();
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

	sf::RectangleShape rectangles[2];
	rectangles[0].setSize(sf::Vector2f(128.0f, 128.0f));
	rectangles[0].setFillColor(sf::Color::Red);
	rectangles[0].setPosition(320, 240);

	rectangles[1].setSize(sf::Vector2f(128.0f, 128.0f));
	rectangles[1].setFillColor(sf::Color::Red);
	rectangles[1].setPosition(640, 240);

	while (window.isOpen())
	{
		deltaTime = clock.restart().asSeconds();
		sf::Event event;
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
			}
		}

		tickTimer += deltaTime;

		//std::cout << tickTimer << endl;

		//if (tickTimer > 0.05) {
			packetSend << playerID;

			packetSend << rectangles[playerID].getPosition().x;
			packetSend << rectangles[playerID].getPosition().y;

			sendtoServer();

			tickTimer = 0;
	//	}

		recievefromServer();

		if (recieved)
		{
			int id;
			float x, y;
			packetRecieved >> id >> x >> y;
			rectangles[id].setPosition(x, y);
		}

		//if (recieved) {
		//	int id;
		//	float x, y;
		//	packetRecieved >> id >> x >> y;
		//	sf::Vector2f next{ x, y };
		//	Data NetMs;
		//	NetMs.id = id;
		//	NetMs.player_position = next;
		//	NetMs.time = time;
		//	Data LocalMs;
		//	LocalMs.id = id;
		//	LocalMs.player_position = rectangles[id].getPosition();
		//	rectangles[id].prediction.NetworkPositionData(NetMs);
		//	rectangles[id].prediction.LocalPositionData(LocalMs);
		//	if (rectangles[id].prediction.NetworkDataHistory.size() == rectangles[id].prediction.quadratic_message_number && rectangles[id].prediction.LocalDataHistory.size() == rectangles[id].prediction.quadratic_message_number)
		//	{

		//		sf::Vector2f m0_local(rectangles[id].prediction.LocalDataHistory.at(0).player_position);
		//		sf::Vector2f m1_local(rectangles[id].prediction.LocalDataHistory.at(1).player_position);
		//		sf::Vector2f m2_local(rectangles[id].prediction.LocalDataHistory.at(2).player_position);

		//		sf::Vector2f m0_network(rectangles[id].prediction.NetworkDataHistory.at(0).player_position);
		//		sf::Vector2f m1_network(rectangles[id].prediction.NetworkDataHistory.at(1).player_position);
		//		sf::Vector2f m2_network(rectangles[id].prediction.NetworkDataHistory.at(2).player_position);

		//		float m0_time = rectangles[id].prediction.LocalDataHistory.at(0).time;
		//		float m1_time = rectangles[id].prediction.LocalDataHistory.at(1).time;
		//		float m2_time = rectangles[id].prediction.LocalDataHistory.at(2).time;

		//		//float currentTime = getTime();

		//		sf::Vector2f newPos = rectangles[id].prediction.quadraticInterpolation(m0_local, m1_local, m2_local, m0_network, m1_network, m2_network, m0_time, m1_time, m2_time, currentTime);
		//		rectangles[id].car.setPosition(newPos);

		//		LocalMs.id = id;
		//		LocalMs.player_position = newPos;
		//		LocalMs.time = getTime();

		//	}


		//Render the scene
		window.clear();
		window.draw(floor);
		for (int i = 0; i < 2; i++)
		{
			window.draw(rectangles[i]);
		}
		window.display();

		}
		
	

	return 0;
}