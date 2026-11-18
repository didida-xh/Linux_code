#include "Shm.hpp"

int main()
{
    printf("aaaaaaaaaaaaa\n");
    printf("aaaaaaaaaaaaa\n");
    printf("aaaaaaaaaaaaa\n");
    printf("aaaaaaaaaaaaa\n");
    printf("aaaaaaaaaaaaa\n");
    printf("aaaaaaaaaaaaa\n");
    // 生命周期的代码级管理！
    Shm sharedmem;
    sharedmem.Create();
    sharedmem.Attach();

    sleep(2);

    sharedmem.PrintAttr();

    char *shm_start = (char *)sharedmem.Addr();
    int size = sharedmem.Size();

    while (true)
    {
        // 本质就是读取共享内存！
        // 临界区代码！
        for (int i = 0; i < size; i++)
        {
            std::cout << shm_start[i] << ' ';
        }
        std::cout << std::endl;
        sleep(1);
    }

    sharedmem.Detach();
    sharedmem.Delete();
    return 0;
}