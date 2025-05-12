target_sources(taller_server
    PRIVATE
    # .cpp files
    acceptor.cpp
    server.cpp
    clientHandler.cpp
    lobby.cpp
    game_loop.cpp
    lobbyClientHandler.cpp
    admin.cpp
    main.cpp
    PUBLIC
    # .h files
    acceptor.h
    server.h
    thread.h
    queue.h
    clientHandler.h
    lobby.h
    game_loop.h
    lobbyClientHandler.h
    admin.h
    )
