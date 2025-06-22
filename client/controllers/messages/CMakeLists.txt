target_sources(taller_client
    PRIVATE
    # .cpp files
    create_event.cpp
    leave_event.cpp
    join_event.cpp
    list_event.cpp
    start_event.cpp
    message_event.cpp

    PUBLIC
    # .h files
    create_event.h
    leave_event.h
    join_event.h
    list_event.h
    start_event.h
    message_event.h
    )