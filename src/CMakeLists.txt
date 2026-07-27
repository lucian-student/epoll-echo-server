add_library(
    epoll_server
    server.cpp
)


target_include_directories(epoll_server PUBLIC "${CMAKE_CURRENT_LIST_DIR}")