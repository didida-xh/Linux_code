#pragma once

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

const std::string gcomfile = "./fifo";

#define ForRead 1
#define ForWrite 2

class Fifo
{
public:
    Fifo(const std::string &comfile = gcomfile) : _comfile(comfile), _mode(0666), _fd(-1)
    {
    }

    void Build()
    {
        if (IsExist())
            return;
        umask(0);
        int n = mkfifo(_comfile.c_str(), _mode);
        if (n < 0)
        {
            std::cerr << "mkfifo error: " << strerror(errno) << " errno: " << errno << std::endl;
            exit(1);
        }
        std::cerr << "mkfifo success: " << strerror(errno) << " errno: " << errno << std::endl;
    }

    void Open(int mode)
    {
        if (mode == ForRead)
        {
            _fd = open(_comfile.c_str(), O_RDONLY);
        }
        else if (mode == ForWrite)
            _fd = open(_comfile.c_str(), O_WRONLY);
        else
        {
        }

        if (_fd < 0)
        {
            std::cerr << "open error: " << strerror(errno) << " errno: " << errno << std::endl;
            exit(2);
        }
        else
        {
            std::cout << "open file success" << std::endl;
        }
    }

    void Send(const std::string &msgin)
    {
        size_t n = write(_fd, msgin.c_str(), sizeof(msgin));
        (void)n;
    }

    int Recv(std::string *msgout)
    {
        char buffer[128];
        ssize_t n = read(_fd, buffer, sizeof(buffer) - 1);
        if (n > 0)
        {
            buffer[n] = 0;
            *msgout = buffer;
            return n;
        }
        else if (n == 0)
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }

    void Delete()
    {
        if (!IsExist())
            return;

        int n = unlink(_comfile.c_str());
        (void)n;
        std::cout << "Unlink " << _comfile << std::endl;
    }

    ~Fifo()
    {
    }

private:
    bool IsExist()
    {
        struct stat st;
        int n = stat(_comfile.c_str(), &st);
        if (n == 0)
        {
            return true;
        }
        else
        {
            errno = 0;
            return false;
        }
    }

private:
    std::string _comfile;
    mode_t _mode;
    int _fd;
};