#include "server.h"
#include <thread>
#include <sys/syscall.h>

int main() {
    try {
        std::cout << "Main thread ID: " << std::this_thread::get_id() << std::endl;

        Server server;
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

