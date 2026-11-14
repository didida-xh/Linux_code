#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <sys/socket.h>
#include "Mutex.hpp"
#include "UserManager.hpp"

class Route
{
public:
    Route() : _uma(std::make_unique<UserManager>())
    {
    }

    void CheckUser(const InetAddr &addr)
    {
        LockGuard lockguard(_lock);
        _uma->AddUser(addr);
    }

    void OfflineUser(const InetAddr &addr)
    {
        LockGuard lockguard(_lock);
        _uma->DelUser(addr);
    }

    void Broadcast(int sockfd, std::string message)
    {
        LockGuard lockguard(_lock);
        auto &users = _uma->Users();
        for (auto &user : users)
        {
            sendto(sockfd, message.c_str(), message.size(), 0, (struct sockaddr *)user.GetNetAddress(), user.Len());
        }
    }

    ~Route()
    {
    }

private:
    std::unique_ptr<UserManager> _uma;
    Mutex _lock;
};