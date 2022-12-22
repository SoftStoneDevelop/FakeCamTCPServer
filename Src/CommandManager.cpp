#pragma once

#include "CommandManager.hpp"

namespace FakeCamServer
{
	CommandManager::CommandManager(
		FakeCamera& camera, std::shared_ptr<ArrayPool::ArrayPool<char>> pool)
		: camera_{ camera }, pool_{ pool }
	{
		commands_.reserve(6);
		commands_.push_back(new GetLedColor());
		commands_.push_back(new SetLedColor());

		commands_.push_back(new GetLedState());
		commands_.push_back(new SetLedState());

		commands_.push_back(new GetLedRate());
		commands_.push_back(new SetLedRate());

		std::sort(commands_.begin(), commands_.end(), sortFunc);
	}

	CommandManager::~CommandManager()
	{
		for each (auto command in commands_)
		{
			delete command;
		}
	}

	bool CommandManager::sortFunc(BaseCommand* i, BaseCommand* j)
	{
		return (i->GetText() < j->GetText());
	}
}