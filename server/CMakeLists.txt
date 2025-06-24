target_sources(taller_server
    PRIVATE
    # .cpp files
    acceptor.cpp
    admin.cpp
    server.cpp
    main.cpp
    PUBLIC
    # .h files
    admin.h
    acceptor.h
    server.h
    serverStructures.h
    )
