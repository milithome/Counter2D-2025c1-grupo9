#include "qtClient.h"

QTClient::QTClient(const std::string gameName, int screenWitdh, int screenHeight, Queue<Response>& rQueue, Queue<std::shared_ptr<MessageEvent>>& sQueue, Protocol& protocol):
    recv_queue(rQueue),
    send_queue(sQueue),
    started(false),
    app(0, nullptr), 
    menuWindow(app, gameName, screenWitdh, screenHeight), 
    menuController(&menuWindow,protocol),    
    timer(&menuController) 
{
}