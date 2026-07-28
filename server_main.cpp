#include "server.h"
#include <boost/program_options.hpp>
#include <iostream>
#include "utils.h"

int main(int argc, char const *argv[])
{
    try
    {
        int port_var = 0; // Used for direct binding (Method 3)

        boost::program_options::options_description desc("Server Configuration");
        desc.add_options()("help,h", "Produce help message")("port,p", boost::program_options::value<int>()->required(), "Set port number");

        boost::program_options::variables_map vm;
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
        boost::program_options::notify(vm); // Must call notify() to apply default values / bindings!

        if (vm.count("help"))
        {
            std::cout << desc << "\n";
            return 0;
        }
        int port = vm["port"].as<int>();

        Server server;
        auto result = server.server_listen(port);

        
    }
    catch (const boost::program_options::error &e)
    {
        std::cerr << "Option error: " << e.what() << "\n";
        return 1;
    }
    catch (const SocketError &err)
    {
        std::cout << "Socket Error: " << err.what() << std::endl;
    }
    catch (const EpollError &err)
    {
        std::cout << "Epoll Error: " << err.what() << std::endl;
    }

    return 0;
}
