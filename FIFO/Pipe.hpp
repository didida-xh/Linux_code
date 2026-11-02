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

const std::string gcomfile="./fifo";

#define ForRead 1
#define ForWrite 2


class Fifo
{
public:
Fifo(const std::string &comfile=gcomfile):_comfile(comfile),mode(0666),fd(-1)
{
    
}

    private:
    std::string _comfile;
    mode_t mode;
    int fd;
};