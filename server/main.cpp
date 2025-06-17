#include "server.h"
#include <thread>
#include <sys/syscall.h>


int main() {
    try {
        auto a = load_server_config("../config_server.yaml");
        Server server{a};
        server.start();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred." << std::endl;
        return 1;
    }
    
    return 0;
}

