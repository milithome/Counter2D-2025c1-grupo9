target_sources(taller_common
    PRIVATE
    # .cpp files
    communication/liberror.cpp
    communication/protocol.cpp
    communication/resolver.cpp
    communication/resolvererror.cpp
    communication/socket.cpp
    PUBLIC
    # .h files
    communication/liberror.h
    communication/protocol.h
    communication/resolver.h
    communication/resolvererror.h
    communication/socket.h
    structures.h
    )
