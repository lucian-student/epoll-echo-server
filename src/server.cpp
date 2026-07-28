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
    _epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (_epoll_fd == -1)
    {
        int err = errno;
        throw EpollError(strerror(err));
    }
    // std::cout << "constructed: " << _epoll_fd << ", " << _socket_fd << std::endl;
}

Server::~Server()
{
    close(_socket_fd);
    _socket_fd = -1;
    close(_epoll_fd);
    _epoll_fd = -1;
}

std::expected<void, ServerError> Server::server_listen(const SignalWatcher &watcher, const int port)
{
    struct sockaddr_in server_addr{};

    server_addr.sin_family = AF_INET;         // IPv4
    server_addr.sin_port = htons(port);       // Port 8080 (htons converts to big-endian)
    server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on all network interfaces (0.0.0.0)

    int bind_code = bind(_socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    if (bind_code == -1)
        return std::unexpected(ServerError::BIND_ERROR);

    if (-1 == listen(_socket_fd, SOMAXCONN))
        return std::unexpected(ServerError::LISTEN_ERROR);

    epoll_event server_event{};
    server_event.events = EPOLLIN;
    server_event.data.fd = _socket_fd;

    if (-1 == epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _socket_fd, &server_event))
    {
        // int err = errno;
        //  std::cout << strerror(err) << ", " << _epoll_fd << ", " << _socket_fd << std::endl;
        return std::unexpected(ServerError::EPOLL_ADD_SERVER);
    }

    epoll_event watcher_event{};
    watcher_event.events = EPOLLIN;
    watcher_event.data.fd = watcher.fd();

    if (-1 == epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, watcher.fd(), &watcher_event))
        return std::unexpected(ServerError::EPOLL_ADD_SIGNAL);

    epoll_event events[Server::MAXEVENTS];

    sockaddr_in client_addr;
    socklen_t client_len;

    epoll_event client_event{};
    client_event.events = EPOLLIN;

    std::byte buffer[Server::BUFFER_SIZE];

    std::cout << "Server is listening!" << std::endl;

    bool running = true;
    while (running)
    {
        client_len = sizeof(client_addr);
        int client_socket = accept4(_socket_fd, reinterpret_cast<sockaddr *>(&client_addr), &client_len, SOCK_NONBLOCK | SOCK_CLOEXEC);

        if (client_socket == -1)
        {
            int err = errno;
            if (err != EWOULDBLOCK)
                return std::unexpected(ServerError::ACCEPT_ERROR);
        }
        else
        {
            client_event.data.fd = client_socket;
            if (-1 == epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_socket, &client_event))
                return std::unexpected(ServerError::EPOLL_ADD_CLIENT);
        }

        int event_count = epoll_wait(_epoll_fd, events, Server::MAXEVENTS, -1);
        if (event_count == -1)
            return std::unexpected(ServerError::EPOLL_WAIT);

        for (int i = 0; i < event_count; i++)
        {
            auto &event = events[i];
            uint32_t flags = event.events;
            int fd = event.data.fd;

            if (fd == watcher.fd())
                running = false;

            if (flags & EPOLLIN)
            {
                // here i can read the socket?
                int recv_status = recv(fd, &buffer, Server::BUFFER_SIZE, 0);
            }
            else
            {
                continue;
            }
        }
    }

    return {};
}