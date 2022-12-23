#pragma once
#include <mutex>
#include <string>
#include <vector>
#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library

#include "MemoryOwnerFactory.hpp"
#include "CommandManager.hpp"

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

		SOCKET listenSocket_;
		std::atomic<bool> isRun_ = false;
		std::vector<std::thread*> clients_;

		std::mutex startMutex_;
		std::thread* listenThread_ = nullptr;

		std::shared_ptr<ArrayPool::MemoryOwnerFactory<char>> mof_;
		std::shared_ptr<CommandManager> manager_;

		void listenLoop();
		void clientLoop(SOCKET socket);

		bool reciveArgs(
			ArrayPool::MemoryOwner<char>& recived, int recivedSize, int& recivedIndx,
			ArrayPool::MemoryOwner<char>& args, int& argsSize,
			bool& recvArg,
			const std::vector<BaseCommand*>& commands,
			int& commandIndx,
			bool& skipToNewline,
			bool& sendFailed,
			SOCKET socket
			);

		void searchCommand(
			ArrayPool::MemoryOwner<char>& recived, int recivedSize, int& recivedIndx,
			ArrayPool::MemoryOwner<char>& args, int& argsSize,
			bool& recvArg,
			const std::vector<BaseCommand*>& commands,
			int& commandIndx,
			int& positionMath
		);

		bool partFindCommand(
			ArrayPool::MemoryOwner<char>& recived, int recivedSize, int& recivedIndx,
			ArrayPool::MemoryOwner<char>& args, int& argsSize,
			bool& recvArg,
			const std::vector<BaseCommand*>& commands,
			int& commandIndx,
			int& positionMath
		);

		bool sendResponce(
			ArrayPool::MemoryOwner<char> responce,
			SOCKET socket
		);

		bool sendResponce(
			std::string&& responce,
			SOCKET socket
		);
	};
}//namespace FakeCamServer