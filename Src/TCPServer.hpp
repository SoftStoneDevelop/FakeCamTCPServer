#pragma once
#include <mutex>
#include <string>
#include <vector>
#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library

#include "ArrayPool.hpp"

namespace FakeCamServer
{
	class TCPServer
	{
	public:
		TCPServer(
			std::string&& host,
			std::string&& port,
			std::shared_ptr<ArrayPool::ArrayPool<char>> arrayPool
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

		SOCKET listenSocket_;
		std::atomic<bool> isRun_ = false;
		std::vector<std::thread*> clients_;

		std::mutex startMutex_;
		std::thread* listenThread_ = nullptr;

		std::shared_ptr<ArrayPool::ArrayPool<char>> arrayPool_;

		void listenLoop();
		void clientLoop(SOCKET socket);
	};
}//namespace FakeCamServer