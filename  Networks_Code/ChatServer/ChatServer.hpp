#ifndef __ECHOSERVER_HPP
#define __ECHOSERVER_HPP

#include <iostream>
#include <string>
#include <cstdlib>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <functional>
#include <arpa/inet.h>
#include "InetAddr.hpp"
#include "Logger.hpp"

using namespace NS_LOG_MODULE;

const static int default_fd = -1;
const static int default_port = 8888;

using handler_addr_t = std::function<void(const InetAddr &)>;
using handler_msg_t = std::function<void(int sokcfd, std::string msg)>;

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
    UdpServer(uint16_t port = default_port)
        : _port(port),
          _sockfd(default_fd),
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

        InetAddr local(_port);

        int n = bind(_sockfd, (struct sockaddr *)(local.GetNetAddress()), local.len());
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "bind socket error";
            exit(BIND_ERR);
        }
        LOG(LogLevel::INFO) << "bind socket success" << ", port: " << _port;
    }

    void RegisterService(handler_addr_t handler_addr, handler_msg_t handler_msg)
    {
        _handler_addr = handler_addr;
        _handler_msg = handler_msg;
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

                inbuffer[n] = 0;
                // 1. 检测新用户
                InetAddr clientaddress(peer);
                std::string tips = clientaddress.ToString();
                std::string message = tips + inbuffer;
                LOG(LogLevel::DEBUG) << message;
                _handler_addr(clientaddress);
                // 2. 转发消息

                // nickname# message -> [ip:port]-nickname# message

                _handler_msg(_sockfd, message);
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
    handler_addr_t _handler_addr;
    handler_msg_t _handler_msg;
};

#endif
