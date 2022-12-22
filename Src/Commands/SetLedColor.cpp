#pragma once
#include "SetLedColor.hpp"

namespace FakeCamServer
{
	SetLedColor::SetLedColor() noexcept : BaseCommand("set-led-color", 3, 5)
	{
	}

	void SetLedColor::Execute(
		char* args, int lengthArgs, 
		char* result, int lengthResult,
		FakeCamera& camera
	)
	{

	}
}