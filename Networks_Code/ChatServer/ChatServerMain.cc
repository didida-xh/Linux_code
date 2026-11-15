#include "ThreadPool.hpp" // 执行者，执行处理动作的人
#include "Route.hpp"      // 任务
#include "UdpServer.hpp"  // 获取事件
#include <memory>

static void Usage(const std::string &process)
{
    std::cerr << "Usage:\n\t";
    std::cerr << process << " local_port" << std::endl;
}

using namespace NS_THREAD_POOL_MODULE;

using task_t = std::function<void()>;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    ENABLE_CONSOLE_LOG_STRATEGY();
    uint16_t server_port = std::stoi(argv[1]);

    // 线程池模块
    auto thread_pool = ThreadPool<task_t>::Instance();

    // 路由模块
    Route r;

    // 网络模块
    UdpServer usvr(server_port);
    usvr.Init();

    usvr.RegisterService(
        [&r](const InetAddr &addr)
        {
            r.CheckUser(addr);
        },
        [&r, thread_pool](int sockfd, std::string msg)
        {
            auto t = std::bind(&Route::Broadcast, &r, sockfd, msg);
            thread_pool->Enqueue(t);
            // thread_pool->Enqueue([&r, &sockfd, &msg](){
            //     r.Broadcast(sockfd, msg);
            // });
        });

    usvr.Start();

    return 0;
}