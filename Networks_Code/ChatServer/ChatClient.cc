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

int sockfd = 0;
std::string server_ip;
uint16_t server_port = 0;
std::string nickname;

static void Usage(const std::string &proc)
{
    std::cout << "Usage:\n\t";
    std::cout << proc << " server_ip server_port" << std::endl;
}

static void Online(InetAddr &serveraddr)
{
    std::cout << "Please Set your Nick Name# ";
    std::getline(std::cin, nickname);
    std::string online_message = nickname + "online!";
    ssize_t n = sendto(sockfd, online_message.c_str(), online_message.size(), 0,
                       (struct sockaddr *)serveraddr.GetNetAddress(), serveraddr.Len());
    (void)n;
}

void RecvMessage()
{
    while (true)
    {
        char inbuffer[1024] = {0};
        struct sockaddr_in temp;
        socklen_t len = sizeof(temp);
        ssize_t m = recvfrom(sockfd, inbuffer, sizeof(inbuffer) - 1, 0, (struct sockaddr *)&temp, &len);
        if (m > 0)
        {
            inbuffer[m] = 0;
            std::cerr << inbuffer << std::endl;
        }
    }
}

void SendMessage()
{
    InetAddr serveraddr(server_port, server_ip);
    Online(serveraddr);
    while (true)
    {
        std::string message;
        std::cout << "please enter# ";
        std::getline(std::cin, message);

        message = nickname + "#" + message;

        ssize_t n = sendto(sockfd, message.c_str(), message.size(), 0,
                           (struct sockaddr *)serveraddr.GetNetAddress(), serveraddr.Len());
        (void)n;
    }
}
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }
    server_ip = argv[1];
    server_port = std::stoi(argv[2]);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "socket error" << std::endl;
        exit(2);
    }

    Thread recver(RecvMessage);
    Thread sender(SendMessage);

    recver.Start();
    sender.Start();

    recver.Join();
    sender.Join();
    return 0;
}