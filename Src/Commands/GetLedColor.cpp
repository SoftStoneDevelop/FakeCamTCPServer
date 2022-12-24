#pragma once
#include "GetLedColor.hpp"

namespace FakeCamServer
{
	GetLedColor::GetLedColor() noexcept : BaseCommand("get-led-color", 0, 0)
	{
	}

	void GetLedColor::Execute(
		ArrayPool::MemoryOwner<char> args,
		int argsSize,
		std::promise<CommandResponse> promise,
		FakeCamera& camera
	)
	{

	}
}