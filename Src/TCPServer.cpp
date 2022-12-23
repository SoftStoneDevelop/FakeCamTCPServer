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
		std::shared_ptr<ArrayPool::MemoryOwnerFactory<char>> mof,
		std::shared_ptr<CommandManager> manager
	) noexcept
		: 
		host_{ std::move(host) },
		port_{ port },
		listenSocket_{ INVALID_SOCKET },
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
		bool sendFailed = false;

		auto recvbuf = mof_->rentMemory(4000);

		int positionMath = 0;
		int commandIndx = 0;
		auto& commands = manager_->getCommands();
		
		bool recvArg = false;
		ArrayPool::MemoryOwner<char> args = mof_->rentMemory(100);
		int argsSize = 0;

		bool skipToNewline = false;
		while (isRun_ && !sendFailed)
		{
			iResult = recv(socket, recvbuf.data(), recvbuf.size(), 0);
			if (iResult > 0) 
			{
				for (int i = 0; i < iResult; i++)
				{
					if (recvbuf.data()[i] == '\n')
					{
						if (skipToNewline)
						{
							skipToNewline = false;
							continue;
						}
						else if(!recvArg)
						{
							sendFailed = !sendResponce("unknown command\n", socket);
						}
					}

					if (recvArg)
					{
						if (reciveArgs(
							recvbuf, iResult, i,
							args, argsSize,
							recvArg,
							commands,
							commandIndx,
							skipToNewline,
							sendFailed,
							socket
						))
						{
							//send to command manager
							//auto future = manager_->enqueue()
							//wait answer from command manager
							//MemoryOwner<char> responce = future.get();

							//send
							//sendFailed = !sendResponce(std::move(responce), socket, iSendResult);
							if (sendFailed)
								break;
							commandIndx = 0;
							argsSize = 0;
						}
					}
					else
					{
						searchCommand(
							recvbuf, iResult, i,
							args, argsSize,
							recvArg,
							commands,
							commandIndx,
							positionMath
						);
					}
				}
			}
			else if (iResult == 0)
			{
				//Connection closing
				printf("Connection closed");
				closesocket(socket);
				return;
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

	bool TCPServer::reciveArgs(
		ArrayPool::MemoryOwner<char>& recived, int recivedSize, int& recivedIndx,
		ArrayPool::MemoryOwner<char>& args, int& argsSize,
		bool& recvArg,
		const std::vector<BaseCommand*>& commands,
		int& commandIndx,
		bool& skipToNewline,
		bool& sendFailed,
		SOCKET socket
	)
	{
		for (; recivedIndx < recivedSize; recivedIndx++)
		{
			if (recived.data()[recivedIndx] == ' ' && (argsSize == 0 || (recivedIndx >= 0 && recived.data()[recivedIndx - 1] == ' ')))
				continue;

			if (recived.data()[recivedIndx] == '\n')
			{
				if (argsSize < commands[commandIndx]->GetMinArgLength())
				{
					sendFailed = !sendResponce("Not enough arguments\n", socket);
					recvArg = false;
					return false;
				}
				else
				{
					recvArg = false;
					return true;
				}
			}

			if (argsSize == commands[commandIndx]->GetMaxArgLength())
			{
				sendFailed = !sendResponce("Wrong arguments length\n", socket);
				skipToNewline = true;
				return false;
			}

			args.data()[argsSize - 1] = recived.data()[recivedIndx];
			argsSize++;
		}

		return false;
	}

	void TCPServer::searchCommand(
		ArrayPool::MemoryOwner<char>& recived, int recivedSize, int& recivedIndx,
		ArrayPool::MemoryOwner<char>& args, int& argsSize,
		bool& recvArg,
		const std::vector<BaseCommand*>& commands,
		int& commandIndx,
		int& positionMath
	)
	{
		bool needRescan = false;
		if (!partFindCommand(
			recived, recivedSize, recivedIndx,
			args, argsSize,
			recvArg,
			commands,
			commandIndx,
			positionMath
			))
		{
			commandIndx = 0;
			needRescan = positionMath != 0;
			positionMath = 0;
			recvArg = false;
			argsSize = 0;
		}

		if (needRescan)
		{
			partFindCommand(
				recived, recivedSize, recivedIndx,
				args, argsSize,
				recvArg,
				commands,
				commandIndx,
				positionMath
			);
		}
	}

	bool TCPServer::partFindCommand(
		ArrayPool::MemoryOwner<char>& recived, int recivedSize, int& recivedIndx,
		ArrayPool::MemoryOwner<char>& args, int& argsSize,
		bool& recvArg,
		const std::vector<BaseCommand*>& commands,
		int& commandIndx,
		int& positionMath
	)
	{
		bool find = false;
		for (int j = commandIndx; j < commands.size(); j++)
		{
			auto command = commands[j];
			if (command->GetText().size() > positionMath && command->GetText()[positionMath] == recived.data()[recivedIndx])
			{
				commandIndx = j;
				positionMath++;
				find = true;
				if (command->GetText().size() == positionMath)
				{
					recvArg = true;
					args = std::move(mof_->rentMemory(command->GetMaxArgLength()));
					argsSize = 0;
					positionMath = 0;
				}
				break;
			}
		}

		return find;
	}

	bool TCPServer::sendResponce(
		ArrayPool::MemoryOwner<char> responce,
		SOCKET socket
	)
	{
		int iSendResult = send(socket, responce.data(), responce.size(), 0);
		if (iSendResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(socket);
			return false;
		}

		return true;
	}

	bool TCPServer::sendResponce(
		std::string&& responce,
		SOCKET socket
	)
	{
		int iSendResult = send(socket, responce.data(), responce.size(), 0);
		if (iSendResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(socket);
			return false;
		}

		return true;
	}
}//namespace FakeCamServer