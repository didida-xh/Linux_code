#include "Shm.hpp"

int main()
{
    // 不需要创建内核级共享内存，当然也不需要删除
    Shm sharedmem;
    sharedmem.Get();
    sharedmem.Attach();

    sleep(2);

    sharedmem.PrintAttr();

    char *shm_start = (char *)sharedmem.Addr();
    int size = sharedmem.Size();
    int index = 0;
    while (true)
    {
        std::cout << "Please Enter@ ";
        std::cin >> *shm_start;

        shm_start++;

        index %= size;
        // sleep(1);
    }

    sharedmem.Detach();
    return 0;
}