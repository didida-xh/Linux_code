#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <functional>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <pthread.h>
#include "InetAddr.hpp"
#include "Logger.hpp"
#include "ThreadPool.hpp"

using namespace NS_LOG_MODULE;
using namespace NS_THREAD_POOL_MODULE;

using task_t = std::function<void()>;

enum
{
    SUCESS = 0,
    USAGE_ERR,
    SOCKET_ERR,
    BIND_ERR,
    LISTEN_ERR,
    FORK_ERR
};

static const int gbacklog = 16;
static const uint16_t gport = 8888;

class TcpServer
{

public:
    TcpServer(uint16_t port = gport) : _port(port)
    {
    }

    void InitServer()
    {
        _listensockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (_listensockfd < 0)
        {
            LOG(LogLevel::FATAL) << "create socket error";
            exit(SOCKET_ERR);
        }
        LOG(LogLevel::DEBUG) << "create socket success: " << _listensockfd;

        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);
        local.sin_addr.s_addr = INADDR_ANY;

        int n = bind(_listensockfd, (struct sockaddr *)&local, sizeof(local));
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "bind error";
            exit(BIND_ERR);
        }

        n = listen(_listensockfd, gbacklog);
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "listen error";
            exit(LISTEN_ERR);
        }
        LOG(LogLevel::DEBUG) << "listen socket success";
    }

    void serviceIO(int sockfd, InetAddr address)
    {
        char inbuffer[1024] = {0};
        ssize_t n = read(sockfd, inbuffer, sizeof(inbuffer) - 1);
        if (n > 0)
        {
            inbuffer[n] = 0;
            LOG(LogLevel::INFO) << address.ToString() << " say# " << inbuffer;

            std::string echo_string = "server echo# ";
            echo_string += inbuffer;
            write(sockfd, echo_string.c_str(), echo_string.size());
        }
        else if (n == 0)
        {
            LOG(LogLevel::INFO) << "client quit, address: " << address.ToString();
        }
        else
        {
            LOG(LogLevel::ERROR) << "client read error, address: " << address.ToString();
        }

        close(sockfd);
    }

    class ThreadData
    {
    public:
        ThreadData(TcpServer *ts, int sockfd, const InetAddr &addr)
            : _this(ts), _sockfd(sockfd), _addr(addr)
        {
        }
        ~ThreadData()
        {
            close(_sockfd);
        }

    public:
        TcpServer *_this;
        int _sockfd;
        InetAddr _addr;
    };

    static void *thread_routine(void *args)
    {
        ThreadData *td = static_cast<ThreadData *>(args);
        pthread_detach(pthread_self());
        td->_this->serviceIO(td->_sockfd, td->_addr);
        delete td;
        return nullptr;
    }

    void Start()
    {
        while (true)
        {
            struct sockaddr_in clientaddr;
            socklen_t len = sizeof(clientaddr);

            int sockfd = accept(_listensockfd, (struct sockaddr *)&clientaddr, &len);
            if (sockfd < 0)
            {
                LOG(LogLevel::WARNING) << "accept errr!";
                continue;
            }
            LOG(LogLevel::DEBUG) << "accept success, sockfd: " << sockfd;
            // version 3: 接入线程池
            InetAddr clientaddress(clientaddr);
            ThreadPool<task_t>::Instance()->Enqueue([this, sockfd, clientaddress]() -> void
                                                    { this->serviceIO(sockfd, clientaddress); });
        }
    }
    ~TcpServer()
    {
        close(_listensockfd);
    }

private:
    uint16_t _port;
    int _listensockfd;
};