#pragma once
#include "GetLedRate.hpp"

namespace FakeCamServer
{
	GetLedRate::GetLedRate() noexcept : BaseCommand("get-led-rate", 0, 0)
	{
	}

	void GetLedRate::Execute(
		ArrayPool::MemoryOwner<char> args,
		int argsSize,
		std::promise<CommandResponse> promise,
		FakeCamera& camera
	)
	{

	}
}