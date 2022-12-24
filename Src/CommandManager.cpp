#pragma once

#include "CommandManager.hpp"

namespace FakeCamServer
{
	CommandManager::CommandManager(
		std::shared_ptr<FakeCamera> camera, std::shared_ptr<ArrayPool::MemoryOwnerFactory<char>> mof)
		: camera_{ std::move(camera) }, mof_{ std::move(mof) }
	{
		commands_.reserve(6);
		commands_.push_back(new GetLedColor());
		commands_.push_back(new SetLedColor());

		commands_.push_back(new GetLedState());
		commands_.push_back(new SetLedState());

		commands_.push_back(new GetLedRate());
		commands_.push_back(new SetLedRate());

		std::sort(commands_.begin(), commands_.end(), sortFunc);
		routine_ = new std::thread(&CommandManager::routineLoop, this);
	}

	CommandManager::~CommandManager()
	{
		run_ = false;
		cv_.notify_all();
		if (routine_->joinable())
			routine_->join();
		delete routine_;
		
		for each (auto command in commands_)
		{
			delete command;
		}
	}

	bool CommandManager::sortFunc(BaseCommand* i, BaseCommand* j)
	{
		return (i->GetText() < j->GetText());
	}

	void CommandManager::routineLoop()
	{
		while (run_ || !queue_.empty())
		{
			if (!queue_.empty())
			{
				CommandRequest request{};
				{
					std::lock_guard<std::mutex> lg(m_);
					request = std::move(queue_.front());
					queue_.pop();
				}

				auto command = request.command;
				command->Execute(std::move(request.args), request.argsSize, std::move(request.promise), *camera_, *mof_);
			}
			else
			{
				std::unique_lock<std::mutex> lock(m_);
				cv_.wait(lock, [&] { return !run_.load() || !queue_.empty(); });
			}
		}
	}

	std::future<CommandResponse> CommandManager::enqueue(
		ArrayPool::MemoryOwner<char> args,
		int argsSize,
		BaseCommand* command
	)
	{
		CommandRequest request{};
		request.args = std::move(args);
		request.argsSize = argsSize;
		request.command = command;
		request.promise = std::promise<CommandResponse>();
		auto future = request.promise.get_future();

		{
			std::lock_guard<std::mutex> lg(m_);
			queue_.push(std::move(request));
		}
		cv_.notify_all();

		return future;
	}
}