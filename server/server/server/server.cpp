
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main()
{
	cout << "Server Running" << endl;

	sf::TcpListener listener;
	sf::SocketSelector selector;
	bool wait = false;
	std::vector<sf::TcpSocket*> clients;
	std::vector<std::string> ID;

	sf::IpAddress ip = sf::IpAddress::getLocalAddress();
	cout << "Server IP is: " << ip<<"\n";
	listener.listen(2000);
	selector.add(listener);
	
	while (!wait)
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
				cout<< id << " has connected to the server" << endl;
				clients.push_back(socket);
				ID.push_back(id);
				selector.add(*socket);
				packet.clear();
				int playerId;
				playerId = clients.size();
				packet << playerId;
				socket->send(packet);
				if (clients.size() == 4)
				{
					//cout << "Maximum server limit reached" << endl;
					
					//listener.close();
					
				}
			
			}
			
			else
			{
				for (int i = 0; i < clients.size(); ++i) {
					if (selector.isReady(*clients[i]))
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
							clients.erase(find(clients.begin(),clients.end(),clients[i]));
							ID.erase(find(ID.begin(), ID.end(), ID[i]));
						}
					}
				}
			}
		}
	}


	for (std::vector<sf::TcpSocket*>::iterator it = clients.begin(); it != clients.end(); it++)
		delete* it;

	//system("pause");

	return 0;


}