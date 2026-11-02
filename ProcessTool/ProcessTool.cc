#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
#include <functional>
#include <unistd.h>
#include <sys/wait.h>

enum
{
    OK,
    FORK_ERROR,
    PIPE_ERROR
};

const int gprocessnum = 5;
using cb_t = std::function<void(int)>;

///////////////////////////////子进程要完成的任务/////////////////////////
void SyncDisk()
{
    std::cout << getpid() << ": 刷新数据到磁盘任务" << std::endl;
    sleep(1);
}

void Download()
{
    std::cout << getpid() << ": 下载数据到系统中" << std::endl;
    sleep(1);
}

void PrintLog()
{
    std::cout << getpid() << ": 打印日志到本地" << std::endl;
    sleep(1);
}

void UpdateStatus()
{
    std::cout << getpid() << ": 更新一次用户的状态" << std::endl;
    sleep(1);
}

typedef void (*task_t)();
task_t tasks[4] = {SyncDisk, Download, PrintLog, UpdateStatus};

// 子进程入口函数
void DoTask(int fd)
{
    while (true)
    {
        int task_node = 0;
        size_t n = read(fd, &task_node, sizeof(task_node));
        if (n = sizeof(task_node))
        {
            if (task_node >= 0 && task_node < 4)
            {
                tasks[task_node];
            }
        }
        else if (n == 0)
        {
            std::cout << getpid() << ": task quit ..." << std::endl;
            break;
        }
        else
        {
            perror("read");
            break;
        }
    }
}

class ProcessTool
{
public:
    ProcessTool()
    {
        srand((unsigned int)time(nullptr) ^ getpid());
    }
    ~ProcessTool()
    {
    }

    void Init(cb_t cb)
    {
        CreateProcessChannel(cb);
    }

    void Run()
    {
        int cnt = 10;
        while (cnt--)
        {
            std::cout << "------------------------------------------------" << std::endl;
            int itask = SelectTask();
            std::cout << "itask: " << itask << std::endl;
            int index = SelectChannel();
            std::cout << "index: " << index << std::endl;
            printf("发送 %d to %s\n", itask, channels[index].Name().c_str());
            SendTask_TO_Salver(itask, index);
            sleep(3);
        }
    }

    void Quit()
    {
        for (auto &channel : channels)
        {
            channel.ClosePipe();
            channel.Wait();
        }
    }

    void Debug()
    {
        for (auto &c : channels)
        {
            c.PrintInfo();
        }
    }

private:
    void CreateProcessChannel(cb_t cb)
    {
        for (int i = 0; i < gprocessnum; i++)
        {
            int pipefd[2] = {0};
            int n = pipe(pipefd);
            if (n < 0)
            {
                std::cerr << "pipe create error" << std::endl;
                exit(PIPE_ERROR);
            }
            pid_t id = fork();
            if (id < 0)
            {
                std::cerr << "fork error" << std::endl;
                exit(FORK_ERROR);
            }
            else if (id == 0)
            {
                if (!channels.empty())
                {
                    for (auto &channel : channels)
                        channel.ClosePipe();
                }

                close(pipefd[1]);
                cb(pipefd[0]);
                exit(OK);
            }
            else
            {
                close(pipefd[0]);
                channels.emplace_back(pipefd[1], id);
                std::cout << "创建子进程: " << id << " 成功..." << std::endl;
                sleep(1);
            }
        }
    }

    int SelectChannel()
    {
        static int index = 0;
        int selected = index;
        index++;
        index %= channels.size();
        return selected;
    }

    int SelectTask()
    {
        int itask = rand() % 4;
        return itask;
    }

    void SendTask_TO_Salver(int itask, int index)
    {
        if (itask >= 4 || itask < 0)
            return;
        if (index < 0 || index >= channels.size())
            return;

        channels[index].Write(itask);
    }

private:
    class channel
    {
    public:
        channel(int wfd, pid_t pid) : _wfd(wfd), _sub_pid(pid)
        {
            _sub_name = "channel_name" + std::to_string(_sub_pid);
        }
        ~channel()
        {
        }

        std::string Name()
        {
            return _sub_name;
        }

        void Write(int index)
        {
            size_t n = write(_wfd, &index, sizeof(index));
            (void)n;
        }

        void ClosePipe()
        {
            std::cout << "关闭wfd: " << _wfd << std::endl;
            close(_wfd);
        }

        void Wait()
        {
            pid_t rid = waitpid(_sub_pid, nullptr, 0);
            (void)rid;
            std::cout << "回收子进程: " << _sub_pid << std::endl;
        }

        void PrintInfo()
        {
            printf("wfd: %d, who: %d, channel name: %s\n", _wfd, _sub_pid, _sub_name.c_str());
        }

    private:
        int _wfd;
        pid_t _sub_pid;
        std::string _sub_name;
    };

private:
    std::vector<channel> channels;
};

int main()
{
    ProcessTool pp;
    pp.Init(DoTask);
    pp.Debug();

    pp.Run();
    pp.Quit();
    return 0;
}