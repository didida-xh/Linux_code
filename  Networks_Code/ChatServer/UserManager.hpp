#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "InetAddr.hpp"
#include "Logger.hpp"

using namespace NS_LOG_MODULE;

class UserManager
{
public:
    UserManager()
    {
    }
    void AddUser(const InetAddr &addr)
    {
        if (SearchUser(addr))
            return;
        _users.push_back(addr);
    }

    void DelUser(const InetAddr &addr)
    {
        for (auto iter = _users.begin(); iter != _users.end(); iter++)
        {
            if (*iter == addr)
            {
                _users.erase(iter);
                break;
            }
        }
    }

    bool SearchUser(const InetAddr &addr)
    {
        for (auto &user : _users)
        {
            if (user == addr)
            {
                return true;
            }
        }
        return false;
    }

    bool ModUser(const InetAddr &addr)
    {
        DelUser(addr);
        AddUser(addr);
        return true;
    }
    std::vector<InetAddr> &Users()
    {
        return _users;
    }

private:
    std::vector<InetAddr> _users;
}