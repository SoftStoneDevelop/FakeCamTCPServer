#pragma once
#include "GetLedState.hpp"

namespace FakeCamServer
{
	GetLedState::GetLedState() noexcept : BaseCommand("get-led-state", 0, 0)
	{
	}

	void GetLedState::Execute(
		ArrayPool::MemoryOwner<char> args,
		int argsSize,
		std::promise<CommandResponse> promise,
		FakeCamera& camera
	)
	{

	}
}