#pragma once

#include <string>
#include "BaseCommand.hpp"

namespace FakeCamServer
{
	class SetLedState : public BaseCommand
	{
	public:
		SetLedState() noexcept;

		void Execute(
			ArrayPool::MemoryOwner<char> args,
			int argsSize,
			std::promise<CommandResponse> promise,
			FakeCamera& camera,
			const ArrayPool::MemoryOwnerFactory<char>& mof
		) override;
	};
}