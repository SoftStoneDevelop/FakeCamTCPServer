#pragma once
#include "SetLedColor.hpp"

namespace FakeCamServer
{
	SetLedColor::SetLedColor() noexcept : BaseCommand("set-led-color", 3, 5)
	{
	}

	void SetLedColor::Execute(
		ArrayPool::MemoryOwner<char> args,
		int argsSize,
		std::promise<CommandResponse> promise,
		FakeCamera& camera
	)
	{

	}
}