#pragma once

#include <string.h>
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#elif defined(__linux__)
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <cerrno>
#endif

#include "TCPServer.hpp"
#include "RequestProcessor.hpp"

namespace FakeCamServer
{
	TCPServer::TCPServer(
		std::string&& host,
		std::string&& port,
		std::shared_ptr<ArrayPool::MemoryOwnerFactory<char>> mof,
		std::shared_ptr<CommandManager> manager
	) noexcept
		: 
		host_{ std::move(host) },
		port_{ port },
        threadPool_(1),
    #if defined(_WIN32)
        listenSocket_{ INVALID_SOCKET },
    #elif defined(__linux__)
        listenSocket_{ -1 },
    #endif
		mof_{std::move(mof)},
		manager_{std::move(manager)}
	{
	}

	TCPServer::~TCPServer()
	{
		stop();
	}

	bool TCPServer::start(std::string* outErrorMessage)
	{
        printf("Starting TCP Server...\n");
		std::lock_guard<std::mutex> lg(startMutex_);

#if defined(_WIN32)
        int iResult;
        WSADATA wsaData;

        printf("\nInitialising Winsock...");
        iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0)
        {
            if (outErrorMessage)
            {
                *outErrorMessage = "WSAStartup failed with error: " + iResult;
            }
            return false;
        }

        printf("Initialised.\n");

        addrinfo hints;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;

        addrinfo* result = NULL;
        iResult = getaddrinfo(NULL, port_.c_str(), &hints, &result);
        if (iResult != 0)
        {
            if (outErrorMessage)
            {
                *outErrorMessage = "getaddrinfo failed with error: " + iResult;
            }

            WSACleanup();
            return false;
        }

        listenSocket_ = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (listenSocket_ == INVALID_SOCKET)
        {
            if (outErrorMessage)
            {
                *outErrorMessage = "socket failed with error: " + WSAGetLastError();
            }

            freeaddrinfo(result);
            WSACleanup();
            return false;
        }

        iResult = bind(listenSocket_, result->ai_addr, (int)result->ai_addrlen);
        if (iResult == SOCKET_ERROR)
        {
            if (outErrorMessage)
            {
                *outErrorMessage = "bind failed with error: " + WSAGetLastError();
            }

            freeaddrinfo(result);
            closesocket(listenSocket_);
            WSACleanup();

            return false;
        }

        freeaddrinfo(result);

        iResult = listen(listenSocket_, SOMAXCONN);
        if (iResult == SOCKET_ERROR)
        {
            if (outErrorMessage)
            {
                *outErrorMessage = "listen failed with error: " + WSAGetLastError();
            }

            closesocket(listenSocket_);
            WSACleanup();
            return false;
        }

#elif defined(__linux__)
            int iResult;
            if( (listenSocket_ = socket(AF_INET , SOCK_STREAM , 0)) == 0)
            {
                if (outErrorMessage)
                {
                    *outErrorMessage = "socket failed with error: " + errno;
                }

                close(listenSocket_);
                return false;
            }

            addrinfo hints;
            memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;
            hints.ai_flags = AI_PASSIVE;

            listenSocketAdress_ = nullptr;
            iResult = getaddrinfo(NULL, port_.c_str(), &hints, &listenSocketAdress_);
            if (iResult != 0)
            {
                if (outErrorMessage)
                {
                    *outErrorMessage = "getaddrinfo failed with error: " + iResult;
                }

                close(listenSocket_);
                return false;
            }

            if (bind(listenSocket_, listenSocketAdress_->ai_addr, listenSocketAdress_->ai_addrlen) < 0)
            {
                freeaddrinfo(listenSocketAdress_);
                if (outErrorMessage)
                {
                    *outErrorMessage = "bind failed with error: " + errno;
                }

                close(listenSocket_);
                return false;
            }

            if (listen(listenSocket_, SOMAXCONN) < 0)
            {
                freeaddrinfo(listenSocketAdress_);
                if (outErrorMessage)
                {
                    *outErrorMessage = "listen failed with error: " + errno;
                }

                close(listenSocket_);
                return false;
            }


#endif

		isRun_ = true;
        listenThread_ = new std::thread(&TCPServer::listenLoop, this);
        printf("TCP Server started\n");
		return true;
	}

	void TCPServer::stop()
	{
        printf("Stopping TCP Server...\n");
		std::lock_guard<std::mutex> lg(startMutex_);
		if (!isRun_)
		{
			return;
		}

		isRun_ = false;
		//stop accepted new clients
		if (listenThread_ != nullptr)
		{
			listenThread_->join();
			delete listenThread_;
			listenThread_ = nullptr;
		}

#if defined(__linux__)
        freeaddrinfo(listenSocketAdress_);
#endif
        printf("TCP Server stopped\n");
	}

	void TCPServer::listenLoop()
	{
		printf("Start listen new connections.\n");
		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(listenSocket_, &readfds);

		int activity;
		timeval time;
		time.tv_sec = 5;
		time.tv_usec = 0;

		while (isRun_)
		{
			activity = select(listenSocket_ + 1, &readfds, NULL, NULL, &time);
			if (activity <=0)
			{
				FD_ZERO(&readfds);
				FD_SET(listenSocket_, &readfds);
				continue;
			}
#if defined(_WIN32)
            SOCKET newClient = accept(listenSocket_, NULL, NULL);
            if (newClient == INVALID_SOCKET)
            {
                printf("accept failed with error: %d\n", WSAGetLastError());
                closesocket(listenSocket_);
                WSACleanup();
                return;
            }
#elif defined(__linux__)
            auto newClient = accept(listenSocket_, NULL, NULL);
            if (newClient < 0)
            {
                printf("accept failed with error:\n");
                printf(strerror(errno));
                printf("\n");
                close(listenSocket_);
                return;
            }
#endif
            printf("accept new client\n");
            threadPool_.enqueue(&TCPServer::clientLoop, this, newClient);
		}
		
#if defined(_WIN32)
        closesocket(listenSocket_);
#elif defined(__linux__)
        close(listenSocket_);
#endif
		printf("End listen new connections.\n");
	}

    void TCPServer::clientLoop(
        #if defined(_WIN32)
                    SOCKET socket
        #elif defined(__linux__)
                    int socket
        #endif
            )
	{
		int iResult = 0;
		auto recvbuf = mof_->rentMemory(4000);
		auto sendbuf = mof_->rentMemory(4000);
		int needSendSize = 0;
		int iSendResult = 0;
		auto processor = RequestProcessor(manager_, mof_);

        while (isRun_ && iSendResult != -1)
		{
			iResult = recv(socket, recvbuf.data(), recvbuf.size(), 0);
			if (iResult > 0) 
			{
				processor.processData(recvbuf, iResult, sendbuf, needSendSize);
				while (needSendSize != 0)
				{
					iSendResult = send(socket, sendbuf.data(), needSendSize, 0);
                    if (iSendResult == -1)
					{
#if defined(_WIN32)
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(socket);
#elif defined(__linux__)
            printf("send failed\n");
            close(socket);
#endif
						needSendSize = 0;
						break;
					}
					needSendSize -= iSendResult;
				}
			}
			else if (iResult == 0)
			{
				//Connection closing
				printf("Connection closed");
#if defined(_WIN32)
                closesocket(socket);
#elif defined(__linux__)
                close(socket);
#endif
				return;
			}
			else 
			{
#if defined(_WIN32)
                printf("recv failed with error: %d\n", WSAGetLastError());
                closesocket(socket);
#elif defined(__linux__)
                printf("recv failed with error:\n");
                printf(strerror(errno));
                printf("\n");
                close(socket);
#endif
				return;
			}
		}

#if defined(_WIN32)
        iResult = shutdown(socket, SD_SEND);
        if (iResult == -1)
        {
            printf("shutdown failed with error: %d\n", WSAGetLastError());
        }

        closesocket(socket);
#elif defined(__linux__)
        iResult = shutdown(socket, SHUT_RD);
        if (iResult == -1)
        {
            printf("shutdown failed with error:\n");
            printf(strerror(errno));
            printf("\n");
        }

        close(socket);
#endif
	}
}//namespace FakeCamServer
