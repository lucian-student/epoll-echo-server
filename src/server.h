#pragma once
#include <expected>
#include <cstdint>
#include <string_view>
#include "utils.h"

enum class ServerError : std::uint8_t
{
    BIND_ERROR,
    LISTEN_ERROR,
    EPOLL_ADD_SERVER,
    ACCEPT_ERROR,
    EPOLL_ADD_CLIENT,
    EPOLL_WAIT,
    EPOLL_ADD_SIGNAL
};

static constexpr std::string_view server_strreror(ServerError error)
{
    switch (error)
    {
    case ServerError::BIND_ERROR:
        return "BIND_ERROR";
    case ServerError::LISTEN_ERROR:
        return "LISTEN_ERROR";
    case ServerError::EPOLL_ADD_SERVER:
        return "EPOLL_ADD_SERVER";
    case ServerError::ACCEPT_ERROR:
        return "ACCEPT_ERROR";
    case ServerError::EPOLL_ADD_CLIENT:
        return "EPOLL_ADD_CLIENT";
    case ServerError::EPOLL_WAIT:
        return "EPOLL_WAIT";
    case ServerError::EPOLL_ADD_SIGNAL:
        return "EPOLL_ADD_SIGNAL";
    }
    return "UNKNOWN";
}

class Server
{
public:
    Server();

    ~Server();

    std::expected<void, ServerError> server_listen(const SignalWatcher &watcher, const int port);

private:
    static constexpr int MAXEVENTS = 1024;
    static constexpr int BUFFER_SIZE = 1024;

    int _socket_fd;
    int _epoll_fd;
    int _close_handler;
};