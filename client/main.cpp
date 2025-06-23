#include "client/launcher.h"

int main(int argc, char **argv) try {
	(void)argc;
	(void)argv;

	Launcher launcher;
	launcher.run();
	

	return 0;

} catch (std::exception& e) {
	std::cerr << e.what() << std::endl;
	return 1;
}
