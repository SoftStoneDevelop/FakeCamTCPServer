#pragma once
#include "GetLedRate.hpp"

namespace FakeCamServer
{
	GetLedRate::GetLedRate() noexcept : BaseCommand("get-led-rate", 0, 0)
	{
	}

	void GetLedRate::Execute(
		char* args, int lengthArgs, 
		char* result, int lengthResult,
		FakeCamera& camera
	)
	{

	}
}