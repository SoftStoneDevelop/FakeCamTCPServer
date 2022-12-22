#pragma once
#define WIN32_LEAN_AND_MEAN

#include "TCPServer.hpp"
#include <windows.h>
#include <ws2tcpip.h>

namespace FakeCamServer
{
	TCPServer::TCPServer(
		std::string&& host,
		std::string&& port,
		std::shared_ptr<ArrayPool::ArrayPool<char>> arrayPool
	) noexcept
		: 
		host_{ std::move(host) },
		port_{ port },
		listenSocket_{ INVALID_SOCKET },
		arrayPool_{arrayPool}
	{
	}

	TCPServer::~TCPServer()
	{
		stop();
	}

	bool TCPServer::start(std::string* outErrorMessage)
	{
		std::lock_guard<std::mutex> lg(startMutex_);
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

		isRun_ = true;
		listenThread_ = new std::thread(&TCPServer::listenLoop, this);
	}

	void TCPServer::stop()
	{
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

		for (auto i = clients_.begin(); i != clients_.end(); i++)
		{
			auto& client = *i;
			if (client->joinable())
			{
				client->join();
			}

			delete client;
		}
	}

	void TCPServer::listenLoop()
	{
		printf("Start listen new connections.\n");
		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(listenSocket_, &readfds);

		int activity;
		timeval time;
		time.tv_sec = 10;
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

			SOCKET newClient = accept(listenSocket_, NULL, NULL);
			if (newClient == INVALID_SOCKET)
			{
				printf("accept failed with error: %d\n", WSAGetLastError());
				closesocket(listenSocket_);
				WSACleanup();
				return;
			}

			clients_.push_back(new std::thread(&TCPServer::clientLoop, this, newClient));
		}
		
		closesocket(listenSocket_);
		printf("End listen new connections.\n");
	}

	void TCPServer::clientLoop(SOCKET socket)
	{
		int iResult = 0;
		int iSendResult = 0;

		int realSize;
		char* recvbuf = arrayPool_->Rent(4000, realSize);
		while (isRun_)
		{
			iResult = recv(socket, recvbuf, realSize, 0);
			if (iResult > 0) 
			{
				/*iSendResult = send(socket, recvbuf, iResult, 0);
				if (iSendResult == SOCKET_ERROR) 
				{
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(socket);
					break;
				}*/
			}
			else if (iResult == 0)
			{
				//Connection closing
			}
			else 
			{
				printf("recv failed with error: %d\n", WSAGetLastError());
				closesocket(socket);
				return;
			}
		}

		iResult = shutdown(socket, SD_SEND);
		if (iResult == SOCKET_ERROR) 
		{
			printf("shutdown failed with error: %d\n", WSAGetLastError());
		}

		closesocket(socket);
	}
}//namespace FakeCamServer