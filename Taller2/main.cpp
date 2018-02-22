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
					// Add the new client to the clients list
					std::cout << "Llega el cliente con puerto: " << client->getRemotePort() << std::endl;
					clients.push_back(client);
					// Add the new client to the selector so that we will
					// be notified when he sends something
					selector.add(*client);
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
				for (std::list<sf::TcpSocket*>::iterator it = clients.begin(); it != clients.end(); ++it) {
					sf::TcpSocket& client = **it;
					if (selector.isReady(client)) {
						char receiveBuffer[2000];
						std::size_t _received;
						// The client has sent some data, we can receive it
						sf::Packet packet;
						status = client.receive(receiveBuffer, sizeof(receiveBuffer), _received);
						if (status == sf::Socket::Done)	{
							std::string strRec;
							strRec = receiveBuffer;
							std::cout << "He recibido " << strRec << " del puerto " << client.getRemotePort() << std::endl;
							//Reenviar mensaje a todos los clientes:
							for (std::list<sf::TcpSocket*>::iterator it2 = clients.begin(); it2 != clients.end(); ++it2) {
								sf::TcpSocket& tempSok = **it2;
								sf::Socket::Status tempSt = tempSok.send(strRec.c_str(), strRec.length() + 1);
							}
						}
						else if (status == sf::Socket::Disconnected) {
							selector.remove(client);
							std::cout << "Elimino el socket que se ha desconectado\n";
						}
						else {
							std::cout << "Error al recibir de " << client.getRemotePort() << std::endl;
						}
					}
				}
			}
		}
	}
}

void main()
{
	std::cout << "Seras servidor (s) o cliente (c)? ... ";
	char c;
	std::cin >> c;

	if (c == 's')
	{
		ControlServidor();
	}
	/*else if (c == 'c')
	{
		sf::TcpSocket socket;
		sf::Socket::Status status = socket.connect("localhost", 50000, sf::milliseconds(15.f));
		if (status != sf::Socket::Done)
		{
			std::cout << "Error al establecer conexion\n";
			exit(0);
		}
		else
		{
			std::cout << "Se ha establecido conexion\n";
		}
		std::string str = "hola";
		do
		{
			std::cout << "Escribe ... ";
			std::getline(std::cin, str);
			sf::Packet packet;
			packet << str;
			status = socket.send(packet);
			if (status != sf::Socket::Done)
			{
				std::cout << "Error al enviar\n";
			}
			std::cout << std::endl;
		} while (str != "exit");
		socket.disconnect();

	}*/
	else
	{
		exit(0);
	}


}