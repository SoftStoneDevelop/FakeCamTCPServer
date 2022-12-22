#pragma once

#include "TCPServer.hpp"
#include "Camera.hpp"
#include "CommandManager.hpp"
#include <iostream>
#include<io.h>
#include<stdio.h>
#include<winsock2.h>

#pragma comment(lib,"ws2_32.lib") //Winsock Library

int main(int argc, char* argv[])
{
    try
    {
        FakeCamServer::FakeCamera camera;
        auto pool = std::make_shared<ArrayPool::ArrayPool<char>>();
        FakeCamServer::CommandManager manager(camera, pool);
        FakeCamServer::TCPServer server("127.0.0.1", "4823", pool);
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