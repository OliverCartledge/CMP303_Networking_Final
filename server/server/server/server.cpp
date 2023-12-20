//#include <SFML/Network.hpp>
//#include <iostream>
//using std::to_string;
//
//int main()
//{
//	// Create a socket and bind it to the port 55002
//	sf::UdpSocket socket;
//	socket.bind(55002);
//
//	//std::cout << "Hi, I am " + sf::IpAddress::getLocalAddress().toString();
//	std::cout << "Waiting on client joining\n";
//	while (true)
//	{
//
//		//Test push
//		// //Test again again
//		// Receive a message from anyone
//		char buffer[1024];
//		std::size_t received = 0;
//		sf::IpAddress sender;
//		unsigned short port;
//		socket.receive(buffer, sizeof(buffer), received, sender, port);
//		std::cout << sender.toString() << " said: " << buffer << std::endl;
//
//		// Send an answer
//		std::string message = "Welcome " + sender.toString();
//		socket.send(message.c_str(), message.size() + 1, sender, port);
//	}
//
//	return 0;
//}

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main()
{
	cout << "Server is running\n";

	sf::TcpListener listener;
	sf::SocketSelector selector;
	std::vector<sf::TcpSocket*> clients;
	std::vector<std::string> ID;

	bool test = false;

	sf::IpAddress ip = sf::IpAddress::getLocalAddress();
	cout << "The servers IP address is " << ip << endl;
	listener.listen(2000);
	selector.add(listener);

	while (!test)
	{
		if (selector.wait())
		{
			if (selector.isReady(listener))
			{
				sf::TcpSocket* socket = new sf::TcpSocket;
				std::string id;
				listener.accept(*socket);
				sf::Packet packet;
				if (socket->receive(packet) == sf::Socket::Done)
					packet >> id;
				cout << id << " has connected to the server" << endl;
				clients.push_back(socket);
				ID.push_back(id);
				selector.add(*socket);
				packet.clear();
				int playerId;
				playerId = clients.size();
				packet << playerId;
				socket->send(packet);
				if (clients.size() > 4)
				{
					cout << "Too many people";
					listener.close();
				}
			}
		}
		else
			for (int i = 0; i < clients.size(); i++)
			{
				if(selector.isReady(*clients[i]))
				{
					sf::Packet packet, sendPacket;

					if (clients[i]->receive(packet) == sf::Socket::Done)
					{
						for (int j = 0; j < clients.size(); j++)
						{
							if (i != j)
							{
								clients[j]->send(packet);
							}
						}
					}
					if (clients[i]->receive(packet) == sf::Socket::Disconnected)
					{
						cout << ID[i] << " disconnected from the server\n";
						selector.remove(*clients[i]);
						clients.erase(find(clients.begin(), clients.end(), clients[i]));
						ID.erase(find(ID.begin(), ID.end(), ID[i]));
					}
				}
			}
	}

	for (std::vector<sf::TcpSocket*>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		delete* it;
	}

	return 0;
}