#include <iostream>
#include <string>
#include <hiredis.h>
#include <windows.h>
#include <sstream>
#include "DistLock.h"

// Redis连接信息 - 根据实际情况修改
const char* REDIS_HOST = "192.168.28.129";
const int REDIS_PORT = 6380;
const std::string SHARED_COUNTER = "counter:test";

// 工作进程函数（每个进程独立执行）
void worker_process(int process_num) {
    // 创建独立Redis连接
    redisContext* context = redisConnect(REDIS_HOST, REDIS_PORT);
    redisCommand(context, "123456");
    if (!context || context->err) {
        std::cerr << "Process " << process_num << " connect failed: "
            << (context ? context->errstr : "can't allocate context") << std::endl;
        exit(1);
    }

    DistLock& lock = DistLock::Init();
    const std::string LOCK_NAME = "winlock";

    // 尝试获取锁（等待5秒，锁持有10秒）
    std::string identifier = lock.acquireLock(context, LOCK_NAME, 10, 5);

    if (identifier.empty()) {
        std::cout << "[PID:" << GetCurrentProcessId() << "] Process " << process_num
            << " failed to acquire lock" << std::endl;
        redisFree(context);
        return;
    }

    // 临界区操作
    std::cout << "\n[PID:" << GetCurrentProcessId() << "] Process " << process_num
        << " acquired lock (" << identifier << ")\n";

    // 操作共享资源
    redisReply* reply = (redisReply*)redisCommand(context, "INCR %s", SHARED_COUNTER.c_str());
    if (reply && reply->type == REDIS_REPLY_INTEGER) 
    {
        std::cout << "Counter updated to: " << reply->integer << std::endl;
    }
    freeReplyObject(reply);

    // 模拟业务处理（Windows使用Sleep，单位毫秒）
    Sleep(2000);

    // 释放锁
    if (lock.releaseLock(context, LOCK_NAME, identifier)) 
    {
        std::cout << "[PID:" << GetCurrentProcessId() << "] Lock released\n";
    }
    else 
    {
        std::cerr << "!!! Release lock failed !!!\n";
    }

    redisFree(context);
}

int main(int argc, char* argv[]) 
{
    // 初始化测试环境
    if (argc == 1) { // 主进程
        redisContext* setup = redisConnect(REDIS_HOST, REDIS_PORT);
        if (setup) {
            redisCommand(setup, "DEL %s", SHARED_COUNTER.c_str());
            redisFree(setup);
        }

        const int PROCESS_COUNT = 3;
        PROCESS_INFORMATION pi[PROCESS_COUNT];

        // 修改后的进程创建代码
        for (int i = 0; i < PROCESS_COUNT; ++i) {
            std::stringstream cmd;
            cmd << argv[0] << " child " << i + 1;

            // 将string转换为可修改的char数组
            std::string cmdStr = cmd.str();
            char* cmdLine = new char[cmdStr.size() + 1];
            strcpy_s(cmdLine, cmdStr.size() + 1, cmdStr.c_str());

            STARTUPINFOA si = { sizeof(STARTUPINFOA) }; // 明确使用ANSI版本
            PROCESS_INFORMATION pi;

            // 使用CreateProcessA显式指定ANSI版本
            if (!CreateProcessA(
                NULL,          // 应用程序名
                cmdLine,       // 命令行参数（可修改的缓冲区）
                NULL,          // 进程安全属性
                NULL,          // 线程安全属性
                FALSE,         // 不继承句柄
                0,             // 无创建标志
                NULL,          // 使用父进程环境块
                NULL,          // 使用父进程工作目录
                &si,           // STARTUPINFOA结构
                &pi            // PROCESS_INFORMATION结构
            )) 
            {
                std::cerr << "CreateProcess failed: " << GetLastError() << std::endl;
                delete[] cmdLine;
                return 1;
            }

            delete[] cmdLine;  // 释放动态分配的内存
            CloseHandle(pi.hThread); // 关闭不需要的线程句柄
            CloseHandle(pi.hProcess); // 关闭不需要的进程句柄
        }

        // 等待所有子进程完成
        WaitForMultipleObjects(PROCESS_COUNT, &pi[0].hProcess, TRUE, INFINITE);

        // 清理句柄
        for (int i = 0; i < PROCESS_COUNT; ++i) 
        {
            CloseHandle(pi[i].hProcess);
            CloseHandle(pi[i].hThread);
        }

        // 验证结果
        redisContext* verify = redisConnect(REDIS_HOST, REDIS_PORT);
        if (verify) 
        {
            redisReply* reply = (redisReply*)redisCommand(verify, "GET %s", SHARED_COUNTER.c_str());
            if (reply && reply->type == REDIS_REPLY_STRING) 
            {
                std::cout << "\nFinal Counter Value: " << reply->str
                    << " (Expected: " << PROCESS_COUNT << ")" << std::endl;
            }
            freeReplyObject(reply);
            redisFree(verify);
        }
    }
    else 
    { // 子进程
        worker_process(atoi(argv[2]));
    }

    return 0;
}