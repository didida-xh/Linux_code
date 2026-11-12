#include "EchoServer.hpp"
#include <memory>

static void Usage(const std::string &process)
{
    std::cerr << "Usage:\n\t";
    std::cerr << process << " local_port" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    ENABLE_CONSOLE_LOG_STRATEGY();
    int16_t server_port = std::stoi(argv[1]);

    std::unique_ptr<UdpServer> usvr = std::make_unique<UdpServer>(server_port);
    usvr->Init();
    usvr->Start();
}