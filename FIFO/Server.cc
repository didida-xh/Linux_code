#include "Pipe.hpp"

int main()
{
    // 创建 && 打开
    Fifo pipefile;
    pipefile.Build();
    pipefile.Open(ForRead);
    std::string msg;

    while (true)
    {
        int n = pipefile.Recv(&msg);
        if(n > 0)
            std::cout << "Client Say# " << msg << std::endl;
        else
            break;
    }

    pipefile.Delete();

    return 0;
}