#pragma once

#include <iostream>
#include <stdio.h>

#include "TCPServer.hpp"
#include "Camera.hpp"
#include "CommandManager.hpp"
#include "MemoryOwnerFactory.hpp"
#include "ThreadPool.hpp"

void routine(int sec)
{
    std::this_thread::sleep_for(std::chrono::seconds(sec));
    printf("Bong\n");
}

int main(int argc, char* argv[])
{
    {
        FakeCamServer::ThreadPool pool(1);//create hread
        pool.enqueue(routine, 60);//nothing
        pool.enqueue(routine, 60);//create hread
        pool.enqueue(routine, 10);//create hread
        pool.enqueue(routine, 10);//create hread
        std::this_thread::sleep_for(std::chrono::seconds(12));
        pool.enqueue(routine, 5);//nothing
        pool.enqueue(routine, 5);//nothing
        std::this_thread::sleep_for(std::chrono::seconds(50));
        //delete 2 threads
        pool.enqueue(routine, 5);//nothing
        pool.enqueue(routine, 5);//nothing
        //only 4 threads created
        std::this_thread::sleep_for(std::chrono::seconds(50));
        //delete 1 threads
        //1 thread live in pool - because is minimal size
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    if (argc < 3)
    {
        printf("Must two parameters(now is %d): first IP and second PORT\n", argc);
        return EXIT_FAILURE;
    }

    printf("IP: %s\n", argv[1]);
    printf("PORT: %s\n", argv[2]);

    try
    {
        auto camera = std::make_shared<FakeCamServer::FakeCamera>();
        auto pool = std::make_shared<ArrayPool::ArrayPool<char>>();
        auto mof = std::make_shared<ArrayPool::MemoryOwnerFactory<char>>(std::move(pool));
        FakeCamServer::TCPServer server(
            argv[1],
            argv[2],
            std::move(mof),
            std::make_shared<FakeCamServer::CommandManager>(std::move(camera), mof)
        );
        server.start(nullptr);
        auto c = getchar();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}