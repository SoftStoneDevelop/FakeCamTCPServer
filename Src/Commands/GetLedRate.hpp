#pragma once

#include <string>
#include "BaseCommand.hpp"

namespace FakeCamServer
{
	class GetLedRate : public BaseCommand
	{
	public:
		GetLedRate() noexcept;

		void Execute(
			ArrayPool::MemoryOwner<char> args,
			int argsSize,
			std::promise<CommandResponse> promise,
			FakeCamera& camera
		) override;
	};
}