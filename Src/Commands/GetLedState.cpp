#pragma once
#include "GetLedState.hpp"

namespace FakeCamServer
{
	GetLedState::GetLedState() noexcept : BaseCommand("get-led-state", 0, 0)
	{
	}

	void GetLedState::Execute(
		char* args, int lengthArgs, 
		char* result, int lengthResult,
		FakeCamera& camera
	)
	{

	}
}