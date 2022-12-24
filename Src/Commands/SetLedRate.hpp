#pragma once

#include <string>
#include "BaseCommand.hpp"

namespace FakeCamServer
{
	class SetLedRate : public BaseCommand
	{
	public:
		SetLedRate() noexcept;

		void Execute(
			ArrayPool::MemoryOwner<char> args,
			int argsSize,
			std::promise<CommandResponse> promise,
			FakeCamera& camera,
			const ArrayPool::MemoryOwnerFactory<char>& mof
		) override;
	};
}