#include <SFML\Network.hpp>
#include <iostream>
#include <list>

void ControlServidor()
{
	bool running = true;
	// Create a socket to listen to new connections
	sf::TcpListener listener;
	sf::Socket::Status status = listener.listen(50000);
	if (status != sf::Socket::Done)	{
		std::cout << "Error al abrir listener\n";
		exit(0);
	}

	// Create a list to store the future clients
	std::list<sf::TcpSocket*> clients;
	// Create a selector
	sf::SocketSelector selector;
	// Add the listener to the selector
	selector.add(listener);

	// Endless loop that waits for new connections
	while (running)	{
		// Make the selector wait for data on any socket
		if (selector.wait()) {
			// Test the listener
			if (selector.isReady(listener)) {
				// The listener is ready: there is a pending connection
				sf::TcpSocket* client = new sf::TcpSocket;
				if (listener.accept(*client) == sf::Socket::Done)
				{
					//avisamos a todos que se ha conectado un nuevo cliente
					for (std::list<sf::TcpSocket*>::iterator it = clients.begin(); it != clients.end(); ++it) {
						sf::TcpSocket& tempSok = **it;
						std::string newClient = "Se ha conectado un nuevo cliente";
						sf::Packet packet;
						packet << newClient;
						tempSok.send(packet);
					}

					// Add the new client to the clients list
					std::cout << "Llega el cliente con puerto: " << client->getRemotePort() << std::endl;
					clients.push_back(client);
					// Add the new client to the selector so that we will
					// be notified when he sends something
					selector.add(*client);
					
					std::string newClient = "Te has conectado al chat"; sf::Packet packet;
					packet << newClient;
					client->send(packet);

				}
				else
				{
					// Error, we won't get a new connection, delete the socket
					std::cout << "Error al recoger conexión nueva\n";
					delete client;
				}
			}
			else {	//Receive:
				// The listener socket is not ready, test all other sockets (the clients)
				//lista de iteradores que iteran listas de tcp sockets
				std::list<std::list<sf::TcpSocket*>::iterator> itTemp; //si se desconecta un socket, lo guardamos en este it para borrarlo después
				for (std::list<sf::TcpSocket*>::iterator it = clients.begin(); it != clients.end(); ++it) {
					sf::TcpSocket& client = **it;
					if (selector.isReady(client)) {
						std::string strRec;
						// The client has sent some data, we can receive it
						sf::Packet packet;
						status = client.receive(packet);
						packet >> strRec;
						if (status == sf::Socket::Done)	{
							std::cout << "He recibido " << strRec << " del puerto " << client.getRemotePort() << std::endl;
							//Reenviar mensaje a todos los clientes:
							for (std::list<sf::TcpSocket*>::iterator it2 = clients.begin(); it2 != clients.end(); ++it2) {
								sf::TcpSocket& tempSok = **it2;
								sf::Packet packet;
								packet << "Testerino: " << strRec;
								tempSok.send(packet);
							}
						}
						else if (status == sf::Socket::Disconnected) {
							std::cout << clients.size() << std::endl;
							selector.remove(client);
							itTemp.push_back(it); //guardamos iterador del socket desconectado
							std::cout << "Elimino el socket que se ha desconectado\n";
						}
						else {
							std::cout << "Error al recibir de " << client.getRemotePort() << std::endl;
						}
					}
				}
				//iterador que itera la lista de iteradores que iteran la lista de sockets
				for (std::list<std::list<sf::TcpSocket*>::iterator>::iterator it = itTemp.begin(); it != itTemp.end(); ++it) {
					clients.erase(*it); 
				}
				//std::list<std::list<sf::TcpSocket*>::iterator>::swap(itTemp);
			}
		}
	}
}

void main()
{
	std::cout << "Seras servidor (s)? ... ";
	char c;
	std::cin >> c;

	if (c == 's') {
		ControlServidor();
	}
	else {
		exit(0);
	}


}