#pragma once

class Server
{
public:
    Server();

    ~Server();

    void listen(const int port);

private:
    int _socket_fd;
};