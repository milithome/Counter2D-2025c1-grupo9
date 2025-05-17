

#include <iostream>
#include <exception>

#include "../common/communication/socket.h"
#include "../common/communication/protocol.h"


int main() try {
	Socket server("localhost", "12345");
	Protocol protocol(std::move(server));
	std::cout << "Socket connected" << std::endl;

	std::cout << "Sending create..." << std::endl;
	protocol.send_create("Lobby1");
	std::cout << "Waiting for response..." << std::endl;
	Response r = protocol.recv_response();
	if (r.result == 0) {
		std::cout << "Lobby created successfully" << std::endl;
	} else {
		std::cout << "Failed to create lobby" << std::endl;
	}

	std::cout << "Sending list..." << std::endl;
	protocol.send_list();
	std::cout << "Waiting for response..." << std::endl;
	r = protocol.recv_response();
	std::cout << "Response: " << r.partidas.size() << std::endl;
	for (const auto& partida : r.partidas) {
		std::cout << partida.c_str() << std::endl;
	}

	std::cout << "Sending join..." << std::endl;
	protocol.send_join("Lobby1");
	std::cout << "Waiting for response..." << std::endl;
	r = protocol.recv_response();
	if (r.result == 0) {
		std::cout << "Joined lobby successfully" << std::endl;
	} else {
		std::cout << "Failed to join lobby" << std::endl;
	}
	

	return 0;
} catch (std::exception& e) {
	// If case of error, print it and exit with error
	std::cerr << e.what() << std::endl;
	return 1;
}
