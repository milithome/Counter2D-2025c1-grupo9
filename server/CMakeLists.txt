target_sources(taller_server
    PRIVATE
    # .cpp files
    acceptor.cpp
    admin.cpp
    clientHandler.cpp
    lobby.cpp
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
    lobby.h
    gameLoop.h
    server.h
    )
