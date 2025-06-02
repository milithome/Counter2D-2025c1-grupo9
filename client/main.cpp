#include "client/logic/client.h"
#include <iostream>
#include <exception>

#define NAME_SERVER "localhost"
#define PORT "12345"

int main(int argc, char **argv) try {
	if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <client_name>" << std::endl;
        return 1;
    }
	Client client(argv[1], NAME_SERVER, PORT);
	client.run();
	return 0;

} catch (std::exception& e) {
	std::cerr << e.what() << std::endl;
	return 1;
}
