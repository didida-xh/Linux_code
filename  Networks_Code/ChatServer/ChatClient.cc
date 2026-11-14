#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "Thread.hpp"
#include "InetAddr.hpp"
using namespace NS_THREAD_MODULE;

static void Usage(const std::string &proc)
{
    std::cout << "Usage:\n\t";
    std::cout << proc << " server_ip server_port" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }
    std::string server_ip = argv[1];
    uint16_t server_port = std::stoi(argv[2]);

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "socket error" << std::endl;
        exit(2);
    }
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);
    server.sin_addr.s_addr = inet_addr(server_ip.c_str());

    while (true)
    {
        std::string message;
        std::cout << "Please Enter#";
        std::getline(std::cin, message);

        ssize_t n = sendto(sockfd, message.c_str(), message.size(), 0, (struct sockaddr *)&server, sizeof(server));
        if (n > 0)
        {
            char inbuffer[1024] = {0};
            struct sockaddr_in temp;
            socklen_t len = sizeof(temp);
            ssize_t m = recvfrom(sockfd, inbuffer, sizeof(inbuffer) - 1, 0, (struct sockaddr *)&temp, &len);
            if (m > 0)
            {
                inbuffer[m] = 0;
                std::cout << inbuffer << std::endl;
            }
        }
    }

    return 0;
}