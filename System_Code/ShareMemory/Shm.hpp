#pragma once
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <sys/shm.h>

const int gsize = 128;

#define PATHNAME "/tmp"
#define PROJ_ID 0x66

class Shm
{
public:
    Shm() : _shmid(-1), _size(gsize), _start_addr(nullptr)
    {
    }

    void Delete()
    {
        int n = shmctl(_shmid, IPC_RMID, nullptr);
        (void)n;
    }

    void Attach()
    {
        _start_addr = shmat(_shmid, nullptr, 0);
        if (_start_addr == (void *)-1)
            exit(3);
    }

    void Detach()
    {
        int n = shmdt(_start_addr);
        (void)n;
    }

    void Get()
    {
        GetHelper(IPC_CREAT);
    }

    void Create()
    {
        GetHelper(IPC_CREAT | IPC_EXCL | 0666);
    }

    void *Addr()
    {
        return _start_addr;
    }

    int Size()
    {
        return _size;
    }

    void PrintAttr()
    {
        struct shmid_ds ds;
        int n = shmctl(_shmid, IPC_STAT, &ds);
        if (n < 0)
        {
            perror("shmctl");
            exit(4);
        }
        printf("key: 0x%x\n", ds.shm_perm.__key);
        printf("shm_nattch: %ld\n", ds.shm_nattch);
        printf("shm_segsz: 0x%lx\n", ds.shm_segsz);
    }

    ~Shm()
    {
    }

private:
    key_t GetKey()
    {
        return ftok(PATHNAME, PROJ_ID);
    }

    void GetHelper(int shmflg)
    {
        key_t k = GetKey();
        if (k < 0)
        {
            std::cerr << "GetKey error";
            exit(1);
        }

        _shmid = shmget(k, _size, shmflg);
        if (_shmid < 0)
        {
            perror("shmget");
            exit(2);
        }
        printf("key=0x%x, _shmid = %d\n", k, _shmid);
    }

private:
    int _shmid;
    int _size;
    void *_start_addr;
};