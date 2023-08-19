#creating a library
add_library(server
    http_server.hpp
    session.hpp
    session_base.hpp
    listener.hpp
    http_server.cpp
    session_base.cpp
)