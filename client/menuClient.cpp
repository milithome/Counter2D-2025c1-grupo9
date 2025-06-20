#include "menuClient.h"

#include <QScreen>
#include <QRect>

MenuClient::MenuClient(QApplication& app, MenuController& menuController, Queue<Response> &recv_queue, Queue<std::shared_ptr<MessageEvent>> &send_queue, RecvLoop &receiver, SendLoop &sender, Protocol &protocol) :
    recv_queue(recv_queue),
    send_queue(send_queue),
    receiver(receiver),
    sender(sender),
    protocol(protocol),
    app(app),
    menuController(menuController)
{
}


bool MenuClient::run(bool looped) {
    QTimer* timer = new QTimer(nullptr);

    // Iniciar el bucle de eventos del menú
    QObject::connect(timer, &QTimer::timeout, timer, [this]() {
        Response msg;
        while (recv_queue.try_pop(msg)) {
            switch (msg.type) {
                case LIST: {
                    LobbyList data = std::get<LobbyList>(msg.data);
                    menuController.onPartyListReceived(data.lobbies, msg.message, msg.result);
                    break;
                }
                case JOIN: {
                    menuController.onJoinPartyResponseReceived(msg.message, msg.result);
                    break;
                }
                case CREATE: {
                    menuController.onCreatePartyResponseReceived(msg.message, msg.result);
                    break;
                }
                case LEAVE: {
                    menuController.onLeavePartyResponseReceived(msg.message, msg.result);
                    break;
                }
                case STATE_LOBBY: {
                    StateLobby data = std::get<StateLobby>(msg.data);
                    players = data.players;
                    menuController.onLobbyPlayersReceived(players, msg.message, msg.result);
                    break;
                }
                case LOBBY_READY: {
                    menuController.onLobbyReady();
                    break;
                }
                case NOT_LOBBY_READY: {
                    menuController.onLobbyNotReady();
                    break;
                }
                case START: {
                    partida_iniciada = true;
                    menuController.onGameStarted();
                    break;
                }
                case INITIAL_DATA: {
					InitialData data = std::get<InitialData>(msg.data);
                    emit initialDataReceived(data);
				}
                default: {
                    break;
                }
            }
        }
    });
    timer->start(16); // limita el menu a 60fps

    // Conectar señales y slots
    QObject::connect(&menuController, &MenuController::newMessage, [this](std::shared_ptr<MessageEvent> message) {
        send_queue.try_push(message);
    });
    if (looped) {
        menuController.showStartingScreen();
    }
    
    app.exec();
    delete timer;

    return !partida_iniciada;
}

std::vector<std::string> MenuClient::allPlayers()
{
    if (partida_iniciada) {
        return players;
    } else {
        std::cerr << "No game started yet, cannot retrieve players." << std::endl;
        return {};
    }
}


