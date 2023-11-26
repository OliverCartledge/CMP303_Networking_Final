#include <SFML/Network/UdpSocket.hpp>
#include <iostream>
using std::to_string;

// The (fixed) size of message that we send between the two programs
#define MESSAGESIZE 40

int main()
{
	// Create a socket and bind it to the port 55001
	sf::UdpSocket socket;
	socket.bind(55001);

	int test = 0;

	char buffer2[MESSAGESIZE];

	do
	{
		// Send a message to 192.168.1.50 on port 55002
		std::string message = "Hi, I am " + sf::IpAddress::getLocalAddress().toString();
		socket.send(message.c_str(), message.size() + 1, sf::IpAddress::getLocalAddress().toString(), 55002);

		// Receive an answer (most likely from 192.168.1.50, but could be anyone else)
		char buffer[1024];
		std::size_t received = 0;
		sf::IpAddress sender;
		unsigned short port;
		socket.receive(buffer, sizeof(buffer), received, sender, port);
		std::cout << sender.toString() << " said: " << buffer << std::endl;

		std::cin >> buffer2;
		socket.send(buffer2, MESSAGESIZE, sf::IpAddress::getLocalAddress().toString(), 55002);
	} while (memcmp(buffer2, "quit", 4) != 0);



	return 0;
}