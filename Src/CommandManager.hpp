#pragma once

#include "Camera.hpp"
#include "ArrayPool.hpp"
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
		CommandManager(FakeCamera& camera, std::shared_ptr<ArrayPool::ArrayPool<char>> pool);
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
		FakeCamera& camera_;
		std::vector<BaseCommand*> commands_;
		std::shared_ptr<ArrayPool::ArrayPool<char>> pool_;

		static bool sortFunc(BaseCommand* i, BaseCommand* j);
	};
}