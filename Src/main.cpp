#pragma once

#include "TCPServer.hpp"
#include "Camera.hpp"
#include "CommandManager.hpp"
#include <iostream>
#include<io.h>
#include<stdio.h>
#include<winsock2.h>
#include "MemoryOwnerFactory.hpp"

#pragma comment(lib,"ws2_32.lib") //Winsock Library

int main(int argc, char* argv[])
{
    try
    {
        auto camera = std::make_shared<FakeCamServer::FakeCamera>();
        auto pool = std::make_shared<ArrayPool::ArrayPool<char>>();
        auto mof = std::make_shared<ArrayPool::MemoryOwnerFactory<char>>(std::move(pool));
        FakeCamServer::TCPServer server(
            "127.0.0.1",
            "4823",
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