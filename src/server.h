#pragma once
#include <expected>
#include <cstdint>

enum class ServerError : std::uint8_t
{
    BIND_ERROR,
    LISTEN_ERROR
};

class Server
{
public:
    Server();

    ~Server();

    std::expected<void, ServerError> server_listen(const int port);

private:
    int _socket_fd;
    int _epoll_fd;
};