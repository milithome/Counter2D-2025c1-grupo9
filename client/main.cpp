#include "client/views/game_view.h"
#include "client/views/main_view.h"
#include "client/views/qtwindow.h"
#include "client/controllers/game_controller.h"
#include "client/controllers/menu_controller.h"
#include "client/controllers/message_event.h"
#include "client/controllers/action_event.h"
#include "common/game.h"
#include "common/player.h"
#include "common/communication/protocol.h"
#include "client/receiver&sender/clientReceiverLoop.h"
#include "client/receiver&sender/clientSenderLoop.h"
#include "client/client.h"


#include <iostream>
#include <exception>

#include <QApplication>
#include <QWidget>
#include <QTimer>

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define NAME_SERVER "localhost"
#define PORT "12345"

using namespace SDL2pp;

#include <variant>

int main(int argc, char **argv) try {
	if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <client_name>" << std::endl;
        return 1;
    }
	Client client = Client(argv[1], NAME_SERVER, PORT);
	client.run();

	return 0;

} catch (std::exception& e) {
	std::cerr << e.what() << std::endl;
	return 1;
}
