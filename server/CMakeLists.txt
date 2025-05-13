target_sources(taller_server
    PRIVATE
    # .cpp files
    acceptor.cpp
    clientHandler.cpp
    lobbyClientHandler.cpp
    lobby.cpp
    gameReceiver.cpp
    gameLoop.cpp
    admin.cpp
    server.cpp
    main.cpp
    PUBLIC
    # .h files
    thread.h
    queue.h
    acceptor.h
    clientHandler.h
    lobbyClientHandler.h
    lobby.h
    gameReceiver.h
    gameLoop.h
    admin.h
    server.h
    )
