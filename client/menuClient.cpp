#include "menuClient.h"

MenuClient::MenuClient(Queue<Response> &recv_queue, Queue<std::shared_ptr<MessageEvent>> &send_queue, RecvLoop &receiver, SendLoop &sender, Protocol &protocol, QPoint& w_pos_when_game_started) :
    partida_iniciada(false),
    recv_queue(recv_queue),
    send_queue(send_queue),
    receiver(receiver),
    sender(sender),
    protocol(protocol),
    app(createApp()),
    //menuWindow(QtWindow(app, "Counter Strike 2D", SCREEN_WIDTH, SCREEN_HEIGHT)),
    //menuController(menuWindow, protocol),
    players(),
    w_pos_when_game_started(w_pos_when_game_started)
{
}


bool MenuClient::run() {

    qputenv("QT_QPA_PLATFORM", "xcb");

    QtWindow menuWindow(QtWindow(app, "Counter Strike 2D", SCREEN_WIDTH, SCREEN_HEIGHT));
    MenuController menuController(menuWindow, protocol);


    QTimer* timer = new QTimer(nullptr);

    // Iniciar el bucle de eventos del menú
    QObject::connect(timer, &QTimer::timeout, timer, [this, &menuWindow, &menuController]() {
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
                case START: {
                    partida_iniciada = true;
                    w_pos_when_game_started = menuWindow.getPosition();
                    menuController.onGameStarted();
                    break;
                }
                default: {
                    break;
                }
            }
        }
    });
    timer->start(16); // limita el menu a 60fps

    // Conectar señales y slots
    QObject::connect(&menuController, &MenuController::nuevoEvento, [this](std::shared_ptr<MessageEvent> event) {
        send_queue.try_push(event);
    });

    // Ejecutar el bucle de eventos de la aplicación
    menuWindow.show();
    app.exec();

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
