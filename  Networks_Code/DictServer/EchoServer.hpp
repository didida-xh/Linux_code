#ifndef __ECHOSERVER_HPP
#define __ECHOSERVER_HPP

#include <iostream>
#include <string>
#include <cstdlib>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Logger.hpp"
#include <functional>

using namespace NS_LOG_MODULE;

const static int default_fd = -1;
const static int default_port = 8888;

using callback_t = std::function<std::string(std::string)>;

enum
{
    SUCCESS = 0,
    USAGE_ERR,
    SOCKET_ERR,
    BIND_ERR,
};

class UdpServer
{

public:
    UdpServer(callback_t cb, uint16_t port = default_port)
        : _port(port),
          _sockfd(default_fd),
          _cb(cb)
    {
    }
    ~UdpServer()
    {
        close(_sockfd);
    }

    void Init()
    {
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (_sockfd < 0)
        {
            LOG(LogLevel::FATAL) << "create socket error";
            exit(SOCKET_ERR);
        }
        LOG(LogLevel::INFO) << "create socket success, sockfd: " << _sockfd;

        struct sockaddr_in local;
        bzero(&local, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);
        local.sin_addr.s_addr = INADDR_ANY;

        int n = bind(_sockfd, (struct sockaddr *)&local, sizeof(local));
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "bind socket error";
            exit(BIND_ERR);
        }
        LOG(LogLevel::INFO) << "bind socket success" << ", port: " << _port;
    }

    void Start()
    {
        char inbuffer[1024];
        while (true)
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);

            ssize_t n = recvfrom(_sockfd, inbuffer, sizeof(inbuffer) - 1, 0, (struct sockaddr *)&peer, &len);
            if (n > 0)
            {
                uint16_t client_port = ntohs(peer.sin_port);
                std::string client_ip = inet_ntoa(peer.sin_addr);
                std::string client_address = "[" + client_ip + ":" + std::to_string(client_port) + "]#";

                inbuffer[n] = 0;
                // LOG(LogLevel::DEBUG) << client_address << inbuffer;
                // std::string echo_string = "server echo#";
                // echo_string += inbuffer;
                std::string result = _cb(inbuffer);
                sendto(_sockfd, result.c_str(), result.size(), 0, (struct sockaddr *)&peer, len);
            }
            else
            {
                LOG(LogLevel::ERROR) << "recvfrom error";
            }
        }
    }

private:
    int _sockfd;
    uint16_t _port;
    callback_t _cb;
};

#endif
