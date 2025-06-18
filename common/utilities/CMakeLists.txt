target_sources(taller_common
    PRIVATE
    # .cpp files
    map.cpp
    serializer_utils.cpp
    config.cpp
    
    PUBLIC
    # .h files
    config.h
    map.h
    serializer_utils.h
    queue.h
    thread.h
    )