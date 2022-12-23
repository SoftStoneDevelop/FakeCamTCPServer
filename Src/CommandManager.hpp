#pragma once

#include "Camera.hpp"
#include "MemoryOwnerFactory.hpp"
#include <vector>

#include "Commands/BaseCommand.hpp"
#include "Commands/GetLedColor.hpp"
#include "Commands/SetLedColor.hpp"
#include "Commands/SetLedState.hpp"
#include "Commands/GetLedState.hpp"
#include "Commands/SetLedRate.hpp"
#include "Commands/GetLedRate.hpp"

#include <future>

namespace FakeCamServer
{
	class CommandManager
	{
	public:
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
	private:
		std::shared_ptr<FakeCamera> camera_;
		std::vector<BaseCommand*> commands_;
		std::shared_ptr<ArrayPool::MemoryOwnerFactory<char>> mof_;

		static bool sortFunc(BaseCommand* i, BaseCommand* j);
	};
}