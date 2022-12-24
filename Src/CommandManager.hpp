#pragma once

#include "Camera.hpp"
#include "MemoryOwnerFactory.hpp"
#include "MemoryOwner.hpp"

#include "Commands/BaseCommand.hpp"
#include "Commands/GetLedColor.hpp"
#include "Commands/SetLedColor.hpp"
#include "Commands/SetLedState.hpp"
#include "Commands/GetLedState.hpp"
#include "Commands/SetLedRate.hpp"
#include "Commands/GetLedRate.hpp"

#include <future>
#include <vector>
#include <queue>
#include <mutex>

namespace FakeCamServer
{
	class CommandManager
	{
	public:		
		struct CommandResponse
		{
			ArrayPool::MemoryOwner<char> response;
			int responseSize;
		};

		struct CommandRequest
		{
			ArrayPool::MemoryOwner<char> args;
			int argsSize;
			BaseCommand* command;
			std::promise<CommandResponse> promise;
		};

		CommandManager(std::shared_ptr<FakeCamera> camera, std::shared_ptr<ArrayPool::MemoryOwnerFactory<char>> mof);
		~CommandManager();

		CommandManager(const CommandManager& other) = delete;
		CommandManager(CommandManager&& other) = delete;

		CommandManager& operator=(CommandManager&& other) = delete;
		CommandManager& operator=(const CommandManager& other) = delete;

		const std::vector<BaseCommand*>& getCommands() const noexcept
		{
			return commands_;
		}

		std::future<CommandManager::CommandResponse> enqueue(
			ArrayPool::MemoryOwner<char> args,
			int argsSize,
			BaseCommand* command)
			;
	private:
		std::shared_ptr<FakeCamera> camera_;
		std::vector<BaseCommand*> commands_;
		std::shared_ptr<ArrayPool::MemoryOwnerFactory<char>> mof_;
		
		std::thread* routine_ = nullptr;
		std::queue<CommandRequest> queue_;
		std::mutex m_;
		std::condition_variable cv_;
		std::atomic<bool> run_ = true;

		static bool sortFunc(BaseCommand* i, BaseCommand* j);

		void routineLoop();
	};
}