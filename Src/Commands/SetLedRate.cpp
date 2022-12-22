#pragma once
#include "SetLedRate.hpp"

namespace FakeCamServer
{
	SetLedRate::SetLedRate() noexcept : BaseCommand("set-led-rate", 1, 1)
	{
	}

	void SetLedRate::Execute(
		char* args, int lengthArgs,
		char* result, int lengthResult,
		FakeCamera& camera
	)
	{

	}
}