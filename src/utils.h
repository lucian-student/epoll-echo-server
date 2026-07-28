#pragma once
#include <stdexcept>
#include <expected>
#include <sys/signalfd.h>
#include <signal.h>
#include <cstring>

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

class SignalError : public std::runtime_error
{
public:
    SignalError(const std::string &message) : std::runtime_error(message)
    {
    }
};

static int close_handler()
{
    sigset_t sig_set;
    sigemptyset(&sig_set);
    sigaddset(&sig_set, SIGTERM);
    sigaddset(&sig_set, SIGINT);

    if (sigprocmask(SIG_BLOCK, &sig_set, nullptr) < 0)
    {
        return -1;
    }

    return signalfd(-1, &sig_set, SFD_NONBLOCK | SFD_CLOEXEC);
}

class SignalWatcher
{
public:
    SignalWatcher()
    {

        _fd = close_handler();
        if (-1 == _fd)
        {
            int err = errno;
            throw SignalError(strerror(err));
        }
    }

    ~SignalWatcher() noexcept
    {
        close(_fd);
    }

    int fd() const noexcept
    {
        return _fd;
    }

private:
    int _fd;
};

/*
write constexpr function
*/

// static constexpr std::expected<void,> handle_error()