#pragma once
#include "SetLedState.hpp"

namespace FakeCamServer
{
	SetLedState::SetLedState() noexcept : BaseCommand("set-led-state", 2, 3)
	{
	}

	void SetLedState::Execute(
		ArrayPool::MemoryOwner<char> args,
		int argsSize,
		std::promise<CommandResponse> promise,
		FakeCamera& camera
	)
	{

	}
}