target_sources(taller_server
    PRIVATE
    # .cpp files
    acceptor.cpp
    admin.cpp
    clientHandler.cpp
    lobbyClientHandler.cpp
    lobby.cpp
    gameReceiver.cpp
    gameLoop.cpp
    server.cpp
    main.cpp
    PUBLIC
    # .h files
    thread.h
    queue.h
    admin.h
    acceptor.h
    clientHandler.h
    lobbyClientHandler.h
    lobby.h
    gameReceiver.h
    gameLoop.h
    server.h
    )
