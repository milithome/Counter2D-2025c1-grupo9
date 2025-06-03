target_sources(taller_server
    PRIVATE
    # .cpp files
    acceptor.cpp
    admin.cpp
    clientReceiver.cpp
    clientSender.cpp
    match.cpp
    server.cpp
    menu.cpp
    main.cpp
    PUBLIC
    # .h files
    admin.h
    acceptor.h
    clientReceiver.h
    clientSender.h
    match.h
    server.h
    menu.h
    serverStructures.h
    )
