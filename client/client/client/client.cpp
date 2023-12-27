//#include <SFML/Network/UdpSocket.hpp>
//#include <iostream>
//using std::to_string;
//
//// The (fixed) size of message that we send between the two programs
//#define MESSAGESIZE 40
//
//int main()
//{
//	// Create a socket and bind it to the port 55001
//	sf::UdpSocket socket;
//	socket.bind(55001);
//
//	int test = 0;
//
//	char buffer2[MESSAGESIZE];
//
//	do
//	{
//		//Testing adding to one and the other at the same time
//		//Test push
//		// Send a message to 192.168.1.50 on port 55002
//		std::string message = "Hi, I am " + sf::IpAddress::getLocalAddress().toString();
//		socket.send(message.c_str(), message.size() + 1, sf::IpAddress::getLocalAddress().toString(), 55002);
//
//		// Receive an answer (most likely from 192.168.1.50, but could be anyone else)
//		char buffer[1024];
//		std::size_t received = 0;
//		sf::IpAddress sender;
//		unsigned short port;
//		socket.receive(buffer, sizeof(buffer), received, sender, port);
//		std::cout << sender.toString() << " said: " << buffer << std::endl;
//
//		std::cin >> buffer2;
//		socket.send(buffer2, MESSAGESIZE, sf::IpAddress::getLocalAddress().toString(), 55002);
//	} while (memcmp(buffer2, "quit", 4) != 0);
//
//
//
//	return 0;
//}
//

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
		recieved = false;
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

	//connecting to server and getting playerId 
	std::string name;
	std::string IP;
	cout << "Enter IP: ";
	std::cin >> IP;
	ip = IP;
	cout << "\nConnected to " << ip.toString() << "\n";
	while (client.connect(ip, 2000) != sf::Socket::Done) {
		cout << "Failed to connect, trying again...\n";
	}
	cout << "connected to server\n";
	cout << "Enter Player Name: ";
	std::cin >> name;
	packetSend << name;
	sendtoServer();
	recievefromServer();
	packetRecieved >> playerID;

	cout << "\nyou are player: " << playerID << endl;
	playerID = playerID - 1;

	client.setBlocking(false);

	//Initialise the background texture and sprite
	sf::Texture floorTexture;
	sf::Sprite floor;
	floorTexture.loadFromFile("media/controlsScreen.png");
	floorTexture.setRepeated(true);
	floor.setTexture(floorTexture);
	floor.setTextureRect(sf::IntRect(0, 0, 960, 540));

	sf::RectangleShape rectangle(sf::Vector2f(128.0f, 128.0f));
	rectangle.setFillColor(sf::Color::Red);
	rectangle.setPosition(320, 240);


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
					/*tanks[0].Reset(); tanks[1].Reset();
					netSimulator.Reset();*/
					//nextPrint = startTime;
					printf("\n\n--------RESET--------\n\n");
				}
				if (event.key.code == sf::Keyboard::Key::W)
				{
					//rectangle.setRotation(10);
					//rectangle.setPostion(getPosition().x);
					rectangle.setPosition(rectangle.getPosition().x, rectangle.getPosition().y-10);
				}
				if (event.key.code == sf::Keyboard::Key::S)
				{
					//rectangle.setRotation(10);
					//rectangle.setPostion(getPosition().x);
					rectangle.setPosition(rectangle.getPosition().x, rectangle.getPosition().y + 10);
				}
				if (event.key.code == sf::Keyboard::Key::A)
				{
					//rectangle.setRotation(10);
					//rectangle.setPostion(getPosition().x);
					rectangle.setPosition(rectangle.getPosition().x - 10, rectangle.getPosition().y);
				}
				if (event.key.code == sf::Keyboard::Key::D)
				{
					//rectangle.setRotation(10);
					//rectangle.setPostion(getPosition().x);
					rectangle.setPosition(rectangle.getPosition().x + 10, rectangle.getPosition().y);
				}
			}
		}
		//Render the scene
		window.clear();
		window.draw(floor);
		window.draw(rectangle);
		/*for (auto& tank : tanks) {
			tank.Render(&window);
		}
		window.draw(debugText);*/
		window.display();
	}

	return 0;
}