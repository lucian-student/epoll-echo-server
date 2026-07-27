#include "server.h"

// Core Socket Headers
#include <sys/socket.h> // socket(), bind(), listen(), accept(), setsockopt()
#include <netinet/in.h> // struct sockaddr_in, htons(), INADDR_ANY
#include <arpa/inet.h>  // inet_pton(), inet_ntop() (converting string IPs to binary)

// Non-blocking & System Operations
#include <fcntl.h>     // fcntl(), O_NONBLOCK
#include <unistd.h>    // read(), write(), close()
#include <sys/epoll.h> // epoll_create1(), epoll_ctl(), epoll_wait()

// Standard C++ Helpers (Optional, but handy)
#include <iostream> // std::cout, std::cerr
#include <cstring>  // memset(), strerror()
#include <cerrno>   // errno, EAGAIN, EWOULDBLOCK
#include "utils.h"

Server::Server()
{
    _socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (_socket_fd == -1)
    {
        int err = errno;
        throw SocketError(strerror(err));
    }
    _epoll_fd = epoll_create1(FD_CLOEXEC);
    if (_socket_fd == -1)
    {
        int err = errno;
        throw EpollError(strerror(err));
    }
}

Server::~Server()
{
    close(_socket_fd);
    _socket_fd = -1;
    close(_epoll_fd);
    _epoll_fd = -1;
}

std::expected<void, ServerError> Server::server_listen(const int port)
{
    /*
    struct __attribute_struct_may_alias__ sockaddr
  {
    sa_family_t sa_family; //short int
    char sa_data[14];
  };
    */

    struct sockaddr_in server_addr{};

    server_addr.sin_family = AF_INET;         // IPv4
    server_addr.sin_port = htons(port);       // Port 8080 (htons converts to big-endian)
    server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on all network interfaces (0.0.0.0)

    int bind_code = bind(_socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    if (bind_code == -1)
    {
        return std::unexpected(ServerError::BIND_ERROR);
    }

    if (-1 == listen(_socket_fd, SOMAXCONN))
    {
        return std::unexpected(ServerError::LISTEN_ERROR);
    }

    return {};
}