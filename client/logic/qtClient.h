#ifndef QTCLIENT_H
#define QTCLIENT_H

#include "client/views/qtwindow.h"
#include "client/controllers/game_controller.h"
#include "client/controllers/menu_controller.h"
#include "client/controllers/message_event.h"
#include "client/controllers/action_event.h"
#include "common/communication/protocol.h"

#include <QApplication>
#include <QWidget>
#include <QTimer>

#include <variant>
#include <atomic>

class QTClient {
public:
    QTClient(const std::string gameName, int screenWitdh, int screenHeight, Queue<Response>& rQueue, Queue<std::shared_ptr<MessageEvent>>& sQueue, Protocol& protocol);  // Constructor
    ~QTClient(); // Destructor

    void connectToServer(const std::string& address, int port);
    void sendMessage(const std::string& message);
    std::string receiveMessage();

private:
    Queue<Response>& recv_queue;
    Queue<std::shared_ptr<MessageEvent>>& send_queue;
    bool started;
    QApplication app;
    QtWindow menuWindow;
    QTimer* timer;
    MenuController menuController;
    QPoint w_pos_when_started;
};

#endif // QTCLIENT_H