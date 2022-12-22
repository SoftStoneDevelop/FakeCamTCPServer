#pragma once

#include <string>
#include "BaseCommand.hpp"

namespace FakeCamServer
{
	class GetLedColor : public BaseCommand
	{
	public:
		GetLedColor() noexcept;

		void Execute(
			char* args, int lengthArgs, 
			char* result, int lengthResult,
			FakeCamera& camera
		) override;
	};
}