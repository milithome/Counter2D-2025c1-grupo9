target_sources(taller_server
    PRIVATE
    # .cpp files
    acceptor.cpp
    server.cpp
    main.cpp
    PUBLIC
    # .h files
    acceptor.h
    server.h
    thread.h
    )
