#pragma once
#include "GetLedColor.hpp"

namespace FakeCamServer
{
	GetLedColor::GetLedColor() noexcept : BaseCommand("get-led-color", 0, 0)
	{
	}

	void GetLedColor::Execute(
		char* args, int lengthArgs, 
		char* result, int lengthResult,
		FakeCamera& camera
	)
	{

	}
}