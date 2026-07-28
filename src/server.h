#pragma once
#include <expected>
#include <cstdint>

enum class ServerError : std::uint8_t
{
    BIND_ERROR,
    LISTEN_ERROR,
    EPOLL_ADD_SERVER,
    ACCEPT_ERROR,
    EPOLL_ADD_CLIENT,
    EPOLL_WAIT
};

class Server
{
public:
    Server();

    ~Server();

    std::expected<void, ServerError> server_listen(const int port);

private:
    static constexpr int MAXEVENTS = 1024;
    static constexpr int BUFFER_SIZE = 1024;

    int _socket_fd;
    int _epoll_fd;
};