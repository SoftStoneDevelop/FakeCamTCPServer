#pragma once

#include <string>
#include "BaseCommand.hpp"

namespace FakeCamServer
{
	class GetLedState : public BaseCommand
	{
	public:
		GetLedState() noexcept;

		void Execute(
			ArrayPool::MemoryOwner<char> args,
			int argsSize,
			std::promise<CommandResponse> promise,
			FakeCamera& camera
		) override;
	};
}