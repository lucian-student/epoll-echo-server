#pragma once
#include <stdexcept>
#include <expected>

class SocketError : public std::runtime_error
{
public:
    SocketError(const std::string &message) : std::runtime_error(message)
    {
    }
};

class EpollError : public std::runtime_error
{
public:
    EpollError(const std::string &message) : std::runtime_error(message)
    {
    }
};

/*
write constexpr function
*/

//static constexpr std::expected<void,> handle_error()