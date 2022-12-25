#pragma once
#include <mutex>
#include <string>
#include <vector>

#include "MemoryOwnerFactory.hpp"
#include "CommandManager.hpp"

#if defined(_WIN32)
#include <windows.h>
#include <ws2tcpip.h>
#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#elif defined(__linux__)
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

namespace FakeCamServer
{
	class TCPServer
	{
	public:
		TCPServer(
			std::string&& host,
			std::string&& port,
			std::shared_ptr<ArrayPool::MemoryOwnerFactory<char>> mof,
			std::shared_ptr<CommandManager> manager
			) noexcept;
		~TCPServer();

		TCPServer(const TCPServer& other) = delete;
		TCPServer(TCPServer&& other) = delete;

		TCPServer& operator=(TCPServer&& other) = delete;
		TCPServer& operator=(const TCPServer& other) = delete;

		bool start(std::string* outErrorMessage);
		void stop();

		const std::string& getPort() const noexcept { return port_; }
		const std::string& getHost() const noexcept { return host_; }
	private:
		const std::string host_;
		const std::string port_;

#if defined(_WIN32)
SOCKET listenSocket_;
#elif defined(__linux__)
int listenSocket_;
addrinfo* listenSocketAdress_;
#endif
		std::atomic<bool> isRun_ = false;
		std::vector<std::thread*> clients_;

		std::mutex startMutex_;
		std::thread* listenThread_ = nullptr;

		std::shared_ptr<ArrayPool::MemoryOwnerFactory<char>> mof_;
		std::shared_ptr<CommandManager> manager_;

		void listenLoop();
#if defined(_WIN32)
        void clientLoop(SOCKET socket);
#elif defined(__linux__)
        void clientLoop(int socket);
#endif
	};
}//namespace FakeCamServer
