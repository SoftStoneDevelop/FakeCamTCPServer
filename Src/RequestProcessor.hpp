#pragma once

#include "../ArrayPool/Src/MemoryOwner.hpp"
#include "../ArrayPool/Src/MemoryOwnerFactory.hpp"
#include "CommandManager.hpp"

namespace FakeCamServer
{
	class RequestProcessor
	{
	public:
		RequestProcessor(
			std::shared_ptr<CommandManager> manager,
			std::shared_ptr<ArrayPool::MemoryOwnerFactory<char>> mof
		);
		~RequestProcessor();

		RequestProcessor(const RequestProcessor& other) = delete;
		RequestProcessor(RequestProcessor&& other) = delete;
		RequestProcessor& operator=(RequestProcessor&& other) = delete;
		RequestProcessor& operator=(const RequestProcessor& other) = delete;

		void processData(
			const ArrayPool::MemoryOwner<char>& requestBuffer,
			const int requestBufferSize,
			ArrayPool::MemoryOwner<char>& responce,
			int& responceSize
		);
	private:
		void searchCommand(
			const ArrayPool::MemoryOwner<char>& requestBuff,
			const int& requestSize,
			const int& requestIndx
		);

		bool partFindCommand(
			const ArrayPool::MemoryOwner<char>& requestBuff,
			const int& requestIndx
		);

		bool reciveArgs(
			const ArrayPool::MemoryOwner<char>& requestBuff,
			const int& requestSize,
			int& requestIndx,
			ArrayPool::MemoryOwner<char>& responce,
			int& responceSize
		);

		void fillBuffer(
			std::string&& responce,
			ArrayPool::MemoryOwner<char>& responceBuff,
			int& responceSize
		);

		void copyToResponseBuffer(
			CommandManager::CommandResponse response,
			ArrayPool::MemoryOwner<char>& responceBuff,
			int& responceSize
		);

		std::shared_ptr<CommandManager> manager_;
		std::shared_ptr<ArrayPool::MemoryOwnerFactory<char>> mof_;

		int positionMath_ = 0;
		int commandIndx_ = 0;
		ArrayPool::MemoryOwner<char>* args_;
		int argsSize_ = 0;

		bool skipToNewline_ = false;
		bool recvArg_ = false;
	};

}//namespace FakeCamServer