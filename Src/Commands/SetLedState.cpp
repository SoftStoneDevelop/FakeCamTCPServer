#pragma once
#include "SetLedState.hpp"

namespace FakeCamServer
{
	SetLedState::SetLedState() noexcept : BaseCommand("set-led-state", 2, 3)
	{
	}

	void SetLedState::Execute(
		char* args, int lengthArgs,
		char* result, int lengthResult,
		FakeCamera& camera
	)
	{

	}
}