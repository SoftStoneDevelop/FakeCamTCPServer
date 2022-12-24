#pragma once
#include "SetLedRate.hpp"

namespace FakeCamServer
{
	SetLedRate::SetLedRate() noexcept : BaseCommand("set-led-rate", 1, 1)
	{
	}

	void SetLedRate::Execute(
		ArrayPool::MemoryOwner<char> args,
		int argsSize,
		std::promise<CommandResponse> promise,
		FakeCamera& camera
	)
	{

	}
}