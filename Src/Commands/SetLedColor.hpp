#pragma once

#include <string>
#include "BaseCommand.hpp"

namespace FakeCamServer
{
	class SetLedColor : public BaseCommand
	{
	public:
		SetLedColor() noexcept;

		void Execute(
			ArrayPool::MemoryOwner<char> args,
			int argsSize,
			std::promise<CommandResponse> promise,
			FakeCamera& camera
		) override;
	};
}